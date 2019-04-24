#include <cstring>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

#include "mrc.h"

MRC::MRC() :MRC("", false)
{
	//Do Nothing
}

MRC::MRC(const std::string &fileName) : MRC(fileName, false)
{
	open(fileName);
}

MRC::MRC(void * data, int width, int height, int slice, ImageDimensionType DimensionType, DataType dataType)
{
	(void)(DimensionType);
	if (dataType != DataType::Integer8) {
		//NOTICE: Only support byte type
		exit(-1);
	}
	m_header.mode = MRC_MODE_BYTE;
	m_d = MRCDataPrivate::create(data);
	m_header.nx = width;
	m_header.ny = height;
	m_header.nz = slice;
	m_header.mx = width;
	m_header.my = height;
	m_header.mz = slice;
	m_header.nxstart = 0;
	m_header.nystart = 0;
	m_header.nzstart = 0;
	m_header.mapc = 1;
	m_header.mapr = 2;
	m_header.maps = 3;
	m_header.ispg = 0;
	m_header.nsymbt = 0;
	m_header.xlen = 0;
	m_header.ylen = 0;
	m_header.zlen = 0;
	m_header.alpha = 90;
	m_header.beta = 90;
	m_header.gamma = 90;
	UpdateDminDmaxDmeanRMSHelper();
	//TODO::update MRCHeader
	m_opened = true;
}

MRC::MRC(void * data,
	int width,
	int height,
	int volumeZ,
	int volumeCount,
	VolumeDimensionType type,
	DataType dataType)
{
	(void)(data);
	(void)(width);
	(void)(height);
	(void)(volumeZ);
	(void)(volumeCount);
	(void)(type);
	(void)(dataType);
	exit(-1);
}

MRC::MRC(const MRC & otherMRC, void * data)
{
	m_header = otherMRC.m_header;
	m_d = MRCDataPrivate::create(data);
	//copyHeaderBuffer(hdBuffer, otherMRC.hdBuffer, MRC_HEADER_SIZE);
	memcpy(hdBuffer, otherMRC.hdBuffer, MRC_HEADER_SIZE);
	m_opened = true;
}

MRC::MRC(const MRC &rhs)
{
	m_header = rhs.m_header;
	m_d = rhs.m_d;
	++m_d->ref;		//reference count increasement
	memcpy(hdBuffer, rhs.hdBuffer, MRC_HEADER_SIZE);
	m_opened = rhs.m_opened;
}

MRC::MRC(MRC && rhs) noexcept
{
	m_header = std::move(rhs.m_header);
	m_d = rhs.m_d;
	rhs.m_d = nullptr;
	memcpy(hdBuffer, rhs.hdBuffer, MRC_HEADER_SIZE);
	m_opened = rhs.m_opened;
}

MRC & MRC::operator=(const MRC &rhs)
{
	if (this == &rhs)return *this;
	m_header = rhs.m_header;
	m_d = rhs.m_d;
	++m_d->ref;
	memcpy(hdBuffer, rhs.hdBuffer, MRC_HEADER_SIZE);
	m_opened = rhs.m_opened;
	return *this;
}

MRC & MRC::operator=(MRC && rhs) noexcept
{
	m_header = std::move(rhs.m_header);
	m_d = rhs.m_d;
	rhs.m_d = nullptr;
	m_opened = rhs.m_opened;
	return *this;
}

