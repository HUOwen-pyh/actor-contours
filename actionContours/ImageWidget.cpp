#include "ImageWidget.h"

using std::cout;
using std::endl;

const float ImageWidget::coeff = 1;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	if (ptr_image_->width() != 0) {
		printf("error0 occured!\n");
		exit(0);
	}

	polygon_start = nullptr;
	polygon_end = nullptr;
	p1 = nullptr;
	p2 = nullptr;
	isInitial = false;
	phiNeedDraw = false;
	isActReady = false;

	phi = nullptr;
	float_image = nullptr;
	solver = new actorContour();

}


ImageWidget::~ImageWidget(void)
{
	delete polygon_start;
	polygon_start = nullptr;
	delete polygon_end;
	polygon_end = nullptr;
	delete phi;
	phi = nullptr;
	delete p1;
	p1 = nullptr;
	delete p2;
	p2 = nullptr;
	delete ptr_image_;
	ptr_image_ = nullptr;
	delete ptr_image_backup_;
	ptr_image_backup_ = nullptr;
	delete solver;
	solver = nullptr;
	delete float_image;
	float_image = nullptr;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{

	if (p1 != nullptr && p2 != nullptr) {
		p1->rx() = (width() - ptr_image_->width()) / 2;
		p1->ry() = (height() - ptr_image_->height()) / 2;
		p2->rx() = (width() + ptr_image_->width()) / 2;
		p2->ry() = (height() + ptr_image_->height()) / 2;
	}

	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect((width() - ptr_image_->width()) / 2,
		(height() - ptr_image_->height()) / 2,
		ptr_image_->width(),
		ptr_image_->height());

	painter.drawImage(rect, *ptr_image_);

	static QPen pen(Qt::red, 2);
	painter.setPen(pen);
	if (phiNeedDraw) {
		for (auto i = 0; i < ptr_image_->width(); i++) {
			float prior = -1;
			for (auto j = 0; j < ptr_image_->height(); j++) {
				float cur = (*phi)[i][j];
				//std::cout << cur << " ";
				if (cur == 0.0f) {
					painter.drawPoint(p1->x() + i, p1->y() + j);
				}
				else if ((cur > 0 && prior < 0) || (cur < 0 && prior>0)) {
					painter.drawPoint(p1->x() + i, p1->y() + j);
				}
				prior = cur;
			}
			//std::cout << std::endl;
		}

		for (auto j = 0; j < ptr_image_->height(); j++) {
			float prior = -1;
			for (auto i = 0; i < ptr_image_->width(); i++) {
				float cur = (*phi)[i][j];
				//std::cout << cur << " ";
				if (cur == 0.0f) {
					painter.drawPoint(p1->x() + i, p1->y() + j);
				}
				else if ((cur > 0 && prior < 0) || (cur < 0 && prior>0)) {
					painter.drawPoint(p1->x() + i, p1->y() + j);
				}
				prior = cur;
			}
		}
	}

	painter.end();

	update();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);


		float_image = new std::vector<std::vector<float>>(ptr_image_->width(), std::vector<float>(ptr_image_->height()));
		for (int i = 0; i < ptr_image_->width(); i++)
		{
			for (int j = 0; j < ptr_image_->height(); j++)
			{
				QRgb color = ptr_image_->pixel(i, j);
				float gray_valuef = ((float)qRed(color) + (float)qGreen(color) + (float)qBlue(color)) / 3;
				(*float_image)[i][j] = gray_valuef;
				int gray_value = (int)gray_valuef;
				ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
			}
		}

		*(ptr_image_backup_) = *(ptr_image_);
		delete phi;
		delete p1;
		delete p2;

		phi = new std::vector<std::vector<float>>(ptr_image_->width(), std::vector<float>(ptr_image_->height()));
		p1 = new QPoint((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2);
		p2 = new QPoint((width() + ptr_image_->width()) / 2, (height() + ptr_image_->height()) / 2);
		//std::cout << p1->x() << " " << p2->x() << " " << p1->y() << " " << p2->y() << std::endl;


	}

	cout << "image size: " << ptr_image_->width() << ' ' << ptr_image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	ptr_image_->save(filename);
}

void ImageWidget::Restore()
{

	delete polygon_start;
	polygon_start = nullptr;
	delete polygon_end;
	polygon_end = nullptr;
	for (auto i = 0; i < ptr_image_->width();i++) {
		for (auto j = 0; j < ptr_image_->height(); j++) {
			(*phi)[i][j] = 1.0f;
		}
	}

	isInitial = false;
	phiNeedDraw = false;
	isActReady = false;
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}

void ImageWidget::initialPolygon() {
	if (ptr_image_->width() == 0) {
		return;
	}

	if (polygon_start != nullptr) {
		Restore();
	}

	isInitial = true;
	return;
}

