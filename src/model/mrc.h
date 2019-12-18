#ifndef MRC_H
#define MRC_H
#include <cstdio>
#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <atomic>
#include <qlist.h>
#include <tuple>

/*
* http://www.sciencedirect.com/science/article/pii/S104784771500074X
* The pdf above shows the details of the MRC 2014 format, and
* explains what every field actually is in the MRC Header.It also
* talks about the concrete implements of MRC format in different popular softwares
*
*/

/*
*			NOTE:
*			For the sake of simplicity,so far,
*			this MRC class only support :
*			float data type reading,char data type reading
*			and the creating of char data type with single image,
*			image stack,single volume and volume stack dimension type.
*			For creating MRC files, only few of fields in the header are
*			considered.
*/

/*CONSIDERED FIELD WHEN READING A MRC FILE:
* nx
* ny
* nz
* mode
*
*/

/*
* CONSIDERED FIELD WHEN CREATING A MRC FILE:
* nx: [0,+)
* ny: [0,+)
* nz: [0,+]
* mode:0 for 8bit signed integer
* mx: equal to nx
* my: equal to ny
* mz:
* mapc,mapr,mapc are 1,2,3 respectively
* dmin
* dmax
* dmean
* ispg
* nversion: year*10+version
*/



/* END_CODE */

#define MRC_IDTYPE_MONO   0
#define MRC_IDTYPE_TILT   1
#define MRC_IDTYPE_TILTS  2
#define MRC_IDTYPE_LINA   3
#define MRC_IDTYPE_LINS   4

#define MRC_SCALE_LINEAR  1
#define MRC_SCALE_POWER   2
#define MRC_SCALE_LOG     3
#define MRC_SCALE_BKG     4

/* DOC_CODE MRC data modes */
/* The modes defined for MRC files in IMOD */
#define MRC_MODE_BYTE          0
#define MRC_MODE_SHORT         1
#define MRC_MODE_FLOAT         2
#define MRC_MODE_COMPLEX_SHORT 3
#define MRC_MODE_COMPLEX_FLOAT 4
#define MRC_MODE_USHORT        6
#define MRC_MODE_RGB           16
#define MRC_MODE_4BIT          101
/* END_CODE */

#define MRC_EXT_TYPE_NONE      0
#define MRC_EXT_TYPE_SERI      1
#define MRC_EXT_TYPE_AGAR      2
#define MRC_EXT_TYPE_FEI       3
#define MRC_EXT_TYPE_UNKNOWN   4

#define PACKED_4BIT_MODE       1
#define PACKED_HALF_XSIZE      2

#define MRC_RAMP_LIN 1
#define MRC_RAMP_EXP 2
#define MRC_RAMP_LOG 3


#define MRC_LABEL_SIZE         80
#define MRC_NEXTRA             16
#define MRC_NLABELS            10
#define MRC_HEADER_SIZE        1024   /* Length of Header is 1024 Bytes. */
#define MRC_MAXCSIZE           3

#define IIUNIT_SWAPPED       (1l << 0)
#define IIUNIT_BYTES_SIGNED  (1l << 1)
#define IIUNIT_OLD_STYLE     (1l << 2)
#define IIUNIT_NINT_BUG      (1l << 3)
#define IIUNIT_BAD_MAPCRS    (1l << 4)
#define IIUNIT_4BIT_MODE     (1l << 5)
#define IIUNIT_HALF_XSIZE    (1l << 6)
#define IIUNIT_Y_INVERTED    (1l << 7)


/*  Data Field Offset in MRC Header according to MRC2014
	For more infomation, Please read
	http://www.ccpem.ac.uk/mrc_format/mrc_format.php
*/
#define NX_OFFSET               0
#define NY_OFFSET               4
#define NZ_OFFSET               8
#define MODE_OFFSET             12
#define NXSTART_OFFSET          16
#define NYSTART_OFFSET          20
#define NZSTART_OFFSET          24
#define MX_OFFSET               28
#define MY_OFFSET               32
#define MZ_OFFSET               36
/*The follwing three are CELLA in the MRC header description*/
#define XLEN_OFFSET             40
#define YLEN_OFFSET             44
#define ZLEN_OFFSET             48
/*The follwing three are CELLB in the MRC header description*/
#define ALPHA_OFFSET            52
#define BETA_OFFSET             56
#define GAMMA_OFFSET            60