MRC MRC::fromData(void* data, int width, int height, int slice, DataType type)
{
	return MRC(data, width, height, slice, ImageDimensionType::ImageStack, type);
}
MRC MRC::fromMRC(const MRC & otherMRC, unsigned char * data)
{
	return MRC(otherMRC, data);
}
bool MRC::open(const std::string &fileName)
{
	detach();
	//FILE * fp = fopen(fileName.c_str(), "rb");
	std::ifstream inFile(fileName.c_str(), std::ifstream::binary);
	if (inFile.is_open()) {
		auto noError = true;
		if (noError) 
		{
			noError = headerReadHelper(inFile, &m_header);
		}
		if (noError) 
		{
			noError = readDataFromFileHelper(inFile);
		}
		m_opened = noError;
	}
	return m_opened;
}
bool MRC::save(const std::string & fileName, MRC::Format format)
{
	//FILE * fp = fopen(fileName.c_str(), "wb");
	std::ofstream out;

	if (!out.is_open() ) {
		std::cerr << "Cannot create file\n";
		return false;
	}
	const auto elemSize = typeSize(dataType());
	const auto dataCount = m_header.nx*m_header.ny*m_header.nz;
	if (format == Format::MRC) {
		headerWriteHelper(out, &m_header);
		//fseek(fp, MRC_HEADER_SIZE, SEEK_SET);
		//size_t count = fwrite(m_d->data, elemSize, dataCount, fp);
		out.seekp(MRC_HEADER_SIZE, out.beg);
		out.write(static_cast<char*>(m_d->data), elemSize*dataCount);
		const auto end = out.tellp();
		out.seekp(MRC_HEADER_SIZE, out.beg);
		const auto begin = out.tellp();
		const auto count = end - begin;

		if (count != dataCount*elemSize) {
			std::cerr << count << " of " << dataCount << " Byte(s) have(has) been written\n";
			return false;
		}
	}
	else if (format == Format::RAW) {
		//rewind(fp);
		//const auto count = fwrite(m_d->data, elemSize, dataCount, fp);
		//out.seekp(MRC_HEADER_SIZE, out.beg);
		out.write(static_cast<char*>(m_d->data), elemSize*dataCount);
		const auto end = out.tellp();
		out.seekp(0, out.beg);
		const auto begin = out.tellp();
		const auto count = end - begin;

		if (count != dataCount * elemSize) {
			std::cerr << count << " of " << dataCount << " Byte(s) have(has) been written\n";
			return false;
		}
	}
	return true;
}



std::string MRC::propertyName(int index)const
{
	static const char *name[] =
	{
		"NX:",
		"NY:",
		"NZ:",
		"MODE:",
		"NX START:" ,
		"NY START:" ,
		"NZ START:" ,
		"MX:" ,
		"MY:" ,
		"MZ:",
		"XLEN:" ,
		"YLEN:",
		"ZLEN:",
		"Alpha:",
		"Beta:",
		"Gamma:",
		"mapc:",
		"mapr:",
		"mapr:",
		"dmin:",
		"dmax:",
		"dmean:",
		"ispg:",
		"nysmbt:",
		"ExtTyp0:",
		"ExtTyp1:",
		"ExtTyp2:",
		"ExtTyp3:",
		"nVersion:",
		"xorg:",
		"yorg:",
		"zorg:"
	};
	return std::string(name[index]);
}
MRC::DataType MRC::propertyType(int index)const
{
	static const DataType types[] = {
		DataType::Integer32,		//x
		DataType::Integer32,		//y
		DataType::Integer32,		//z
		DataType::Integer32,		//mode
		DataType::Integer32,		//x start
		DataType::Integer32,		//y start
		DataType::Integer32,		//z start
		DataType::Integer32,		//mx
		DataType::Integer32,		//my
		DataType::Integer32,		//mz
		DataType::Real32,		//xlen
		DataType::Real32,		//ylen
		DataType::Real32,		//zlen
		DataType::Real32,			//alpha
		DataType::Real32,			//beta
		DataType::Real32,			//gamma
		DataType::Integer32,		//mapc
		DataType::Integer32,		//mapr
		DataType::Integer32,		//maps
		DataType::Real32,			//dmin
		DataType::Real32,			//dmax
		DataType::Real32,			//dmean
		DataType::Integer32,		//ispg
		DataType::Integer32,		//nysmbt
		DataType::Integer8,			//exttyp0
		DataType::Integer8,			//1
		DataType::Integer8,			//2
		DataType::Integer8,			//3
		DataType::Integer32,		//nversion
		DataType::Real32,			//xorg
		DataType::Real32,			//yorg
		DataType::Real32,			//zorg
	};

	return types[index];
}

float MRC::minValue() const
{
	return m_header.dmin;
}

float MRC::maxValue() const
{
	return m_header.dmax;
}

//const unsigned char *MRC::data() const
//{
//	return m_d->data;
//}
//
//unsigned char *MRC::data()
//{
//    return const_cast<unsigned char*>(
//                static_cast<const MRC *>(this)->data()
//                );
//}

MRC::DataType MRC::dataType() const
{
	switch (m_header.mode)
	{
	case MRC_MODE_BYTE:
		return MRC::DataType::Integer8;
	case MRC_MODE_FLOAT:
		return DataType::Real32;
	case MRC_MODE_COMPLEX_SHORT:
		return DataType::Complex16;
	case MRC_MODE_COMPLEX_FLOAT:
		return DataType::Complex32;
	case MRC_MODE_SHORT:
		return DataType::Integer16;
	case MRC_MODE_USHORT:
		return DataType::Integer16;
	default:
		return DataType();
	}
}

