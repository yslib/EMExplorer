#include "mrc.h"

//int mrc_head_read(FILE *fin, MrcHeader *hdata)
//{
//  int i;
//  int retval = 0;
//  int datasize;
//  ImodImageFile *iiFile;

//  if (!fin)
//    return(-1);

//  iiFile = iiLookupFileFromFP(fin);
//  if (iiFile) {
//    return iiFillMrcHeader(iiFile, hdata);
//  }

//  b3dRewind(fin);

//  i = fread(hdata, 4, 56, fin);
//  if (i != 56) {
//    b3dError(stderr, "ERROR: mrc_head_read - reading header data; %d of 56 words read, "
//             "system error: %s\n", i, strerror(errno));
//    return(-1);
//  }


//  hdata->swapped = 0;
//  hdata->iiuFlags = 0;

//  /* Test for byte-swapped data with image size and the map numbers and
//     mark data as swapped if it fails */
//  if (mrc_test_size(hdata))
//    hdata->swapped = 1;

//  /* DNM 7/30/02: test for old style header and rearrange origin info */
//  if (hdata->cmap[0] != 'M' || hdata->cmap[1] != 'A' ||
//      hdata->cmap[2] != 'P') {
//    memcpy(&hdata->zorg, &hdata->cmap[0], 4);
//    memcpy(&hdata->xorg, &hdata->stamp[0], 4);
//    memcpy(&hdata->yorg, &hdata->rms, 4);
//    hdata->rms = -1.;
//    if (hdata->swapped)
//      mrc_swap_floats(&hdata->rms, 1);
//    mrc_set_cmap_stamp(hdata);
//    hdata->iiuFlags |= IIUNIT_OLD_STYLE;
//  }

//  if (hdata->swapped) {
//    mrc_swap_header(hdata);

//    /* Test that this swapping makes values acceptable */
//    /* Let calling program issue error message */
//    if (mrc_test_size(hdata))
//      return(1);
//  }

//  /* Set other run-time data and adjust min/max/mean up for signed bytes */
//  hdata->headerSize = 1024;
//  hdata->sectionSkip = 0;
//  hdata->yInverted = 0;
//  hdata->headerSize += hdata->next;
//  hdata->packed4bits = 0;
//  hdata->bytesSigned = readBytesSigned(hdata->imodStamp, hdata->imodFlags, hdata->mode,
//                                       hdata->amin, hdata->amax);
//  if (hdata->bytesSigned) {
//    hdata->amin += 128.;
//    hdata->amax += 128.;
//    hdata->amean += 128.;
//  }

//  /* If not an IMOD file, clear out the flags, otherwise retain them */
//  if (hdata->imodStamp != IMOD_MRC_STAMP)
//    hdata->imodFlags = 0;

//  /* Invert origin coming in if this flag is set (added for 4.7 release) or if the
//     MRC version is standard */
//  if ((hdata->imodStamp == IMOD_MRC_STAMP && (hdata->imodFlags & MRC_FLAGS_INV_ORIGIN)) ||
//      mrcGetStandardVersion(hdata) > 0) {
//    hdata->xorg *= -1;
//    hdata->yorg *= -1;
//    hdata->zorg *= -1;
//  }

//  /* Check for inversion either for the original form of FEI file or by MAPR = -2
//     (which is tentative) */
//  if (hdata->imodStamp != IMOD_MRC_STAMP &&
//      ((mrcGetExtendedType(hdata, &i) == MRC_EXT_TYPE_FEI && mrcGetStandardVersion(hdata)
//        == 20140) || hdata->mapr == -2)) {
//    hdata->yInverted = 1;
//    if (hdata->mapr == -2)
//      hdata->mapr = 2;
//    hdata->iiuFlags |= IIUNIT_Y_INVERTED;
//  }

//  for ( i = 0; i < MRC_NLABELS; i ++){
//    if (fread(hdata->labels[i], MRC_LABEL_SIZE, 1, fin) == 0) {
//      b3dError(stderr, "ERROR: mrc_head_read - reading label %d.\n", i);
//      hdata->labels[i][MRC_LABEL_SIZE] = 0;
//      return(-1);
//    }
//    hdata->labels[i][MRC_LABEL_SIZE] = 0;
//    if (i < hdata->nlabl)
//      fixTitlePadding(hdata->labels[i]);
//  }

