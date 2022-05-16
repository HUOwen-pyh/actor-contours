#pragma once
#include <QWidget>
#include "actorContour.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent* paintevent);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Restore();												// Restore image to origin
	void initialPolygon();                                      //initialize zero level set

	void actContour();

public:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:

	QImage*      ptr_image_;				// image 
	QImage*      ptr_image_backup_;

	QPoint*      polygon_start;
	QPoint*      polygon_end;

	QPoint*      p1;
	QPoint*      p2;

	bool         isInitial;

	std::vector<std::vector<float>>* phi;
	std::vector<std::vector<float>>* float_image;

	actorContour* solver;

private:

	static const float coeff;
	bool isActReady;
	bool phiNeedDraw;
	bool inPolyGon(QPoint& point,int lw,int rw,int bh,int ah);
	void setPhi();
	void initialPhiOnPoint(QPoint& point,int i,int j);
};