size_t MRC::typeSize(MRC::DataType type) const
{
	switch (type)
	{
	case DataType::Integer8:
		return sizeof(MRCInt8);
	case DataType::Integer16:
		return sizeof(MRCInt16);
	case DataType::Integer32:
		return sizeof(MRCInt32);
	case DataType::Complex16:
		return sizeof(MRCComplexShort);
	case DataType::Complex32:
		return sizeof(MRCComplexFloat);
	case DataType::Real32:
		return sizeof(MRCFloat);
	default:
		return 0;
	}
}


std::string MRC::info()const
{
	return propertyInfoString(&m_header);
}

MRC::~MRC()
{
	detach();
}


void MRC::UpdateDminDmaxDmeanRMSHelper()
{
	/*For efficency
	*DMAX < DMIN indicates that minimum and maximum of density are not determined.
	DMEAN < (smaller of DMIN and DMAX) indicates that mean value of the density is not determined.
	RMS < 0 indicates that RMS deviation of map from mean is not determined.
	*/
	m_header.dmax = 0;
	m_header.dmin = 1;
	m_header.dmean = -1;
	m_header.rms = -1;
}

//Only for byte and float data
bool MRC::headerReadHelper(std::ifstream & in, MRCHeader *hd)
{
	if (!in.is_open())
		return false;
	in.seekg(0, in.beg);
	//rewind(fp);
	//unsigned char hdBuffer[MRC_HEADER_SIZE];
	//int elemSize = 0;
	//elemSize = fread(hdBuffer, sizeof(unsigned char), MRC_HEADER_SIZE, fp);
	in.read((char*)hdBuffer, MRC_HEADER_SIZE);

	const auto elemSize = in.gcount();

	if (elemSize != MRC_HEADER_SIZE) {
		std::cerr << "ERROR:Read " << elemSize << " of " << MRC_HEADER_SIZE << std::endl;
		return false;
	}
	memcpy(&hd->nx, hdBuffer + NX_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->ny, hdBuffer + NY_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->nz, hdBuffer + NZ_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->mode, hdBuffer + MODE_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->nxstart, hdBuffer + NXSTART_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->nystart, hdBuffer + NYSTART_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->nzstart, hdBuffer + NZSTART_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->mx, hdBuffer + MX_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->my, hdBuffer + MY_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->mz, hdBuffer + MZ_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->xlen, hdBuffer + XLEN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->ylen, hdBuffer + YLEN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->zlen, hdBuffer + ZLEN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->alpha, hdBuffer + ALPHA_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->beta, hdBuffer + BETA_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->gamma, hdBuffer + GAMMA_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->mapc, hdBuffer + MAPC_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->mapr, hdBuffer + MAPR_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->maps, hdBuffer + MAPS_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->dmin, hdBuffer + DMIN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->dmax, hdBuffer + DMAX_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->dmean, hdBuffer + DMEAN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->ispg, hdBuffer + ISPG_OFFSET, sizeof(MRCInt32));
	memcpy(&hd->nsymbt, hdBuffer + NSYMBT_OFFSET, sizeof(MRCInt32));

	//memcpy(hd,hdBuffer,sizeof(24*4));
	memcpy(hd->extType, hdBuffer + EXTTYP_OFFSET, sizeof(char) * 4);
	memcpy(&hd->nversion, hdBuffer + NVERSION_OFFSET, sizeof(MRCInt32));

	memcpy(&hd->xorg, hdBuffer + XORIGIN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->yorg, hdBuffer + YORIGIN_OFFSET, sizeof(MRCFloat));
	memcpy(&hd->zorg, hdBuffer + ZORIGIN_OFFSET, sizeof(MRCFloat));

	return true;
}