void ImageWidget::mousePressEvent(QMouseEvent* event) {
	
	if (!Qt::LeftButton == event->button()||!isInitial) {
		return;
	}


	QPoint start_point = event->pos();
	if (!inPolyGon(start_point,p1->x(),p2->x(),p1->y(),p2->y())) {
		isInitial = false;
		return;
	}
	polygon_start = new QPoint(start_point.x() - p1->x(),
							   start_point.y() - p1->y());
	polygon_end = new QPoint(start_point.x() - p1->x(),
							 start_point.y() - p2->y());

	update();
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event) {
	if (!isInitial) {
		return;
	}

	QPoint end_point = event->pos();

	auto min = [&](int a, int b) {return a >= b ? b : a; };
	auto max = [&](int a, int b) {return a >= b ? a : b; };

	int w = min(p2->x(), max(p1->x(), end_point.x()));
	int h = min(p2->y(), max(p1->y(), end_point.y()));

	polygon_end->rx() = w - p1->x();
	polygon_end->ry() = h - p1->y();

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event) {
	if (!isInitial) {
		return;
	}

	QPoint end_point = event->pos();

	auto min = [&](int a, int b) {return a >= b ? b : a; };
	auto max = [&](int a, int b) {return a >= b ? a : b; };

	int w = min(p2->x(), max(p1->x(), end_point.x()));
	int h = min(p2->y(), max(p1->y(), end_point.y()));

	polygon_end->rx() = w - p1->x();
	polygon_end->ry() = h - p1->y();
	
	int w0 = min(polygon_end->x(), polygon_start->x());
	int w1 = max(polygon_end->x(), polygon_start->x());
	int h0 = min(polygon_end->y(), polygon_start->y());
	int h1 = max(polygon_end->y(), polygon_start->y());

	polygon_end->rx() = w1;
	polygon_end->ry() = h1;
	polygon_start->rx() = w0;
	polygon_start->ry() = h0;
	setPhi();
	isInitial = false;
	update();
}


bool ImageWidget::inPolyGon(QPoint& point, int lw, int rw, int bh, int ah) {
	if (ptr_image_->width() == 0) {
		return false;
	}

	bool isLeft = (point.x() < lw);
	bool isRight = (point.x() > rw);
	bool isBelow = (point.y() < bh);
	bool isAbove = (point.y() > ah);
	//std::cout << isLeft << " " << isRight << " " << isBelow << " " << isAbove << std::endl;
	
	return !(isLeft || isRight || isBelow || isAbove);
}

void ImageWidget::initialPhiOnPoint(QPoint& point, int i, int j) {
	auto min = [&](int a, int b) {return a >= b ? b : a; };
	auto max = [&](int a, int b) {return a >= b ? a : b; };

	int lw = polygon_start->x() + p1->x();
	int rw = polygon_end->x() + p1->x(); 
	int bh = polygon_start->y() + p1->y(); 
	int ah = polygon_end->y() + p1->y();
	//std::cout << lw << " " << rw << " " << bh << " " << ah << std::endl;
	
	if (inPolyGon(point, lw, rw, bh, ah)) {
		int x = min(point.x() - lw, rw - point.x());
		int y = min(point.y() - bh, ah - point.y());
		(*phi)[i][j] = min(x, y)* coeff;
		return;
	}
	else {
		int x = 0;
		int y = 0;
		if (point.x() < lw) {
			x = lw - point.x();
		}
		else if (point.x() > rw) {
			x = point.x() - rw;
		}

		if (point.y() < bh) {
			y = bh - point.y();
		}
		else if (point.y() > ah) {
			y = point.y() - ah;
		}

		(*phi)[i][j] = -max(x, y)* coeff;
		return;
	}
	printf("error1 occured!\n");
	exit(1);
}

void ImageWidget::setPhi() {

	auto min = [&](int a, int b) {return a >= b ? b : a; };
	auto max = [&](int a, int b) {return a >= b ? a : b; };

	for (auto i = 0; i < ptr_image_->width(); i++) {
		for (auto j = 0; j < ptr_image_->height(); j++) {
			QPoint cur_point(p1->x() + i, p1->y() + j);
			initialPhiOnPoint(cur_point,i,j);
		}
	}
	phiNeedDraw = true;
	isActReady = true;
	/*
	for (auto i = 0; i < ptr_image_->width(); i++) {
		for (auto j = 0; j < ptr_image_->height(); j++) {
			QPoint cur_point(p1->x() + i, p1->y() + j);
			std::cout << (*phi)[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/
}

void ImageWidget::actContour() {
	if (!isActReady) {
		return;
	}

	while (solver->exec(float_image, phi)) {

		repaint();
	}

	return;
}