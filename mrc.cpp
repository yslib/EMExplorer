#include "mrc.h"
#include <cstring>
#include <cassert>
#include <sstream>
#include <iostream>

MRC::MRC():MRC("",false)
{
    //Do Nothing
}

MRC::MRC(const std::string &fileName):MRC(fileName,false)
{
    open(fileName);
}

MRC::MRC(void * data, int width, int height, int slice, ImageDimensionType DimensionType, DataType dataType)
{

	//DataType
	if (dataType != DataType::Integer8) {
		//NOTICE: Only support byte type
		exit(-1);
	}
	m_header.mode = MRC_MODE_BYTE;

	//There is a critical fault to reference a outer raw pointer directly
	//This is a ad hoc.
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

	_dmin_dmax_dmean_rms_Field();
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

	exit(-1);
}

MRC::MRC(const MRC & otherMRC, void * data)
{
	m_header = otherMRC.m_header;
	m_d = MRCDataPrivate::create(data);
	m_opened = true;
}

MRC::MRC(const MRC &rhs)
{
    m_header = rhs.m_header;
	m_d = rhs.m_d;
	++m_d->ref;		//reference count increasement
    m_opened = rhs.m_opened;
}

MRC & MRC::operator=(const MRC &rhs)
{
    if(this == &rhs)return *this;
    m_header = rhs.m_header;
	m_d = rhs.m_d;
	++m_d->ref;
    m_opened = rhs.m_opened;
    return *this;
}

MRC MRC::fromData(void* data, int width, int height, int slice, DataType type)
{
	return MRC(data,width,height,slice,ImageDimensionType::ImageStack,type);
}



MRC MRC::fromMRC(const MRC & otherMRC,unsigned char * data)
{
    return MRC(otherMRC,data);
}

bool MRC::open(const std::string &fileName)
{
	detach();

    FILE * fp = fopen(fileName.c_str(),"rb");
    if(fp != nullptr){
        bool noError = true;
        if(true == noError){
            noError = _mrcHeaderRead(fp,&m_header);
        }
        if(true == noError){
            noError = _readDataFromFile(fp);
        }
        m_opened = noError;
        fclose(fp);
    }
    return m_opened;
}

bool MRC::save(const std::string & fileName, MRC::Format format)
{
	FILE * fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
		std::cerr << "Cannot create file\n";
		return false;
	}
	size_t elemSize = typeSize(type());
	size_t dataCount = m_header.nx*m_header.ny*m_header.nz;
	if (format == Format::MRC) {
		_mrcHeaderWrite(fp, &m_header);
		fseek(fp, MRC_HEADER_SIZE, SEEK_SET);
		size_t count = fwrite(m_d->data, elemSize, dataCount, fp);
		if (count != dataCount) {
			std::cerr << count << " of " << dataCount << " Byte(s) have(has) been written\n";
			fclose(fp);
			return false;
		}
	}
	else if (format == Format::RAW) {
		rewind(fp);
		size_t count = fwrite(m_d->data, elemSize, dataCount, fp);
		if (count != dataCount) {
			std::cerr << count << " of " << dataCount << " Byte(s) have(has) been written\n";
			fclose(fp);
			return false;
		}
	}
	fclose(fp);
	return true;
}

bool MRC::isOpened() const
{
    return m_opened;
}

int MRC::getWidth() const
{
    return m_header.nx;
}

int MRC::getHeight() const
{
    return m_header.ny;
}