bool MRC::headerWriteHelper(std::ofstream & out, MRCHeader * hd)
{
	if (!out.is_open())return false;
	unsigned char hdBuffer[MRC_HEADER_SIZE];
	memcpy(hdBuffer + NX_OFFSET, &hd->nx, sizeof(MRCInt32));
	memcpy(hdBuffer + NY_OFFSET, &hd->ny, sizeof(MRCInt32));
	memcpy(hdBuffer + NZ_OFFSET, &hd->nz, sizeof(MRCInt32));
	memcpy(hdBuffer + MODE_OFFSET, &hd->mode, sizeof(MRCInt32));
	memcpy(hdBuffer + NXSTART_OFFSET, &hd->nxstart, sizeof(MRCInt32));
	memcpy(hdBuffer + NYSTART_OFFSET, &hd->nystart, sizeof(MRCInt32));
	memcpy(hdBuffer + NZSTART_OFFSET, &hd->nzstart, sizeof(MRCInt32));
	memcpy(hdBuffer + MX_OFFSET, &hd->mx, sizeof(MRCInt32));
	memcpy(hdBuffer + MY_OFFSET, &hd->my, sizeof(MRCInt32));
	memcpy(hdBuffer + MZ_OFFSET, &hd->mz, sizeof(MRCInt32));
	memcpy(hdBuffer + XLEN_OFFSET, &hd->xlen, sizeof(MRCFloat));
	memcpy(hdBuffer + YLEN_OFFSET, &hd->ylen, sizeof(MRCFloat));
	memcpy(hdBuffer + ZLEN_OFFSET, &hd->zlen, sizeof(MRCFloat));
	memcpy(hdBuffer + ALPHA_OFFSET, &hd->alpha, sizeof(MRCFloat));
	memcpy(hdBuffer + BETA_OFFSET, &hd->beta, sizeof(MRCFloat));
	memcpy(hdBuffer + GAMMA_OFFSET, &hd->gamma, sizeof(MRCFloat));
	memcpy(hdBuffer + MAPC_OFFSET, &hd->mapc, sizeof(MRCInt32));
	memcpy(hdBuffer + MAPR_OFFSET, &hd->mapr, sizeof(MRCInt32));
	memcpy(hdBuffer + MAPS_OFFSET, &hd->maps, sizeof(MRCInt32));
	memcpy(hdBuffer + DMIN_OFFSET, &hd->dmin, sizeof(MRCFloat));
	memcpy(hdBuffer + DMAX_OFFSET, &hd->dmax, sizeof(MRCFloat));
	memcpy(hdBuffer + DMEAN_OFFSET, &hd->dmean, sizeof(MRCFloat));
	memcpy(hdBuffer + ISPG_OFFSET, &hd->ispg, sizeof(MRCInt32));
	memcpy(hdBuffer + NSYMBT_OFFSET, &hd->nsymbt, sizeof(MRCInt32));

	//memcpy(hd,hdBuffer,sizeof(24*4));
	memcpy(hdBuffer + EXTTYP_OFFSET, &hd->extType, sizeof(char) * 4);
	memcpy(hdBuffer + NVERSION_OFFSET, &hd->nversion, sizeof(MRCInt32));
	memcpy(hdBuffer + XORIGIN_OFFSET, &hd->xorg, sizeof(MRCFloat));
	memcpy(hdBuffer + YORIGIN_OFFSET, &hd->yorg, sizeof(MRCFloat));
	memcpy(hdBuffer + ZORIGIN_OFFSET, &hd->zorg, sizeof(MRCFloat));

	out.seekp(0, out.beg);
	//fwrite(hdBuffer, sizeof(unsigned char), MRC_HEADER_SIZE);
	out.write((char*)hdBuffer, MRC_HEADER_SIZE);

	return true;
}

std::string MRC::propertyInfoString(const MRCHeader *header) const
{
	std::stringstream ss;
	ss << "NX:" << header->nx << std::endl
		<< "NY:" << header->ny << std::endl
		<< "NZ:" << header->nz << std::endl
		<< "MODE:" << header->mode << std::endl
		<< "NX START:" << header->nxstart << std::endl
		<< "NY START:" << header->nystart << std::endl
		<< "NZ START:" << header->nzstart << std::endl
		<< "MX:" << header->mx << std::endl
		<< "MY:" << header->my << std::endl
		<< "MZ:" << header->my << std::endl
		<< "XLEN:" << header->xlen << std::endl
		<< "YLEN:" << header->ylen << std::endl
		<< "ZLEN:" << header->zlen << std::endl
		<< "Alpha:" << header->alpha << std::endl
		<< "Beta:" << header->beta << std::endl
		<< "Gamma:" << header->gamma << std::endl
		<< "mapc:" << header->mapc << std::endl
		<< "mapr:" << header->mapr << std::endl
		<< "mapr:" << header->maps << std::endl
		<< "dmin:" << header->dmax << std::endl
		<< "dmax:" << header->dmin << std::endl
		<< "dmean:" << header->dmean << std::endl
		<< "ispg:" << header->ispg << std::endl
		<< "nysmbt:" << header->nystart << std::endl
		<< "ExtTyp:" << header->extType[0] << " " << header->extType[1] << " " << header->extType[2] << " " << header->extType[3] << std::endl
		<< "nVersion:" << header->nversion << std::endl
		<< "xorg:" << header->xorg << std::endl
		<< "yorg:" << header->yorg << std::endl
		<< "zorg:" << header->zorg << std::endl;
	return ss.str();
}