//  /* Recognize 4 bit mode or packed 4-bit data in half-size byte file */
//  if (hdata->mode == MRC_MODE_4BIT) {
//    hdata->packed4bits = PACKED_4BIT_MODE;
//    hdata->mode = MRC_MODE_BYTE;
//    hdata->iiuFlags |= IIUNIT_4BIT_MODE;
//  } else if (((hdata->imodFlags & MRC_FLAGS_4BIT_BYTES) && hdata->mode == MRC_MODE_BYTE) ||
//             (hdata->mode == MRC_MODE_BYTE &&
//              hdata->nlabl == 1 && strstr(hdata->labels[0], "4 bits packed") &&
//              sizeCanBe4BitK2SuperRes(hdata->nx, hdata->ny))) {
//    hdata->packed4bits = PACKED_HALF_XSIZE;
//    hdata->iiuFlags |= IIUNIT_HALF_XSIZE;
//    if (hdata->mx == hdata->nx) {
//      hdata->mx *= 2;
//      hdata->xlen *= 2.;
//    }
//    hdata->nx *= 2;
//    if (hdata->bytesSigned) {
//      b3dError(stderr, "ERROR: mrc_head_read - cannot read 4-bit data packed in signed "
//               "bytes.\n");
//      return(1);
//    }
//  }

//  if ((hdata->mode > 31) || (hdata->mode < 0)) {
//    b3dError(stderr, "ERROR: mrc_head_read - bad file mode %d.\n", hdata->mode);
//    return(1);
//  }
//  if (hdata->nlabl > MRC_NLABELS) {
//    b3dError(stderr, "ERROR: mrc_head_read - impossible number of "
//             "labels, %d.\n", hdata->nlabl);
//    return(1);
//  }

//  /* 12/31/13: If the map indexes are wrong just fix them */
//  if ((hdata->mapc + 2) / 3 != 1 || (hdata->mapr + 2) / 3 != 1 ||
//      (hdata->maps + 2) / 3 != 1 || hdata->mapc == hdata->mapr ||
//      hdata->mapr == hdata->maps || hdata->mapc == hdata->maps) {
//    hdata->mapc = 1;
//    hdata->mapr = 2;
//    hdata->maps = 3;
//    hdata->iiuFlags |= IIUNIT_BAD_MAPCRS;
//  }

//  /* 12/9/12: To match what is done in irdhdr when mx or xlen is zero;
//     also if Z pixel size is 0, set Z cell to match X pixel size */
//  if (!hdata->mx || hdata->xlen < 1.e-5) {
//    hdata->mx = hdata->my = hdata->mz = 1;
//    hdata->xlen = hdata->ylen = hdata->zlen = 1.;
//  }
//  if (hdata->zlen < 1.e-5)
//    hdata->zlen = hdata->mz * hdata->xlen / hdata->mx;

//  /* DNM 6/10/04: Workaround to FEI goof in which nints was set to # of bytes, 4 * nreal*/
//  if (hdata->nint == 128 && hdata->nreal == 32 &&
//      (hdata->next == 131072 || strstr(hdata->labels[0], "Fei ") ==  hdata->labels[0])) {
//    hdata->nint = 0;
//    hdata->iiuFlags |= IIUNIT_NINT_BUG;
//  }

//  /* DNM 7/2/02: This calculation is won't work for big files and is
//     a bad idea anyway, so comment out the test below */
//  datasize = hdata->nx * hdata->ny * hdata->nz;
//  switch(hdata->mode){
//  case MRC_MODE_BYTE:
//    break;
//  case MRC_MODE_SHORT:
//  case MRC_MODE_USHORT:
//    datasize *= 2;
//    break;
//  case MRC_MODE_FLOAT:
//  case MRC_MODE_COMPLEX_SHORT:
//    datasize *= 4;
//    break;
//  case MRC_MODE_COMPLEX_FLOAT:
//    datasize *= 8;
//    break;
//  case MRC_MODE_RGB:
//    datasize *= 3;
//    break;
//  default:
//    b3dError(stderr, "ERROR: mrc_head_read - bad file mode %d.\n",
//             hdata->mode);
//    return(1);
//  }

