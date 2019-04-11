//#ifndef AUTOMMRCDATAMODEL_H
//#define AUTOMMRCDATAMODEL_H
//#include "abstract/abstractslicedatamodel.h"
//
//#include <iostream>
//
//
//#define MRC_MODE_BYTE            0
//#define MRC_MODE_SHORT           1
//#define MRC_MODE_FLOAT           2
//#define MRC_MODE_COMPLEX_SHORT   3
//#define MRC_MODE_COMPLEX_FLOAT   4
//#define MRC_MODE_USHORT          6
//#define MRC_MODE_RGB             16
//
//#define MRC_CFLOAT_REAL_IMAG     20            /* COMPLEX FLOAT mode */
//#define MRC_CFLOAT_AMP_RAD       21           /* COMPLEX FLOAT mode, but in amplitude and phase(  ) form */
//#define MRC_CFLOAT_AMP_DEG       22           /* COMPLEX FLOAT mode, but in amplitude and phase( degree ) form */
//
//
//#define MRC_LABEL_SIZE         79
//#define MRC_NEXTRA             16
//#define MRC_NLABELS            10
//#define MRC_HEADER_SIZE        1024   /* Length of Header is 1024 Bytes. */
//#define MRC_MAXCSIZE           3
//
///* The header structure for MRC files */
//#pragma pack(1)
//typedef struct MRCheader
//{
//	int   nx;         /*  # of Columns                  */
//	int   ny;         /*  # of Rows                     */
//	int   nz;         /*  # of Sections.                */
//	int   mode;       /*  given by #define MRC_MODE...  */
//
//	int   nxstart;    /*  Starting point of sub image.  */
//	int   nystart;
//	int   nzstart;
//
//	int   mx;         /* Grid size in x, y, and z       */
//	int   my;
//	int   mz;
//
//	float   xlen;       /* length of x element in um.     */
//	float   ylen;       /* get scale = xlen/nx ...        */
//	float   zlen;
//
//	float   alpha;      /* cell angles, ignore */
//	float   beta;
//	float   gamma;
//
//	int   mapc;       /* map coloumn 1=x,2=y,3=z.       */
//	int   mapr;       /* map row     1=x,2=y,3=z.       */
//	int   maps;       /* map section 1=x,2=y,3=z.       */
//
//	float   amin;
//	float   amax;
//	float   amean;
//
//	short   ispg;       /* image type */
//	short   nsymbt;     /* space group number */
//
//
//	/* 64 bytes */
//
//	int   next;
//	short   creatid;  /* Creator id, hvem = 1000, DeltaVision = -16224 */
//
//
//	char    blank[30];
//
//	short   nint;
//	short   nreal;
//	short   sub;
//	short   zfac;
//
//	float   min2;
//	float   max2;
//	float   min3;
//	float   max3;
//	float   min4;
//	float   max4;
//
//
//	short   idtype;
//	short   lens;
//	short   nd1;     /* Devide by 100 to get float value. */
//	short   nd2;
//	short   vd1;
//	short   vd2;
//	float   tiltangles[6];  /* 0,1,2 = original:  3,4,5 = current */
//
//
//	float   xorg;
//	float   yorg;
//	float   zorg;
//	char    cmap[4];
//	char    stamp[4];
//	float   rms;
//
//	int nlabl;
//	char  labels[MRC_NLABELS][MRC_LABEL_SIZE + 1];
//
//} MrcHeader;
//
///*
//Extended Header (FEI format and IMOD format)
//The extended header contains the information about a maximum of 1024 images.
//Each section is 128 bytes long. The extended header is thus 1024 * 128 bytes
//(always the same length, regardless of how many images are present
//
//*/
//struct  ExtendedHeader {
//	float   a_tilt;   // size: 4 byte  Alpha tilt (deg)
//	float   b_tilt;   // size: 4 byte  Beta tilt (deg)
//	float   x_stage;   // size: 4 byte  Stage x position (Unit=m. But if value>1, unit=???m)
//	float   y_stage;   // size: 4 byte  Stage y position (Unit=m. But if value>1, unit=???m)
//	float   z_stage;   // size: 4 byte  Stage z position (Unit=m. But if value>1, unit=???m)
//	float   x_shift;   // size: 4 byte  Image shift x (Unit=m. But if value>1, unit=???m)
//	float   y_shift;   // size: 4 byte  Image shift y (Unit=m. But if value>1, unit=???m)
//	float   defocus;   // size: 4 byte  Defocus Unit=m. But if value>1, unit=???m)
//	float   exp_time;   // size: 4 byte Exposure time (s)
//	float   mean_int;   // size: 4 byte Mean value of image
//	float   tilt_axis;   // size: 4 byte   Tilt axis (deg)
//	float   pixel_size;   // size: 4 byte  Pixel size of image (m)
//	float   magnification;   // size: 4 byte   Magnification used
//};
//#pragma pack()
///* END OF HEAD CODE */
//
//bool MrcReadHead(MrcHeader* header, const char* filename);
//bool MrcWriteHead(const MrcHeader& header, const char* filename);
//void MrcInitHead(MrcHeader *head);
//
//char* 		convertTostring(char lb[10][80]);
//
///**slcN couts from 0 to N-1, so if you want to read the first slice slcN shoud be 0*/
//bool MrcReadSlice(const char* filename, int slcN, char axis, float *slcdata);
//bool MrcAppData(int modetype, int size, const void* data, const char* filename);
//void MrcUpdateHead(MrcHeader* header, void* data);
//void MrcSave(const MrcHeader& header, const void* data, const char* filename);
//void MrcConvertTo1(MrcHeader* header, void* data);
//void MrcDataConTo1(const MrcHeader& header, void* data);
//
//
//
//
//
//class AutomMRCDataModel :AbstractSliceDataModel
//{
//	Q_OBJECT
//	QString fileName;
//	MRCheader* header;
//	ExtendedHeader* extendedHeader;
//	std::string name;
//	std::ifstream in;
//	std::ofstream out;
//	bool is_cashed;
//
//
//	void FixTitlePadding(char *label);
//
//	bool AnalysLabel(const std::string& label, std::string& key, std::string&  value);
//
//
//public:
//	enum Mode { MODE_BYTE = 0, MODE_SHORT = 1, MODE_FLOAT = 2 };
//
//public:
//
//	float inplane_rotation;//////////
//public:
//	AutomMRCDataModel(const QString & fileName);
//	int topSliceCount() const override;
//	int rightSliceCount() const override;
//	int frontSliceCount() const override;
//
//	QImage originalTopSlice(int index) const override;
//	QImage originalRightSlice(int index) const override;
//	QImage originalFrontSlice(int index) const override;
//
//	float minValue()const override;
//	float maxValue()const override;
//
//	int dataType() override;
//	void * rawData() override;
//	const void * constRawData()override;
//
//
//
//	~AutomMRCDataModel() 
//	{
//		if (header) {
//			delete header;
//		}
//
//		if (extendedHeader) {
//			delete extendedHeader;
//		}
//	}
//
//	bool Open(const char* filename);
//
//	void Close();
//
//	//IplImage* GetIplImage(int index);
//
//	//void DoCaching();
//
//	//void FreeCache();
//
//	//IplImage* const& operator [](int idx);
//
//	//IplImage* const& operator [] (int idx) const;
//
//	int Size() const { return header->nz; }
//
//	int Width() const { return header->nx; }
//
//	int Height() const { return header->ny; }
//
//	const MRCheader& Header() const { return *header; }
//
//	const char* Name() const { return name.c_str(); }
//
//	void PrintHeader(std::ostream& o = std::cout) const;
//
//	void PrintExtendedHeader(std::ostream& o = std::cout) const;
//
//	//void CopyToNewStack(MrcStack& nmrcs) const;
//
//	//const MrcStack& operator = (const MrcStack& _mrcs);
//
//	void SetHeader(Mode mode, float amin, float amean, float amax);
//
//	void SetName(const char* __name);
//
//	void WriteHeaderToFile(const char* __name = NULL);
//
//	//void AppendIplImageToFile(const IplImage* img);
//
//
//
//};
//
//#endif // AUTOMMRCDATAMODEL_H