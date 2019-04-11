//#include "autommrcdatamodel.h"
//
//
//#include <fstream>
//#include <climits>
//#include <cassert>
//
//bool MrcReadHead(MrcHeader* header, const char* filename)
//{
//	std::ifstream in;
//	in.open(filename);
//	if (!in.good()) {
//		return false;
//	}
//
//	in.read((char*)header, sizeof(MrcHeader));
//	if (!(header->cmap[0] == 'M'&&header->cmap[1] == 'A'&&header->cmap[2] == 'P')) {
//		in.close();
//		return false;
//	}
//
//	in.close();
//	return true;
//}
//
//bool MrcWriteHead(const MrcHeader& header, const char* filename)
//{
//	std::ofstream out;
//	out.open(filename, std::ios::out | std::ios::ate);
//	out.seekp(std::ios::beg);
//	out.write(reinterpret_cast<const char*>(&header), sizeof(MrcHeader));
//	out.close();
//}
//
//void MrcInitHead(MrcHeader *head)
//{
//	head->nx = 1;
//	head->ny = 1;
//	head->nz = 1;
//
//	head->mode = MRC_MODE_BYTE;
//
//	head->nxstart = 0;
//	head->nystart = 0;
//	head->nzstart = 0;
//
//	head->mx = 1;
//	head->my = 1;
//	head->mz = 1;
//
//	head->xlen = 1;
//	head->ylen = 1;
//	head->zlen = 1;
//
//	head->alpha = 90;
//	head->beta = 90;
//	head->gamma = 90;
//
//	head->mapc = 1;
//	head->mapr = 2;
//	head->maps = 3;
//
//	head->amin = 0;
//	head->amax = 255;
//	head->amean = 128;
//
//	head->ispg = 1;
//	head->nsymbt = 0;
//
//	head->next = 0;
//
//	head->creatid = 1000;
//	head->cmap[0] = 'M';
//	head->cmap[1] = 'A';
//	head->cmap[2] = 'P';
//	head->stamp[0] = 'D';
//}
//
///**slcN couts from 0 to N-1, so if you want to read the first slice slcN shoud be 0*/
//bool MrcReadSlice(const char* filename, int slcN, char axis, float *slcdata)
//{
//
//	std::ifstream in(filename);
//	if (!in.good()) {
//		return false;
//	}
//	MRCheader head;
//	in.read((char*)&head, sizeof(MrcHeader));
//
//	int psize;
//
//	switch (head.mode) {
//	case MRC_MODE_BYTE:
//		psize = sizeof(unsigned char);
//		break;
//	case MRC_MODE_SHORT:
//		psize = sizeof(short);
//		break;
//	case MRC_MODE_FLOAT:
//		psize = sizeof(float);
//		break;
//	}
//
//	short buf_short;
//	unsigned char buf_byte;
//	float buf_float;
//	char* vbuf;
//
//	switch (head.mode) {
//	case MRC_MODE_BYTE:
//		vbuf = (char*)&buf_byte;
//		break;
//
//	case MRC_MODE_SHORT:
//		vbuf = (char*)&buf_short;
//		break;
//
//	case MRC_MODE_FLOAT:
//		vbuf = (char*)&buf_float;
//		break;
//	}
//
//	switch (axis) {
//	case 'x':
//	case 'X':
//		in.seekg(sizeof(MrcHeader) + head.next + slcN * psize, std::ios::beg);
//
//		for (int i = 0; i < head.ny*head.nz; i++) {
//			in.read(vbuf, psize);
//			switch (head.mode) {
//			case MRC_MODE_BYTE:
//				slcdata[i] = *((unsigned char*)vbuf);
//				break;
//
//			case MRC_MODE_SHORT:
//				slcdata[i] = *((short*)vbuf);
//				break;
//
//			case MRC_MODE_FLOAT:
//				slcdata[i] = *((float*)vbuf);
//				break;
//			}
//			in.seekg((head.nx - 1)*psize, std::ios::cur);
//		}
//		break;
//
//	case 'y':
//	case 'Y':
//		for (int k = 0; k < head.nz; k++) {
//			in.seekg(sizeof(MrcHeader) + head.next + (k*head.nx*head.ny + head.nx*slcN)*psize, std::ios::beg);
//
//			for (int i = 0; i < head.nx; i++) {
//				in.read(vbuf, psize);
//				switch (head.mode) {
//				case MRC_MODE_BYTE:
//					slcdata[k*head.nx + i] = *((unsigned char*)vbuf);
//					break;
//
//				case MRC_MODE_SHORT:
//					slcdata[k*head.nx + i] = *((short*)vbuf);
//					break;
//
//				case MRC_MODE_FLOAT:
//					slcdata[k*head.nx + i] = *((float*)vbuf);
//					break;
//				}
//			}
//		}
//		break;
//
//	case 'z':
//	case 'Z':
//		in.seekg(sizeof(MrcHeader) + head.next + slcN * head.nx*head.ny*psize, std::ios::beg);
//
//		for (int i = 0; i < head.nx*head.ny; i++) {
//			in.read(vbuf, psize);
//			switch (head.mode) {
//			case MRC_MODE_BYTE:
//				slcdata[i] = *((unsigned char*)vbuf);
//				break;
//
//			case MRC_MODE_SHORT:
//				slcdata[i] = *((short*)vbuf);
//				break;
//
//			case MRC_MODE_FLOAT:
//				slcdata[i] = *((float*)vbuf);
//				break;
//			}
//		}
//		break;
//	}
//	return true;
//}
//
//bool MrcAppData(int modetype, int size, const void* data, const char* filename)
//{
//	int psize;
//
//	switch (modetype) {
//	case MRC_MODE_BYTE:
//		psize = sizeof(unsigned char);
//		break;
//	case MRC_MODE_SHORT:
//		psize = sizeof(short);
//		break;
//	case MRC_MODE_FLOAT:
//		psize = sizeof(float);
//		break;
//	}
//
//	std::ofstream out;
//	out.open(filename, std::ios::out | std::ios::app);
//	out.write((char*)data, size*psize);
//	out.close();
//}
//
//void MrcUpdateHead(MrcHeader* header, void* data)
//{
//	unsigned char* vchar;
//	short* vshort;
//	float* vfloat;
//
//	float amin = INT_MAX, amax = INT_MIN, amean = 0;
//
//	switch (header->mode) {
//	case MRC_MODE_BYTE:
//		vchar = (unsigned char*)data;
//		for (int i = 0; i < header->nx*header->ny*header->nz; i++) {
//			amean += vchar[i];
//			if (vchar[i] < amin) {
//				amin = vchar[i];
//			}
//			if (vchar[i] > amax) {
//				amax = vchar[i];
//			}
//		}
//		break;
//	case MRC_MODE_SHORT:
//		vshort = (short*)data;
//		for (int i = 0; i < header->nx*header->ny*header->nz; i++) {
//			amean += vshort[i];
//			if (vshort[i] < amin) {
//				amin = vshort[i];
//			}
//			if (vshort[i] > amax) {
//				amax = vshort[i];
//			}
//		}
//		break;
//	case MRC_MODE_FLOAT:
//		vfloat = (float*)data;
//		for (int i = 0; i < header->nx*header->ny*header->nz; i++) {
//			amean += vfloat[i];
//			if (vfloat[i] < amin) {
//				amin = vfloat[i];
//			}
//			if (vfloat[i] > amax) {
//				amax = vfloat[i];
//			}
//		}
//		break;
//	}
//	header->amin = amin;
//	header->amax = amax;
//	header->amean = amean / header->nx*header->ny*header->nz;
//}
//
//void MrcSave(const MrcHeader& header, const void* data, const char* filename)
//{
//	MrcWriteHead(header, filename);
//	MrcAppData(header.mode, header.nx*header.ny*header.nz, data, filename);
//}
//
//void MrcConvertTo1(MrcHeader* header, void* data)
//{
//	assert(header->amax - header->amin != 0);
//	for (int i = 0; i < header->nx*header->ny*header->nz; i++) {
//		switch (header->mode) {
//		case MRC_MODE_BYTE:
//			((unsigned char*)data)[i] = (((unsigned char*)data)[i] - header->amin) / (header->amax - header->amin);
//			break;
//		case MRC_MODE_SHORT:
//			((short*)data)[i] = (((short*)data)[i] - header->amin) / (header->amax - header->amin);
//			break;
//		case MRC_MODE_FLOAT:
//			((float*)data)[i] = (((float*)data)[i] - header->amin) / (header->amax - header->amin);
//			break;
//		}
//	}
//	header->amean = (header->amean - header->amin) / (header->amax - header->amin);
//	header->amin = 0;
//	header->amax = 1;
//}
//
//void MrcDataConTo1(const MrcHeader& header, void* data)
//{
//	assert(header.amax - header.amin != 0);
//	for (int i = 0; i < header.nx*header.ny*header.nz; i++) {
//		switch (header.mode) {
//		case MRC_MODE_BYTE:
//			((unsigned char*)data)[i] = (((unsigned char*)data)[i] - header.amin) / (header.amax - header.amin);
//			break;
//		case MRC_MODE_SHORT:
//			((short*)data)[i] = (((short*)data)[i] - header.amin) / (header.amax - header.amin);
//			break;
//		case MRC_MODE_FLOAT:
//			((float*)data)[i] = (((float*)data)[i] - header.amin) / (header.amax - header.amin);
//			break;
//		}
//	}
//}
//
//
//bool AutomMRCDataModel::Open(const char* filename) {
//	in.open(filename);
//	if (!in.good()) {
//		return false;
//	}
//
//	if (header) {
//		delete header;
//	}
//
//	header = new MRCheader();
//
//	in.read((char*)header, sizeof(MrcHeader));
//	//     if ( ! ( header->cmap[0]=='M'&&header->cmap[1]=='A'&&header->cmap[2]=='P' ) ) {
//	//         in.close();
//	//         delete header;
//	//         header = NULL;
//	//         return false;
//	//     }
//
//	std::string key, value;
//	inplane_rotation = -100;
//	//     printf("size of short %lu \n", sizeof(short));
//	// 
//	//     //liuce
//	//     printf("print labels(%d):\n",header->nlabl);
//	//     for(int i=0; i<10; i++) {
//	//         for(int j=0; j<80; j++) {
//	//             printf("%c",header->labels[i][j]);
//	//         }
//	//         printf("\n");
//	// 
//	//     }
//	//     //liuce
//	//     printf("print header :\n");
//	//     PrintHeader();
//	for (int i = 0; i < MRC_NLABELS; i++) {
//		header->labels[i][MRC_LABEL_SIZE] = 0;
//
//		if (i < header->nlabl) {
//			FixTitlePadding(header->labels[i]);
//			if (AnalysLabel(header->labels[i], key, value))
//			{
//				if (key == "Tilt axis rotation angle") {
//					std::istringstream ss(value);
//					float rot;
//					ss >> rot;
//					inplane_rotation = rot;
//				}
//			}
//		}
//	}
//
//	//If the tilt axis rotation angle is not in the "Tilt axis rotation angle" label, it should be in the extended header.
//	if (inplane_rotation == -100) {
//		if (header->next != 0) {
//			int nbh = header->next / 128; //128 is the length of the  FEI extended header
//			if (nbh == 1024) {
//				extendedHeader = new ExtendedHeader;
//				//FEI extended header
//				in.read((char*)extendedHeader, sizeof(ExtendedHeader));
//				//PrintExtendedHeader();
//				inplane_rotation = 1.0 * (extendedHeader->tilt_axis);
//
//			}
//		}
//	}
//
//
//
//	name = std::string(filename);
//	return true;
//}
//
//void AutomMRCDataModel::Close()
//{
//	if (header)
//	{
//		in.close();
//		out.close();
//		delete header;
//		header = NULL;
//	}
//}
//
//IplImage* AutomMRCDataModel::ReadIplImage(size_t index)
//{
//	assert(header != NULL && index >= 0 && index < header->nz);
//	assert(in.good());
//
//	const int& nx = header->nx;			//the coordinate is different(have not been tested)
//	const int& ny = header->ny;
//
//	QSize size(nx, ny);
//
//	//QImage img = QImage(size, IPL_DEPTH_32F, 1);
//	QImage img(size, QImage::Format_ARGB32_Premultiplied);
//
//	int bufsize = nx * ny;
//
//	switch (header->mode) {
//	case MRC_MODE_BYTE: {
//		unsigned char* tmpbuf = new unsigned char[bufsize];
//		for (size_t y = 0; y < ny; y++) {
//			in.seekg(sizeof(MrcHeader) + header->next + (index*nx*ny + y * nx) * sizeof(unsigned char), std::ios::beg);
//			in.read((char*) & (tmpbuf[y*nx]), nx * sizeof(unsigned char));
//		}
//
//		unsigned char* src = tmpbuf;
//		for (size_t y = 0; y < img->height; y++) {
//			float* start = (float*)(img->imageData + y * img->widthStep);
//			for (int x = 0; x < img->width; x++) {
//				*start++ = *src++ / 255.0f;
//			}
//		}
//
//		delete[] tmpbuf;
//		break;
//	}
//	case MRC_MODE_SHORT: {
//		short* tmpbuf = new short[bufsize];
//		for (size_t y = 0; y < ny; y++) {
//			in.seekg(sizeof(MrcHeader) + header->next + (index*nx*ny + y * nx) * sizeof(short), std::ios::beg);
//			in.read((char*) & (tmpbuf[y*nx]), nx * sizeof(short));
//		}
//
//		short* src = tmpbuf;
//		for (size_t y = 0; y < img->height; y++) {
//			float* start = (float*)(img->imageData + y * img->widthStep);
//			for (int x = 0; x < img->width; x++) {
//				*start++ = *src++;
//			}
//		}
//
//		delete[] tmpbuf;
//		break;
//	}
//	case MRC_MODE_FLOAT: {
//		for (size_t y = 0; y < ny; y++) {
//			in.seekg(sizeof(MrcHeader) + header->next + (index*nx*ny + y * nx) * sizeof(float), std::ios::beg);
//			in.read((char*)(img->imageData + y * img->widthStep), nx * sizeof(float));
//		}
//
//		break;
//	}
//	default:
//		cvReleaseImage(&img);
//		return NULL;
//	}
//
//	return img;
//}
//
//
//IplImage* AutomMRCDataModel::GetIplImage(int index) {
//	if (!is_cashed) {
//		return ReadIplImage(index);
//	}
//	else {
//		return cvCloneImage(slices[index]);
//	}
//}
//
//IplImage* const& AutomMRCDataModel::operator[](int idx)
//{
//	return slices[idx];
//}
//
//IplImage* const& AutomMRCDataModel::operator[](int idx) const
//{
//	return slices[idx];
//}
//
//void AutomMRCDataModel::CopyToNewStack(util::MrcStack& nmrcs) const
//{
//	nmrcs.FreeCache();
//
//	if (header) {
//		if (!nmrcs.header) {
//			nmrcs.header = new MrcHeader();
//		}
//
//		memcpy(nmrcs.header, header, sizeof(MrcHeader));
//	}
//
//	nmrcs.name = name;
//	nmrcs.in.open(nmrcs.name.c_str());
//	nmrcs.is_cashed = is_cashed;
//
//	if (is_cashed) {
//		nmrcs.slices = new IplImage*[header->nz];
//
//		for (int i = 0; i < header->nz; i++) {
//			nmrcs.slices[i] = cvCreateImage(cvGetSize(slices[i]), slices[i]->depth, slices[i]->nChannels);
//			cvCopy(nmrcs.slices[i], slices[i]);
//		}
//	}
//}
//
//const util::MrcStack& AutomMRCDataModel::operator=(const util::MrcStack& _mrcs)
//{
//	_mrcs.CopyToNewStack(*this);
//	return *this;
//}
//
//void AutomMRCDataModel::SetHeader(util::MrcStack::Mode mode, float amin, float amean, float amax)
//{
//	header->amin = amin;
//	header->amean = amean;
//	header->amax = amax;
//	header->mode = mode;
//}
//
//void AutomMRCDataModel::SetName(const char* __name)
//{
//	name = std::string(__name);
//}
//
//void AutomMRCDataModel::WriteHeaderToFile(const char* __name)
//{
//	if (__name) {
//		name = std::string(__name);
//	}
//
//	out.open(name.c_str(), std::ios::binary);
//	out.write((char*)header, sizeof(MRCheader));
//
//	char nullbuf[header->next];
//
//	out.write(nullbuf, header->next);
//}
//
//void AutomMRCDataModel::AppendIplImageToFile(const IplImage* img)
//{
//	for (size_t y = 0; y < header->ny; y++) {
//		out.write((char*)(img->imageData + y * img->widthStep), header->nx * sizeof(float));
//	}
//}
//
//static inline bool IsSpace(char c)
//{
//	if (' ' == c || '\t' == c)
//		return true;
//	return false;
//}
//
//static inline void Trim(std::string & str)
//{
//	if (str.empty()) {
//		return;
//	}
//	int i, start_pos, end_pos;
//	for (i = 0; i < str.size(); ++i) {
//		if (!IsSpace(str[i])) {
//			break;
//		}
//	}
//	if (i == str.size()) { // È«²¿ÊÇ¿Õ°××Ö·û´®
//		str = "";
//		return;
//	}
//
//	start_pos = i;
//
//	for (i = str.size() - 1; i >= 0; --i) {
//		if (!IsSpace(str[i])) {
//			break;
//		}
//	}
//	end_pos = i;
//
//	str = str.substr(start_pos, end_pos - start_pos + 1);
//}
//
//
//bool AutomMRCDataModel::AnalysLabel(const std::string& label, std::string& key, std::string& value)
//{
//	int pos;
//	if ((pos = label.find('=')) == -1)
//		return false;
//	key = label.substr(0, pos);
//	value = label.substr(pos + 1, label.size() - (pos + 1));
//	Trim(key);
//	if (key.empty()) {
//		return false;
//	}
//	Trim(value);
//	return true;
//}
//
//void AutomMRCDataModel::FixTitlePadding(char *label)
//{
//	int len;
//	label[MRC_LABEL_SIZE] = 0x00;
//	len = strlen(label);
//	if (len < MRC_LABEL_SIZE)
//		memset(&label[len], ' ', MRC_LABEL_SIZE - len);
//}
//
//void AutomMRCDataModel::PrintExtendedHeader(std::ostream& o) const {
//	o << "\n size of ExtendedHeader is :" << sizeof(ExtendedHeader);
//	o << "\n  a_tilt:  " << extendedHeader->a_tilt;   // size: 4 byte  Alpha tilt (deg)
//	o << "\n  b_tilt:  " << extendedHeader->b_tilt;   // size: 4 byte  Beta tilt (deg)
//	o << "\n  x_stage:  " << extendedHeader->x_stage;   // size: 4 byte  Stage x position (Unit=m. But if value>1, unit=???m)
//	o << "\n  y_stage:  " << extendedHeader->y_stage;   // size: 4 byte  Stage y position (Unit=m. But if value>1, unit=???m)
//	o << "\n  z_stage:  " << extendedHeader->z_stage;   // size: 4 byte  Stage z position (Unit=m. But if value>1, unit=???m)
//	o << "\n  x_shift:  " << extendedHeader->x_shift;   // size: 4 byte  Image shift x (Unit=m. But if value>1, unit=???m)
//	o << "\n  y_shift:  " << extendedHeader->y_shift;   // size: 4 byte  Image shift y (Unit=m. But if value>1, unit=???m)
//	o << "\n  defocus:  " << extendedHeader->defocus;   // size: 4 byte  Defocus Unit=m. But if value>1, unit=???m)
//	o << "\n  exp_time:  " << extendedHeader->exp_time;   // size: 4 byte Exposure time (s)
//	o << "\n  mean_int:  " << extendedHeader->mean_int;   // size: 4 byte Mean value of image
//	o << "\n  tilt_axis:  " << extendedHeader->tilt_axis;   // size: 4 byte   Tilt axis (deg)
//	o << "\n  pixel_size:  " << extendedHeader->pixel_size;   // size: 4 byte  Pixel size of image (m)
//	o << "\n  magnification:  " << extendedHeader->magnification;   // size: 4 byte   Magnification used
//
//}
//
//void AutomMRCDataModel::PrintHeader(std::ostream& o) const {
//	o << "\n nx: " << header->nx;         /*  # of Columns                  */
//	o << "\n ny: " << header->ny;         /*  # of Rows                     */
//	o << "\n nz: " << header->nz;         /*  # of Sections.                */
//	o << "\n mode: " << header->mode;       /*  given by #define MRC_MODE...  */
//
//	o << "\n nxstart: " << header->nxstart;    /*  Starting point of sub image.  */
//	o << "\n nystart: " << header->nystart;
//	o << "\n nzstart: " << header->nzstart;
//
//	o << "\n mx: " << header->mx;         /* Grid size in x, y, and z       */
//	o << "\n my: " << header->my;
//	o << "\n mz: " << header->mz;
//
//	o << "\n xlen: " << header->xlen;       /* length of x element in um.     */
//	o << "\n ylen: " << header->ylen;       /* get scale = xlen/nx ...        */
//	o << "\n zlen: " << header->zlen;
//
//	o << "\n alpha: " << header->alpha;      /* cell angles, ignore */
//	o << "\n beta: " << header->beta;
//	o << "\n gamma: " << header->gamma;
//
//	o << "\n mapc: " << header->mapc;       /* map coloumn 1=x,2=y,3=z.       */
//	o << "\n mapr: " << header->mapr;       /* map row     1=x,2=y,3=z.       */
//	o << "\n maps: " << header->maps;       /* map section 1=x,2=y,3=z.       */
//
//	o << "\n amin: " << header->amin;
//	o << "\n amax: " << header->amax;
//	o << "\n amean: " << header->amean;
//
//	o << "\n ispg: " << header->ispg;       /* image type */
//	o << "\n nsymbt: " << header->nsymbt;     /* space group number */
//
//
//	/* 64 bytes */
//
//	o << "\n next: " << header->next;
//	o << "\n sizeof header: " << sizeof(MRCheader);
//	o << "\n creatid: " << header->creatid;  /* Creator id, hvem = 1000, DeltaVision = -16224 */
//
//
//	o << "\n blank: " << header->blank;
//
//	o << "\n nint: " << header->nint;
//	o << "\n nreal: " << header->nreal;
//	o << "\n sub: " << header->sub;
//	o << "\n zfac: " << header->zfac;
//
//	o << "\n min2: " << header->min2;
//	o << "\n max2: " << header->max2;
//	o << "\n min3: " << header->min3;
//	o << "\n max3: " << header->max3;
//	o << "\n min4: " << header->min4;
//	o << "\n max4: " << header->max4;
//
//
//	o << "\n idtype: " << header->idtype;
//	o << "\n lens: " << header->lens;
//	o << "\n nd1: " << header->nd1;     /* Devide by 100 to get o<<header-> value. */
//	o << "\n nd2: " << header->nd2;
//	o << "\n vd1: " << header->vd1;
//	o << "\n vd2: " << header->vd2;
//	o << "\n tiltangles: " << header->tiltangles[0] << " " << header->tiltangles[1] << " "
//		<< header->tiltangles[2] << " " << header->tiltangles[3] << " " << header->tiltangles[4] << " "
//		<< header->tiltangles[5] << " ";  /* 0,1,2 = original:  3,4,5 = current */
//
//
//	o << "\n xorg: " << header->xorg;
//	o << "\n yorg: " << header->yorg;
//	o << "\n zorg: " << header->zorg;
//	o << "\n cmap: " << header->cmap[0] << header->cmap[1] << header->cmap[2] << header->cmap[3];
//	o << "\n stamp: " << header->stamp[0] << header->stamp[1] << header->stamp[2] << header->stamp[3];
//	o << "\n rms: " << header->rms;
//
//	for (int i = 0; i < header->nlabl; i++) {
//		o << "\n labels[" << i << "]: " << header->labels[i];
//	}
//	o.flush();
//
//}
//
//
//
//AutomMRCDataModel::AutomMRCDataModel(const QString& fileName):AbstractSliceDataModel()
//{
//
//}
//
//int AutomMRCDataModel::topSliceCount() const
//{
//
//}
//
//int AutomMRCDataModel::rightSliceCount() const
//{
//}
//
//int AutomMRCDataModel::frontSliceCount() const
//{
//}
//
//QImage AutomMRCDataModel::originalTopSlice(int index) const
//{
//}
//
//QImage AutomMRCDataModel::originalRightSlice(int index) const
//{
//}
//
//QImage AutomMRCDataModel::originalFrontSlice(int index) const
//{
//
//}
//
//float AutomMRCDataModel::minValue() const
//{
//
//}
//
//float AutomMRCDataModel::maxValue() const
//{
//
//}
//
//int AutomMRCDataModel::dataType()
//{
//
//}
//
//void* AutomMRCDataModel::rawData()
//{
//
//}
//
//const void* AutomMRCDataModel::constRawData()
//{
//
//}
