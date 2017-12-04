#include "mrc.h"

MRC::MRC():MRC("",false)
{
    //Do Nothing
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
    m_header = std::move(rhs.m_header);
    _init();
    m_mrcData = std::move(rhs.m_mrcData);
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
    return *this;
}

MRC & MRC::operator=(MRC &&rhs)
{
    //if(this == &rhs)return *this;
	qDebug() << "move assignment operator has been called\n";
    _reset();
    m_fileName = std::move(rhs.m_fileName);
    m_header = std::move(rhs.m_header);
    _init();
    m_mrcData = std::move(rhs.m_mrcData);
    rhs.m_mrcData=nullptr;
    m_slices = std::move(rhs.m_slices);
    m_opened = rhs.m_opened;
    return *this;
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

bool MRC::save(const QString & fileName)
{
	//TODO:
	return false;
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


QImage MRC::getSlice(int slice) const
{
    int width = getWidth();
    int height = getHeight();
    return QImage(m_mrcData+slice*width*height,width,height,QImage::Format_Grayscale8);
}


const unsigned char *MRC::data() const
{
    return m_mrcData;
}

unsigned char *MRC::data()
{
    return const_cast<unsigned char*>(
                static_cast<const MRC *>(this)->data()
                );
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

bool MRC::setSlice(const QImage & image, int slice)
{
	return false;
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

std::string MRC::_getMRCHeaderInfo(const MRC::MRCHeader *header) const
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