bool MRC::readDataFromFileHelper(std::ifstream& in)
{
	bool noError = true;
	if (true == noError) {
		if (!in.is_open()) {
			noError = false;
		}
	}
	if (true == noError) {

		std::size_t offset = MRC_HEADER_SIZE + m_header.nsymbt;		/*Extented header size*/
		in.seekg(offset, in.beg);
		const size_t dataCount = static_cast<size_t>(m_header.nx)*static_cast<size_t>(m_header.ny)*static_cast<size_t>(m_header.nz);		// size_t is important
		const auto elemSize = typeSize(dataType());
		std::cout << "data count :" << dataCount << " element size:" << elemSize << std::endl;
		
		if (MRC_MODE_BYTE == m_header.mode) {
			//transform into byte8 type
			//this->m_d = MRCDataPrivate::create(m_header.nx, m_header.ny, m_header.nz, elemSize);

			//const auto begin = in.tellg();
			//in.seekg(0, in.end);
			//const auto end = in.tellg();
			//std::cout << "Max file size supported:"<<end-begin<<" bytes.\n";
			//in.seekg(MRC_HEADER_SIZE, in.beg);
			this->m_d = MRCDataPrivate::create(m_header.nx, m_header.ny, m_header.nz, elemSize);
			in.read((char*)m_d->data, dataCount*elemSize);
			const auto readCount = in.gcount();
			if (readCount != dataCount * elemSize) {
				std::cerr << "Runtime Error: Reading size error.>>>" << __LINE__ << std::endl;
				noError = false;
			}
		}
		else if (MRC_MODE_FLOAT == m_header.mode) {
			//float * buffer = new float[m_mrcDataSize*sizeof(float)];
			this->m_d = MRCDataPrivate::create(m_header.nx, m_header.ny, m_header.nz, elemSize);
			if (this->m_d == nullptr)
			{
				std::cerr << "Create data pointer failed\n" << __LINE__ << std::endl;
				return false;
			}
			//std::unique_ptr<MRCFloat[]> buffer(new float[dataCount]);
			in.read((char*)m_d->data, dataCount*elemSize);
			const auto readCount = in.gcount();
			//const size_t readCount = fread(this->m_d->data, elemSize, dataCount, fp);

			if (readCount != dataCount*elemSize)
			{
				std::cerr << "Runtime Error: Reading size error. Read Count:" << readCount << ". DataCount: " << dataCount << ".>>> " << __LINE__ << std::endl;
				noError = false;
			}
			m_header.mode = MRC_MODE_FLOAT;

		}
		else if (MRC_MODE_SHORT == m_header.mode || MRC_MODE_USHORT == m_header.mode) {
			//std::unique_ptr<MRCInt16[]> buffer(new MRCInt16[dataCount * sizeof(MRCInt16)]);
			this->m_d = MRCDataPrivate::create(m_header.nx, m_header.ny, m_header.nz, typeSize(DataType::Integer16));
			if(!this->m_d)
			{
				std::cerr << "Create data pointer failed. " << __LINE__ << std::endl;
				noError = true;
				return false;
			}
			in.read(reinterpret_cast<char*>(m_d->data), dataCount*elemSize);
			const auto readCount = in.gcount();
			//const size_t readCount = fread(buffer.get(), elemSize, dataCount, fp);
			if (readCount != dataCount * elemSize) 
			{
				std::cerr << "Runtime Error: Reading size error.>>> " << __LINE__ << std::endl;
				noError = false;
			}
			//if (true == noError) {
			//	const auto dmin = static_cast<MRCInt16>(m_header.dmin);
			//	const auto dmax = static_cast<MRCInt16>(m_header.dmax);
			//	const auto k = 256.0 / (dmax - dmin);
			//	for (size_t i = 0; i < dataCount; i++)
			//		static_cast<MRCInt8*>(m_d->data)[i] = static_cast<MRCInt16>(k*buffer[i]);
			//}
			//m_header.mode = MRC_MODE_BYTE;
		}
		else 
		{
			std::cerr << "Unsupported Format now.>>> " << __LINE__ << std::endl;
			return false;
		}
	}

	return (noError);
}