#define MAPC_OFFSET             64
#define MAPR_OFFSET             68
#define MAPS_OFFSET             72
#define DMIN_OFFSET             76
#define DMAX_OFFSET             80
#define DMEAN_OFFSET            84
#define ISPG_OFFSET             88
#define NSYMBT_OFFSET           92
#define FIRST_BLANK_OFFSET      96
#define EXTTYP_OFFSET           104
#define NVERSION_OFFSET         108
//#define SECOND_BLANK_OFFSET     112
#define XORIGIN_OFFSET          196
#define YORIGIN_OFFSET          200
#define ZORIGIN_OFFSET          204
#define MAP_OFFSET              208
#define STAMP_OFFSET            212
#define RMS_OFFSET              216
#define NLABL_OFFSET            220
#define LABEL_OFFSET            224


#define MRC_VERSION_FIELD(year,verion) ((year*10)+version)

/**
* @brief \n
*
*/


inline
uint16_t reverseByte(uint16_t value)
{
	const auto v = ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
	return v;
}
inline
uint32_t reverseByte(uint32_t value)
{
	const auto v = ((value & 0xffU) << 24) | ((value & 0xff00U) << 8) | ((value & 0xff0000U) >> 8) | ((value & 0xff000000U) >> 24);
	return v;
}

union FloatConv
{
	float f;
	char byte[4];
};
inline float reverseEndian(float value)
{
	FloatConv c1,c2;
	c1.f = value;
	c2.byte[0] = c1.byte[3];
	c2.byte[1] = c1.byte[2];
	c2.byte[2] = c1.byte[1];
	c2.byte[3] = c1.byte[0];
	return c2.f;
}

class MRC
{

public:
	using MRCInt32 = int;
	using MRCFloat = float;
	using MRCInt16 = short;
	using MRCUInt32 = unsigned int;
	using MRCUint16 = unsigned short;
	using MRCDouble = double;
	using MRCInt8 = char;
	using MRCUInt8 = unsigned char;
	using MRCSizeType = size_t;

	typedef struct  /*complex floating number*/
	{
		MRCFloat a;
		MRCFloat b;

	} MRCComplexFloat;

	typedef struct  /*complex short number*/
	{
		MRCInt16 a;
		MRCInt16 b;

	} MRCComplexShort;


	enum class Format { MRC, RAW };
	enum class ImageDimensionType { SingleImage, ImageStack };
	enum class VolumeDimensionType { SingleVolume, VolumeStack };
	enum class DataType { Integer8, Integer16, Integer32, Real32, Complex16, Complex32 };

private:
	struct MRCHeader
	{
		MRCInt32   nx;         /*  # of Columns                  */
		MRCInt32   ny;         /*  # of Rows                     */
		MRCInt32   nz;         /*  # of Sections.                */
		MRCInt32   mode;       /*  given by #define MRC_MODE...  */
							   /*0 8-bit signed integer (range -128 to 127)
							   1 16-bit signed integer
							   2 32-bit signed real
							   3 transform : complex 16-bit integers
							   4 transform : complex 32-bit reals
							   6 16-bit unsigned integer
							   */

		MRCInt32   nxstart;    /*  Starting point of sub image.  UNSUPPORTED */
		MRCInt32   nystart;
		MRCInt32   nzstart;

		MRCInt32   mx;         /* "Grid size", # of pixels in "unit cell"    */
		MRCInt32   my;         /* Keep the same as nx, ny, nz                */
		MRCInt32   mz;

		MRCFloat   xlen;       /* length of unit cell in Angstroms           */
		MRCFloat   ylen;       /* get scale = xlen/nx ...                    */
		MRCFloat   zlen;

		MRCFloat   alpha;      /* cell angles, ignored, set to 90            */
		MRCFloat   beta;
		MRCFloat   gamma;

		MRCInt32   mapc;       /* map column  1=x,2=y,3=z.     UNSUPPORTED  */
		MRCInt32   mapr;       /* map row     1=x,2=y,3=z.                  */
		MRCInt32   maps;       /* map section 1=x,2=y,3=z.                  */

		MRCFloat   dmin;      //Minimum Pixel Value
		MRCFloat   dmax;      //Maximum Pixel Value
		MRCFloat   dmean;     //Mean of Pixel Value

							  /* 1/12/12: Removed nsymbt and made ispg be 4 bytes to match standard */
		MRCInt32   ispg;       /* space group number in the standard */ /* 64 bytes */

		MRCInt32   nsymbt;     /* This is nsymbt in the MRC standard. */
							   /*Number of bytes in extended header*/

							   /*Following definition is not a part of standard.
							   * */
		MRCInt16   creatid;  /* Used to be creator id, hvem = 1000, now 0 */

		MRCInt8    blank[6];      //Blank data. First two bytes should be 0.

								  /**/
		MRCInt8    extType[4]; /* Extended type. */
		MRCInt32   nversion;  /* Version number in MRC 2014 standard */
							  /**/

		MRCInt8    blank2[16]; /*Blank*/
		MRCInt16   nint;
		MRCInt16   nreal;

