#pragma once
#ifndef MRCMODEL_H_
#define MRCMODEL_H_
#include <QObject>
#include <QString>
#include <QVector>
#include <QImage>
#include <QPair>
#include <QRectF>
#include "mrc.h"
class MRCModel :public QObject
{
	Q_OBJECT
public:
	MRCModel();
	explicit MRCModel(const QString & fileName);
	MRCModel(const MRCModel & model) = delete;
	MRCModel(MRCModel && model) = delete;
	MRCModel & operator=(const MRCModel & model) = delete;
	MRCModel & operator=(MRCModel && model) = delete;
	virtual ~MRCModel();

	void setGrayscaleStrechingLowerBound(int value) { m_mrcContext.grayscaleStrechingLowerBound = value; }
	void setGrayScaleStrechingUpperBound(int value) { m_mrcContext.grayscaleStrechingUpperBound = value; }
	void setGrayScaleStrechBound(int lower, int upper) { setGrayscaleStrechingLowerBound(lower); setGrayScaleStrechingUpperBound(upper); }
	void setGrayScaleLowerBound(int value) { m_mrcContext.grayscaleLowerBound = value; }
	void setGrayScaleUpperBound(int value) { m_mrcContext.grayscaleUpperBound = value; }
	void setGrayScaleBound(int lower, int upper) { setGrayScaleLowerBound(lower); setGrayScaleUpperBound(upper); }
	int getGrayscaleStrechingLowerBound()const { return m_mrcContext.grayscaleStrechingLowerBound; }
	int getGrayscaleStrechingUpperBound()const { return m_mrcContext.grayscaleStrechingUpperBound; }
	int getGrayscaleLowerBound()const { return m_mrcContext.grayscaleLowerBound; }
	int getGrayscaleUpperBound()const { return m_mrcContext.grayscaleUpperBound; }
	bool isValid()const { return m_mrcContext.valid;};
	void setCurrentSlice(int slice) { m_mrcContext.currentSlice=slice; }
	int getCurrentSlice()const { return m_mrcContext.currentSlice; }
	bool save(const QString & fileName);
	bool open(const QString & fileName);
	bool openMarks(const QString & fileName);
	bool saveMarks(const QString & fileName);
	bool isOpened()const { return isValid();}


	QString getMRCInfo()const { return m_mrcFile.getMRCInfo(); };
	QImage getSlice(int index)const;
	void setSlice(const QImage & image, int index);
	QVector<QImage> getSlices()const;
	void setMark(const QImage & iamge, int index);
	QImage getMark(int index);
	QVector<QImage> getMarks()const;

private:
	struct MRCContext {
		MRCContext() :grayscaleStrechingLowerBound{ 0 },
			grayscaleStrechingUpperBound{ 255 },
			grayscaleUpperBound{ 255 },
			grayscaleLowerBound{ 0 },
			currentSlice{ -1 },
			valid{ false } {}
		int grayscaleLowerBound;
		int grayscaleUpperBound;
		int grayscaleStrechingLowerBound;
		int grayscaleStrechingUpperBound;
		int currentSlice;
		qreal zoomFactor;
		QRectF zoomRect;
		bool valid;
	};
private:
	MRC m_mrcFile;
	MRCContext m_mrcContext;
	QVector<QImage> m_marks;
};
#endif