int MRC::getSliceCount() const
{
    return m_header.nz;
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

MRC::DataType MRC::type() const
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


std::string MRC::getMRCInfo()const
{
	return _getMRCHeaderInfo(&m_header);
}

MRC::~MRC()
{
	detach();
}


void MRC::_dmin_dmax_dmean_rms_Field()
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
bool MRC::_mrcHeaderRead(FILE * fp, MRCHeader *hd)
{
    if(fp == nullptr)return false;
    rewind(fp);

    unsigned char hdBuffer[MRC_HEADER_SIZE];
    int elemSize = 0;
    elemSize = fread(hdBuffer,sizeof(unsigned char),MRC_HEADER_SIZE,fp);
    if(elemSize != MRC_HEADER_SIZE){
        std::cerr<<"ERROR:Read "<<elemSize<<" of "<<MRC_HEADER_SIZE<<std::endl;
        return false;
    }
    memcpy(&hd->nx,hdBuffer+NX_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->ny,hdBuffer+NY_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->nz,hdBuffer+NZ_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->mode,hdBuffer+MODE_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->nxstart,hdBuffer+NXSTART_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->nystart,hdBuffer+NYSTART_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->nzstart,hdBuffer+NZSTART_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->mx,hdBuffer+MX_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->my,hdBuffer+MY_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->mz,hdBuffer+MZ_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->xlen,hdBuffer+XLEN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->ylen,hdBuffer+YLEN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->zlen,hdBuffer+ZLEN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->alpha,hdBuffer+ALPHA_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->beta,hdBuffer+BETA_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->gamma,hdBuffer+GAMMA_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->mapc,hdBuffer+MAPC_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->mapr,hdBuffer+MAPR_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->maps,hdBuffer+MAPS_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->dmin,hdBuffer+DMIN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->dmax,hdBuffer+DMAX_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->dmean,hdBuffer+DMEAN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->ispg,hdBuffer+ISPG_OFFSET,sizeof(MRCInt32));
    memcpy(&hd->nsymbt,hdBuffer+NSYMBT_OFFSET,sizeof(MRCInt32));

    //memcpy(hd,hdBuffer,sizeof(24*4));
    memcpy(hd->extType,hdBuffer+EXTTYP_OFFSET,sizeof(char)*4);
    memcpy(&hd->nversion,hdBuffer+NVERSION_OFFSET,sizeof(MRCInt32));

    memcpy(&hd->xorg,hdBuffer+XORIGIN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->yorg,hdBuffer+YORIGIN_OFFSET,sizeof(MRCFloat));
    memcpy(&hd->zorg,hdBuffer+ZORIGIN_OFFSET,sizeof(MRCFloat));

    return true;
}

bool MRC::_mrcHeaderWrite(FILE * fp, MRCHeader * hd)
{
	if (fp == nullptr)return false;
	unsigned char hdBuffer[MRC_HEADER_SIZE];
	memcpy(hdBuffer + NX_OFFSET, &hd->nx, sizeof(MRCInt32));
	memcpy(hdBuffer + NY_OFFSET, &hd->ny,sizeof(MRCInt32));
	memcpy(hdBuffer + NZ_OFFSET, &hd->nz,sizeof(MRCInt32));
	memcpy(hdBuffer + MODE_OFFSET, &hd->mode,sizeof(MRCInt32));
	memcpy(hdBuffer + NXSTART_OFFSET, &hd->nxstart, sizeof(MRCInt32));
	memcpy(hdBuffer + NYSTART_OFFSET, &hd->nystart,  sizeof(MRCInt32));
	memcpy(hdBuffer + NZSTART_OFFSET, &hd->nzstart, sizeof(MRCInt32));
	memcpy(hdBuffer + MX_OFFSET, &hd->mx,  sizeof(MRCInt32));
	memcpy(hdBuffer + MY_OFFSET, &hd->my,  sizeof(MRCInt32));
	memcpy(hdBuffer + MZ_OFFSET, &hd->mz,  sizeof(MRCInt32));
	memcpy(hdBuffer + XLEN_OFFSET, &hd->xlen, sizeof(MRCFloat));
	memcpy(hdBuffer + YLEN_OFFSET, &hd->ylen,  sizeof(MRCFloat));
	memcpy(hdBuffer + ZLEN_OFFSET, &hd->zlen,  sizeof(MRCFloat));
	memcpy(hdBuffer + ALPHA_OFFSET, &hd->alpha,sizeof(MRCFloat));
	memcpy(hdBuffer + BETA_OFFSET, &hd->beta, sizeof(MRCFloat));
	memcpy(hdBuffer + GAMMA_OFFSET, &hd->gamma,sizeof(MRCFloat));
	memcpy(hdBuffer + MAPC_OFFSET, &hd->mapc, sizeof(MRCInt32));
	memcpy(hdBuffer + MAPR_OFFSET, &hd->mapr,  sizeof(MRCInt32));
	memcpy(hdBuffer + MAPS_OFFSET, &hd->maps, sizeof(MRCInt32));
	memcpy(hdBuffer + DMIN_OFFSET, &hd->dmin,  sizeof(MRCFloat));
	memcpy(hdBuffer + DMAX_OFFSET, &hd->dmax, sizeof(MRCFloat));
	memcpy(hdBuffer + DMEAN_OFFSET, &hd->dmean, sizeof(MRCFloat));
	memcpy(hdBuffer + ISPG_OFFSET, &hd->ispg,sizeof(MRCInt32));
	memcpy(hdBuffer + NSYMBT_OFFSET, &hd->nsymbt, sizeof(MRCInt32));

	//memcpy(hd,hdBuffer,sizeof(24*4));
	memcpy(hdBuffer + EXTTYP_OFFSET, &hd->extType,  sizeof(char) * 4);
	memcpy(hdBuffer + NVERSION_OFFSET, &hd->nversion, sizeof(MRCInt32));
	memcpy(hdBuffer + XORIGIN_OFFSET, &hd->xorg,sizeof(MRCFloat));
	memcpy(hdBuffer + YORIGIN_OFFSET, &hd->yorg,  sizeof(MRCFloat));
	memcpy(hdBuffer + ZORIGIN_OFFSET, &hd->zorg,sizeof(MRCFloat));
	rewind(fp);
	fwrite(hdBuffer, sizeof(unsigned char), MRC_HEADER_SIZE, fp);
	return true;
}

std::string MRC::_getMRCHeaderInfo(const MRCHeader *header) const
{

    std::stringstream ss;
    ss<<"NX:"<<header->nx<<std::endl
    <<"NY:"<<header->ny<<std::endl
    <<"NZ:"<<header->nz<<std::endl
    <<"MODE:"<<header->mode<<std::endl
    <<"NX START:"<<header->nxstart<<std::endl
    <<"NY START:"<<header->nystart<<std::endl
    <<"NZ START:"<<header->nzstart<<std::endl
    <<"MX:"<<header->mx<<std::endl
    <<"MY:"<<header->my<<std::endl
    <<"MZ:"<<header->my<<std::endl
    <<"XLEN:"<<header->xlen<<std::endl
    <<"YLEN:"<<header->ylen<<std::endl
    <<"ZLEN:"<<header->zlen<<std::endl
    <<"Alpha:"<<header->alpha<<std::endl
    <<"Beta:"<<header->beta<<std::endl
    <<"Gamma:"<<header->gamma<<std::endl
    <<"mapc:"<<header->mapc<<std::endl
    <<"mapr:"<<header->mapr<<std::endl
    <<"mapr:"<<header->maps<<std::endl
    <<"dmin:"<<header->dmax<<std::endl
    <<"dmax:"<<header->dmin<<std::endl
    <<"dmean:"<<header->dmean<<std::endl
    <<"ispg:"<<header->ispg<<std::endl
    <<"nysmbt:"<<header->nystart<<std::endl
    <<"ExtTyp:"<<header->extType[0]<<" "<<header->extType[1]<<" "<<header->extType[2]<<" "<<header->extType[3]<<std::endl
    <<"nVersion:"<<header->nversion<<std::endl
    <<"xorg:"<<header->xorg<<std::endl
    <<"yorg:"<<header->yorg<<std::endl
    <<"zorg:"<<header->zorg<<std::endl;
    return ss.str();
}

bool MRC::_readDataFromFile(FILE *fp)
{
    bool noError = true;
    if(true == noError){
        if(fp == nullptr){
            noError = false;
        }
    }
    if(true == noError){
        fseek(fp,MRC_HEADER_SIZE,SEEK_SET);

		const size_t dataCount = m_header.nx*m_header.ny*m_header.nz;
		const size_t elemSize = typeSize(type());

		//transform into byte8 type
		MRCDataPrivate * d = MRCDataPrivate::create(m_header.nx, m_header.ny, m_header.nz, typeSize(DataType::Integer8));

        if(MRC_MODE_BYTE == m_header.mode){
            const int readCount = fread(m_d->data,elemSize,dataCount,fp);
            if(readCount != dataCount){
                std::cerr<<"Runtime Error: Reading size error."<<__LINE__<<std::endl;
                noError =false;
            }

        }else if(MRC_MODE_FLOAT == m_header.mode){
            //float * buffer = new float[m_mrcDataSize*sizeof(float)];
            std::unique_ptr<MRCFloat[]> buffer(new float[dataCount*sizeof(MRCFloat)]);
            const int readCount = fread(buffer.get(),elemSize,dataCount,fp);
            if(readCount != dataCount){
                std::cerr<<"Runtime Error: Reading size error."<<__LINE__<<std::endl;
                noError = false;
            }
            if(true == noError){
                //Mapping float type to unsigned char type
                float dmin = static_cast<float>(m_header.dmin);
                float dmax = static_cast<float>(m_header.dmax);
                double k = 256.0/(dmax-dmin);
                for(size_t i =0;i<dataCount;i++){
                    static_cast<MRCInt8*>(m_d->data)[i] = static_cast<MRCInt8>(k*buffer[i]);
                }
            }
            //delete[] buffer;
        }else{
            std::cerr<<"Only float and byte type are supported now.";
            return false;
        }
    }

    return (noError);
}

void MRC::detach()
{
	if (m_d != nullptr && --m_d->ref == 0)
		delete m_d;
}

//void MRC::_reset()
//{
//	m_fileName = "";
//    //_destroy();
//    bool m_opened = false;
//    m_mrcDataSize = 0;
//}
//bool MRC::_init()
//{
//    size_t nx = static_cast<size_t>(m_header.nx);
//    size_t ny = static_cast<size_t>(m_header.ny);
//    size_t nz = static_cast<size_t>(m_header.nz);
//    m_mrcDataSize = nx*ny*nz;
//    return true;
//}
//
//bool MRC::_allocate()
//{
//    m_mrcData = new unsigned char[m_mrcDataSize*sizeof(unsigned char)];
//    if(m_mrcData == nullptr){
//        return false;
//    }
//    return true;
//}
//
//void MRC::_destroy()
//{
//    if(m_mrcData != nullptr){
//        delete[] m_mrcData;
//        m_mrcData = nullptr;
//    }
//}