//  /* fseek(fin, 0, 2);
//     filesize = ftell(fin); */
//  b3dRewind(fin);

//  /* if ((filesize - datasize) < 0)
//     return(0);
//     if ((filesize - datasize) > 512)
//     return(0); */

//  hdata->fp = fin;

//  return(retval);
//}
MRC::MRC():MRC("",false)
{
    //Empty
}

MRC::MRC(const QString &fileName):MRC("",false)
{
    open(fileName);
}

MRC::MRC(const MRC &rhs)
{
    m_fileName = rhs.m_fileName;
    m_header = rhs.m_header;
    _init();        //init property by header
    _allocate();    //allocate memory by header
    memcpy(m_mrcData,rhs.m_mrcData,m_mrcDataSize*sizeof(unsigned char));
    m_slices = rhs.m_slices;
    m_opened = rhs.m_opened;
}

MRC::MRC(MRC &&rhs)
{
    m_fileName = std::move(rhs.m_fileName);
    m_header = rhs.m_header;
    _init();
    m_mrcData = rhs.m_mrcData;
    m_opened = rhs.m_opened;
    rhs.m_mrcData = nullptr;
    m_slices = std::move(rhs.m_slices);
    qDebug()<<"move constructor has been called\n";
}

MRC & MRC::operator=(const MRC &rhs)
{
    if(this == &rhs)return *this;
    _reset();       //clear all previous item

    m_fileName=rhs.m_fileName;
    m_header = rhs.m_header;
    _init();
    _allocate();
    memcpy(m_mrcData,rhs.m_mrcData,m_mrcDataSize*sizeof(unsigned char));
    m_slices = rhs.m_slices;
    m_opened = rhs.m_opened;
}

MRC &&MRC::operator=(MRC &&rhs)
{
    //if(this == &rhs)return *this;
    _reset();
    m_fileName = std::move(rhs.m_fileName);
    m_header = rhs.m_header;
    _init();
    rhs.m_mrcData = rhs.m_mrcData;
    rhs.m_mrcData=nullptr;
    m_slices = std::move(rhs.m_slices);
    m_opened = rhs.m_opened;
    qDebug()<<"move = operator has been called\n";
}

