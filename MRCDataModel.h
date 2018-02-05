#pragma once
#ifndef MRCMODEL_H_
#define MRCMODEL_H_
#include <QObject>
#include <QString>
#include <QVector>
#include <QPicture>
//#include <qDebug>
#include <qdebug.h>
#include <QImage>
#include <QPair>
#include <QRectF>
#include "mrc.h"

class MRCDataBaseModel
{
public:

	MRCDataBaseModel();
    MRCDataBaseModel(const QString & fileName,int width,int height);
	virtual ~MRCDataBaseModel();

	int getWidth()const { return m_mrcFile.getWidth(); }
	int getHeight()const { return m_mrcFile.getHeight(); }
	int getSliceCount()const { return m_mrcFile.getSliceCount(); }
	bool save(const QString & fileName);
	bool open(const QString & fileName);

	bool isOpened()const { return m_mrcFile.isOpened(); }
private:
	MRC m_mrcFile;

};

class MRCDataModel //:public QObject
{
	//Q_OBJECT
public:
    enum class MarkFormat {MRC,RAW,mks};
    enum class DataFormat{mrc,raw};
public:
	MRCDataModel();
    explicit MRCDataModel(const QString & fileName);
	explicit MRCDataModel(const MRCDataModel & model);
	explicit MRCDataModel(MRCDataModel && model);
	MRCDataModel & operator=(const MRCDataModel & model);
	MRCDataModel & operator=(MRCDataModel && model);
	virtual ~MRCDataModel();

	void setGrayscaleStrechingLowerBound(int value) { m_mrcContext.grayscaleStrechingLowerBound = value; }
	void setGrayScaleStrechingUpperBound(int value) { m_mrcContext.grayscaleStrechingUpperBound = value; }
	void setGrayscaleStrechBound(int lower, int upper) { setGrayscaleStrechingLowerBound(lower); setGrayScaleStrechingUpperBound(upper); }
	void setGrayscaleLowerBound(int value) { m_mrcContext.grayscaleLowerBound = value; }
	void setGrayscaleUpperBound(int value) { m_mrcContext.grayscaleUpperBound = value; }
	void setGrayscaleBound(int lower, int upper) { setGrayscaleLowerBound(lower); setGrayscaleUpperBound(upper); }
	int getGrayscaleStrechingLowerBound()const { return m_mrcContext.grayscaleStrechingLowerBound; }
	int getGrayscaleStrechingUpperBound()const { return m_mrcContext.grayscaleStrechingUpperBound; }
	int getGrayscaleLowerBound()const { return m_mrcContext.grayscaleLowerBound; }
	int getGrayscaleUpperBound()const { return m_mrcContext.grayscaleUpperBound; }

	int getMinGrayscale()const { return m_mrcContext.minGrayscale; }
	int getMaxGrayscale()const { return m_mrcContext.maxGrayscale; }

	int getWidth()const { return m_mrcFile.getWidth(); }
	int getHeight()const { return m_mrcFile.getHeight(); }

	QImage getOriginalSlice(int index)const;

	void setZoomFactor(qreal factor) { m_mrcContext.zoomFactor = factor; }
	qreal getZoomFactor()const { return m_mrcContext.zoomFactor; }

	void setZoomRegion(QRect region) { m_mrcContext.zoomRegion = region; qDebug() << "setZoomRegion:"<<region; }
	QRect getZoomRegion()const { qDebug() << "getZoomRegion:"<<m_mrcContext.zoomRegion; return m_mrcContext.zoomRegion;  }

	bool isValid()const { return m_mrcContext.valid;}
	void setCurrentSlice(int slice) { m_mrcContext.currentSlice=slice; }
	int getCurrentSlice()const { return m_mrcContext.currentSlice; }
	int getSliceCount()const { return m_mrcFile.getSliceCount(); }
    bool save(const QString & fileName, MRCDataModel::DataFormat formate = MRCDataModel::DataFormat::mrc);
	bool open(const QString & fileName);
	bool isOpened()const { return isValid(); }



	bool openMarks(const QString & fileName);
	bool saveMarks(const QString & fileName,MarkFormat format = MarkFormat::MRC);
	QString getMRCInfo()const { return QString(QString::fromLocal8Bit(m_mrcFile.getMRCInfo().c_str())); }
	QImage getSlice(int index)const;
	void setSlice(const QImage & image, int index);
	QVector<QImage> getSlices()const;
    void setMark(const QPicture & mark, int index);
    void addMark(int slice,const QPicture& mark);
	QPicture getMark(int index)const;
    QVector<QPicture> getMarks(int slice)const;

private:
	struct MRCContext {
		MRCContext() :grayscaleStrechingLowerBound{ 0 },
			grayscaleStrechingUpperBound{ 255 },
			grayscaleUpperBound{ 255 },
			grayscaleLowerBound{ 0 },
			minGrayscale{ 0 },
			maxGrayscale{ 255 },
			currentSlice{ -1 },
            zoomFactor{1.0},
            zoomRegion{},
			valid{ false } {}
		int grayscaleLowerBound;
		int grayscaleUpperBound;
		int grayscaleStrechingLowerBound;
		int grayscaleStrechingUpperBound;
		int minGrayscale;
		int maxGrayscale;
		int currentSlice;
		qreal zoomFactor;
        QRect zoomRegion;
		bool valid;
	};
private:
	MRC m_mrcFile;

	MRCContext m_mrcContext;

    QVector<QVector<QPicture>> m_marks;

	QVector<QImage> m_modified;

	QVector<bool> m_modifiedFlags;
};
#endif
