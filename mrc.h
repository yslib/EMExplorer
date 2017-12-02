#ifndef MRC_H
#define MRC_H
#include <cstdio>
#include <QString>
#include <QImage>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <QDebug>


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
#define XLEN_OFFSET             40
#define YLEN_OFFSET             44
#define ZLEN_OFFSET             48
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



class MRC
{
public:     //Type Definition

public:
    explicit MRC();
    explicit MRC(const QString & fileName);

    MRC(const MRC & rhs);
    MRC(MRC && rhs);
    MRC& operator=(const MRC & rhs);
    MRC& operator=(MRC && rhs);
public:

    bool open(const QString & fileName);
    bool save(const QString & fileName);
    bool isOpened()const;

    int getWidth()const;           //first dimension
    int getHeight()const;          //second dimension
    int getSliceCount()const;          //third dimension
    const unsigned char * data()const;
    unsigned char * data();


    QString getMRCInfo()const;
    QImage getSlice(int slice)const;
    QVector<QImage> getSlices()const;
    bool setSlice(const QImage & image,int slice);

    virtual ~MRC();
private:
    using MRCInt32 = int;
    using MRCFloat = float;
    using MRCInt16 = short;
    using MRCUInt32 = unsigned int;
    using MRCUint16 = unsigned short;
    using MRCDouble = double;
    using MRCByte = char;
    using MRCUByte = unsigned char;
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


    /*The MRC Header is MRC2014 Version. Reading
    http://www.ccpem.ac.uk/mrc_format/mrc2014.php for more details*/

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

      MRCByte    blank[6];      //Blank data. First two bytes should be 0.

      /**/
      MRCByte    extType[4]; /* Extended type. */
      MRCInt32   nversion;  /* Version number in MRC 2014 standard */
      /**/

      MRCByte    blank2[16]; /*Blank*/
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
      MRCByte    cmap[4];
      MRCByte    stamp[4];
      MRCFloat   rms;
    #endif

      MRCInt32 nlabl;
      MRCByte  labels[MRC_NLABELS][MRC_LABEL_SIZE + 1];

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

private:            //variance
    QString m_fileName;

    MRCHeader m_header;

    unsigned char *m_mrcData;

    size_t m_mrcDataSize;

    QVector<QImage> m_slices;




    //QVector<QPixmap> m_slicesPixmap;



    bool m_opened;
private:
    MRC(const QString & fileName,bool opened):m_fileName(fileName),m_mrcData{nullptr},m_opened{opened},m_mrcDataSize{0}{}


    bool _mrcHeaderRead(FILE *fp,MRCHeader * header);
    const unsigned char * _getData(size_t start)const;
    int _setData(size_t start,unsigned char * data,size_t size);
    std::string _getMRCHeaderInfo(const MRCHeader *header)const;
    bool _readDataFromFile(FILE * fp);

    void _reset();
    bool _init();
    bool _allocate();
    void _destroy();
};

#endif // MRC_H