bool MRC::open(const QString &fileName)
{
    _reset();
    QByteArray cstr=fileName.toLatin1();
    FILE * fp = fopen(cstr.data(),"rb");
    if(fp != nullptr){
        bool noError = true;
        if(true == noError){
            noError = _mrcHeaderRead(fp,&m_header);
        }
        if(true == noError){
            noError = _init();
        }
        if(true == noError){
            noError = _allocate();
        }
        if(true == noError){
            noError = _readDataFromFile(fp);
        }
        m_opened = noError;
        fclose(fp);
    }
    return m_opened;
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



//QImage & MRC::getSlice(int slice)
//{
//    /*It's safe to drop const qualifier in non-const member function*/
//    return
//         const_cast<QImage &>(
//             static_cast<const MRC &>(*this).getSlice(slice)
//                );
//}

QImage MRC::getSlice(int slice) const
{
    int width = getWidth();
    int height = getHeight();
    return QImage(m_mrcData+slice*width*height,width,height,QImage::Format_Grayscale8);
}

QVector<QImage> MRC::getSlices() const
{
    QVector<QImage> imgVec;
    int tot = getSliceCount();
    for(int i=0;i<tot;i++){
        imgVec.push_back(getSlice(i));
    }
    return imgVec;
}

QString MRC::getMRCInfo()const
{
    QString info = _getMRCHeaderInfo(&m_header).c_str();
    //qDebug()<<info;
    return info;
}

MRC::~MRC()
{
    _destroy();
}

//Only for bytes and float
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

std::__1::string MRC::_getMRCHeaderInfo(const MRC::MRCHeader *header) const
{

    std::stringstream ss;
    ss<<"NX:\t\t"<<header->nx<<std::endl
    <<"NY:\t\t"<<header->ny<<std::endl
    <<"NZ:\t\t"<<header->nz<<std::endl
    <<"MODE:\t\t"<<header->mode<<std::endl
    <<"NX START:\t\t"<<header->nxstart<<std::endl
    <<"NY START:\t\t"<<header->nystart<<std::endl
    <<"NZ START:\t\t"<<header->nzstart<<std::endl
    <<"MX:\t\t"<<header->mx<<std::endl
    <<"MY:\t\t"<<header->my<<std::endl
    <<"MZ:\t\t"<<header->my<<std::endl
    <<"XLEN:\t\t"<<header->xlen<<std::endl
    <<"YLEN:\t\t"<<header->ylen<<std::endl
    <<"ZLEN:\t\t"<<header->zlen<<std::endl
    <<"Alpha:\t\t"<<header->alpha<<std::endl
    <<"Beta:\t\t"<<header->beta<<std::endl
    <<"Gamma:\t\t"<<header->gamma<<std::endl
    <<"mapc:\t\t"<<header->mapc<<std::endl
    <<"mapr:\t\t"<<header->mapr<<std::endl
    <<"mapr:\t\t"<<header->maps<<std::endl
    <<"dmin:\t\t"<<header->dmax<<std::endl
    <<"dmax:\t\t"<<header->dmin<<std::endl
    <<"dmean:\t\t"<<header->dmean<<std::endl
    <<"ispg:\t\t"<<header->ispg<<std::endl
    <<"nysmbt:\t\t"<<header->nystart<<std::endl
    <<"ExtTyp:\t\t"<<header->extType[0]<<" "<<header->extType[1]<<" "<<header->extType[2]<<" "<<header->extType[3]<<std::endl
    <<"nVersion:\t\t"<<header->nversion<<std::endl
    <<"xorg:\t\t"<<header->xorg<<std::endl
    <<"yorg:\t\t"<<header->yorg<<std::endl
    <<"zorg:\t\t"<<header->zorg<<std::endl;
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

        if(MRC_MODE_BYTE == m_header.mode){
            int readCount = fread(m_mrcData,sizeof(unsigned char),m_mrcDataSize,fp);
            if(readCount != m_mrcDataSize){
                //QMessageBox::critical(nullptr,tr("Reading Error"),tr("Can not read file.\n"),
                                      //QMessageBox::Ok,QMessageBox::Ok);
                noError =false;
            }

        }else if(MRC_MODE_FLOAT == m_header.mode){
            float * buffer = new float[m_mrcDataSize*sizeof(float)];
            int readCount = fread(buffer,sizeof(float),m_mrcDataSize,fp);
            if(readCount != m_mrcDataSize){
                //QMessageBox::critical(nullptr,tr("Reading Error"),tr("Can not read file.\n"),
                //                      QMessageBox::Ok,QMessageBox::Ok);
                noError = false;
            }
            if(true == noError){
                //Mapping float type to unsigned char type
                float dmin = static_cast<float>(m_header.dmin);
                float dmax = static_cast<float>(m_header.dmax);
                double k = 256.0/(dmax-dmin);
                for(size_t i =0;i<m_mrcDataSize;i++){
                    m_mrcData[i] = static_cast<unsigned char>(k*buffer[i]);
                }
            }
            delete[] buffer;
        }else{
            //QMessageBox::critical(nullptr,tr("Format Error"),tr("Unsupported Format"),QMessageBox::Ok,QMessageBox::Ok);
            return false;
        }
    }

    return (noError);
}

void MRC::_reset()
{
    m_fileName = QString();
    _destroy();
    bool m_opened = false;
    m_mrcDataSize = 0;
}

bool MRC::_init()
{
    size_t nx = static_cast<size_t>(m_header.nx);
    size_t ny = static_cast<size_t>(m_header.ny);
    size_t nz = static_cast<size_t>(m_header.nz);
    m_mrcDataSize = nx*ny*nz;
    return true;
}

bool MRC::_allocate()
{
    m_mrcData = new unsigned char[m_mrcDataSize*sizeof(unsigned char)];
    if(m_mrcData == nullptr){
        return false;
    }
    return true;
}

void MRC::_destroy()
{
    if(m_mrcData != nullptr){
        delete[] m_mrcData;
        m_mrcData = nullptr;
    }
}