		/*20 bytes blank*/
		MRCInt16   sub;
		MRCInt16   zfac;
		MRCFloat   min2;
		MRCFloat   max2;
		MRCFloat   min3;
		MRCFloat   max3;

		MRCInt32   imodStamp;
		MRCInt32   imodFlags;
		/*Bit flags:
		1 = bytes are stored as signed
		2 = pixel spacing was set from size in extended header
		4 = origin is stored with sign inverted from definition below
		8 = RMS value is negative if it was not computed
		16 = Bytes have two 4-bit values, the first one in the
		low 4 bits and the second one in the high 4 bits*/

		/*  UINT   extra[MRC_NEXTRA];*/

		/* HVEM extra data */
		/* DNM 3/16/01: divide idtype into two shorts */

		MRCInt16   idtype;
		MRCInt16   lens;
		MRCInt16   nd1;     /* Devide by 100 to get float value. */
		MRCInt16   nd2;
		MRCInt16   vd1;
		MRCInt16   vd2;
		MRCFloat   tiltangles[6];  /* 0,1,2 = original:  3,4,5 = current */

#ifdef OLD_STYLE_HEADER
								   /* before 2.6.20 */
								   /* DNM 3/16/01: redefine the last three floats as wavelength numbers */
		MRCInt16   nwave;   /* # of wavelengths and values */
		MRCInt16   wave1;
		MRCInt16   wave2;
		MRCInt16   wave3;
		MRCInt16   wave4;
		MRCInt16   wave5;
		MRCFloat   zorg;           /* origin */

		MRCFloat   xorg;
		MRCFloat   yorg;
#else
								   /* MRC 2000 standard */
		MRCFloat   xorg;
		MRCFloat   yorg;
		MRCFloat   zorg;
		MRCInt8    cmap[4];
		MRCInt8    stamp[4];
		MRCFloat   rms;
#endif

		MRCInt32 nlabl;
		MRCInt8  labels[MRC_NLABELS][MRC_LABEL_SIZE + 1];

		/* Internal data not stored in file header */
		//      b3dUByte *symops;
		//      FILE   *fp;
		//      int    pos;
		//      struct LoadInfo *li;
		//      int    headerSize;
		//      int    sectionSkip;
		//      int    swapped;
		//      int    bytesSigned;
		//      int    yInverted;
		//      int    iiuFlags;
		//      int    packed4bits;

		//      char *pathname;
		//      char *filedesc;
		//      char *userData;
	};

	/*The MRC Header is MRC2014 Version. Reading
	http://www.ccpem.ac.uk/mrc_format/mrc2014.php for more details*/




	//Type Definition


	//static constexpr  std::tuple<std::string, DataType,int> tables[] = {{"asdf",DataType::Integer32,32},{}};



	struct MRCDataPrivate
	{
		std::atomic<int> ref;
		void * data;
		bool own;
		MRCDataPrivate() = default;
		~MRCDataPrivate()
		{
			if (data && own)
				free(data);
			data = nullptr;
		}
		MRCDataPrivate & operator=(const MRCDataPrivate & other) = delete;
		MRCDataPrivate & operator=(MRCDataPrivate && other) = delete;

		static MRCDataPrivate * create(void * data)
		{
			MRCDataPrivate * d = nullptr;
			d = new MRCDataPrivate;
			d->own = false;			//shared data;
			d->ref = 1;
			d->data = data;
			return d;
		}

		static MRCDataPrivate * create(size_t width, size_t height, size_t slice, size_t elemSize)
		{
			MRCDataPrivate *d = nullptr;
			d = new MRCDataPrivate;
			d->own = true;			//own the data
			d->ref = 1;
			d->data = malloc(width*height*slice*elemSize);
			if (d->data == nullptr) {
				delete d;
				d = nullptr;
				return nullptr;
			}
			return d;
		}

	};
public:
	/**
	* @brief Default constructor
	*/
	MRC();
	/**
	* @brief Constructor receiving a path string
	* @param fileName	mrc file path
	*/
	explicit MRC(const std::string & fileName);
	//image and image stack

	MRC(void * data,
		int width,
		int height,
		int slice, ImageDimensionType DimensionType, DataType dataType = MRC::DataType::Integer8);
	//volume and volume stack
	MRC(void * data,
		int width,
		int height,
		int volumeZ,
		int volumeCount,
		VolumeDimensionType DimensionType,
		DataType dataType = MRC::DataType::Integer8);
	MRC(const MRC & otherMRC, void * data);
	MRC(const MRC & rhs);
	MRC(MRC && rhs)noexcept;
	MRC& operator=(const MRC & rhs);
	MRC& operator=(MRC && rhs)noexcept;
public:
	static MRC fromData(void * data, int width, int height, int slice, DataType type = MRC::DataType::Integer8);
	static MRC fromMRC(const MRC & otherMRC, unsigned char *data);
	bool open(const std::string & fileName);
	bool save(const std::string & fileName, MRC::Format format = Format::MRC);
	std::string fileName()const { return std::string(); }
	bool isOpened()const;
	int width()const;           //first dimension
	int height()const;          //second dimension
	int slice()const;          //third dimension  z-axis
	int propertyCount()const;
	std::string propertyName(int index)const;
	DataType propertyType(int index)const;
	template<typename T> T property(int index)const;
	template<typename T> T * data()const;
	float minValue()const;
	float maxValue()const;
	//template<typename T> auto dataWithType()const;
	DataType dataType()const;
	std::string info()const;
	virtual ~MRC()noexcept;
private:
	MRCHeader m_header;
	unsigned char hdBuffer[MRC_HEADER_SIZE];
	MRCDataPrivate* m_d;
	bool m_opened;
private:

	MRC(const std::string & fileName, bool opened) : m_d{ nullptr }, m_opened{ opened } { (void)fileName; }

	void reverseHeaderByteByField(MRCHeader* header);
	void udpateNVersionFiled(int year, int version = 0);
	void UpdateDminDmaxDmeanRMSHelper();
	bool headerReadHelper(std::ifstream & in, MRCHeader * header);
	bool headerWriteHelper(std::ofstream & out, MRCHeader * header);
	void createMRCHeader();
	void updateMRCHeader();
	static inline void copyHeaderBuffer(unsigned char* dst, const unsigned char* src, int size);
	size_t typeSize(MRC::DataType type) const;
	std::string propertyInfoString(const MRCHeader *header)const;
	bool readDataFromFileHelper(std::ifstream& in);
	inline void detach();
};

template<typename T>
inline T MRC::property(int index)const
{
	static const int offset[] = {
		NX_OFFSET,
		NY_OFFSET,
		NZ_OFFSET,
		MODE_OFFSET,
		NXSTART_OFFSET,
		NYSTART_OFFSET,
		NZSTART_OFFSET,
		MX_OFFSET,
		MY_OFFSET,
		MZ_OFFSET,
		XLEN_OFFSET,
		YLEN_OFFSET,
		ZLEN_OFFSET,
		ALPHA_OFFSET,
		BETA_OFFSET,
		GAMMA_OFFSET,
		MAPC_OFFSET,
		MAPR_OFFSET,
		MAPS_OFFSET,
		DMIN_OFFSET,
		DMAX_OFFSET,
		DMEAN_OFFSET,
		ISPG_OFFSET,
		NSYMBT_OFFSET,
		EXTTYP_OFFSET,
		EXTTYP_OFFSET + 1,
		EXTTYP_OFFSET + 2,
		EXTTYP_OFFSET + 3,
		NVERSION_OFFSET,
		XORIGIN_OFFSET,
		YORIGIN_OFFSET,
		ZORIGIN_OFFSET,
		MAP_OFFSET,
		STAMP_OFFSET ,
		RMS_OFFSET ,
		NLABL_OFFSET ,
	};
	T * d = ((T*)(hdBuffer + offset[index]));
	return *d;
}

template <typename T>
T* MRC::data() const
{
	if (std::is_same<T, void>::value)
	{
		return static_cast<T*>(m_d->data);
	}
	bool canConvert;
	switch (dataType())
	{
	case DataType::Integer8:
		canConvert = std::is_same<T, MRCInt8>::value || std::is_same<T, MRCUInt8>::value;
		break;
	case DataType::Integer16:
		canConvert = std::is_same<T, MRCInt16>::value;
		break;
	case DataType::Integer32:
		canConvert = std::is_same<T, MRCInt32>::value;
		break;
	case DataType::Real32:
		canConvert = std::is_same<T, MRCFloat>::value;
		break;
	case DataType::Complex16:
		canConvert = std::is_same<T, MRCComplexShort>::value;
		break;
	case DataType::Complex32:
		canConvert = std::is_same<T, MRCComplexFloat>::value;
		break;
	default:
		canConvert = false;
		break;
	}
	if (canConvert)
		return static_cast<T*>(m_d->data);
	else
		return nullptr;
}
inline void MRC::detach() { if (m_d != nullptr && --m_d->ref == 0)delete m_d; }
inline void MRC::copyHeaderBuffer(unsigned char* dst, const unsigned char* src, int size) { memcpy(dst, src, size); }
inline bool MRC::isOpened() const { return m_opened; }
inline int MRC::width() const { return m_header.nx; }
inline int MRC::height() const { return m_header.ny; }
inline int MRC::slice() const { return m_header.nz; }
inline int MRC::propertyCount() const { return 36; }
#endif // MRC_H