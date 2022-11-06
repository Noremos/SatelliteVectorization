// ImageItem.cpp
#include "imageitem.h"

ImageItem::ImageItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    this->current_pixmap = new QPixmap(":/images/no_image.png");
}

//    if (wisMode && wisWid<current_image.width() && wisHei<current_image.height()) {
//        int x = qMax(0, wisX - 50);
//        int y = qMax(0, wisY - 50);
//        wid = wisWid + 50;
//        hei = wisHei + 50;
//        if (x + wid >= current_image.width())
//            wid = current_image.width() - x-1;
//        if (y + wid >= current_image.height())
//            hei = current_image.height() - y-1;
//        painter->drawPixmap(wisX, wisY,wisWid,wisHei,QPixmap::fromImage(current_image.copy(x,y,wid,hei)));

//    } else
void ImageItem::paint(QPainter *painter)
{
    if (current_pixmap == nullptr)
		return;

	int pixmapWid = current_pixmap->width();
	int pixmapHei = current_pixmap->height();

	if (pixmapWid <= 0 || pixmapHei <= 0)
		return;


	// img.wid > img.hei
	//	if (pixmapWid >= pixmapHei)
	//	{
	//		imgCoof = static_cast<float>(pixmapWid) / (width() / height());
	//		displayedWid = width();
	//		displayedHei = pixmapHei * imgCoof;
	//		displayedStartX = 0;
	//		displayedStartY = (height() - displayedHei) / 2;
	//	}
	//	else
	//	{
	//		imgCoof = static_cast<float>(pixmapHei) / (height() / width());

	//		displayedHei = height();
	//		displayedWid = pixmapWid * imgCoof;
	//		displayedStartX = (width() - displayedWid) / 2;
	//		displayedStartY = 0;

	//	}

	//	if(width() > height())
	{
		float winCoof = static_cast<float>(width()) / height();
		imgCoof = static_cast<float>(pixmapWid) / pixmapHei;
		if (imgCoof > winCoof)
		{
			displayedWid = width();
			displayedHei = displayedWid / imgCoof;
			displayedStartX = 0;
			displayedStartY = (height() - displayedHei) / 2;
		}
		else
		{
			displayedHei = height();
			displayedWid = displayedHei * imgCoof;
			displayedStartX = (width() - displayedWid) / 2;
			displayedStartY = 0;
		}
	}
//	else
//	{
//		double winCoof = static_cast<double>(height()) / width();
//		imgCoof = static_cast<double>(displayedHei) /displayedWid;
//		if (winCoof>imgCoof) {
//			displayedWid = width();
//			displayedHei = displayedWid * imgCoof;
//			displayedStartY = (height() - displayedHei) / 2;
//		} else {
//			displayedHei = height();
//			displayedWid = displayedHei / imgCoof;
//			displayedStartX = (width() - displayedWid) / 2;
//		}
//	}
//	imgCoof = 1.0 / imgCoof;
	//    current_pixmap->scaled(QSize(width(),height()), Qt::AspectRatioMode::KeepAspectRatio, Qt::FastTransformation)
	painter->drawPixmap(displayedStartX, displayedStartY, displayedWid, displayedHei, *current_pixmap);
}

QImage ImageItem::image() const
{
    return current_pixmap==nullptr?QImage():current_pixmap->toImage();
}

QString ImageItem::getSource() const
{
    return source;
}

int ImageItem::getImageWid()
{
    return current_pixmap == nullptr?0:current_pixmap->width();
}


int ImageItem::getImageHei()
{
    return current_pixmap == nullptr ? 0:current_pixmap->height();
}


void ImageItem::setImage(const QImage &image)
{
    if (current_pixmap != nullptr)
        delete current_pixmap;
    this->current_pixmap = new QPixmap(QPixmap::fromImage(image)); //QPixmap::fromImage(image));
//    qDebug() << current_pixmap->loadFromData(image.bits(), image.sizeInBytes());
    update();
}

//void ImageItem::setRect(int x, int y, int wid, int hei)
//{
//    wisX = x;
//    wisY = y;
//    wisWid = wid;
//    wisHei = hei;
//}

void ImageItem::setSource(QString src)
{
    if(current_pixmap!=nullptr)
        delete current_pixmap;

    if(src.startsWith("file:///"))
        this->current_pixmap = new QPixmap(src.remove(0, 8));
    else if (src.startsWith("data:image/png;base64,"))
    {
        this->current_pixmap = new QPixmap();
        current_pixmap->loadFromData(src.remove(0, 22).toStdString().c_str());
    } else
        this->current_pixmap = new QPixmap();
//    setWidth(current_image.width());
//    setHeight(current_image.height());
    update();
}

void ImageItem::setZoom(float proc)
{
    zoomer = proc;
    update();
}

void ImageItem::clear()
{
    if (current_pixmap != nullptr)
        delete current_pixmap;
    current_pixmap = nullptr;
    update();
}

ImageItem::~ImageItem()
{
	if (current_pixmap != nullptr) {
		delete current_pixmap;
	}
	current_pixmap = nullptr;
}

int ImageItem::getRealX(int displayedX)
{
	int x = displayedX - displayedStartX;
	if (x< 0)
		return 0;
	float otn = (float)getImageWid() / displayedWid;
	x *= otn;
	return x;
}

int ImageItem::getRealY(int displayedy)
{
	int y = displayedy - displayedStartY;
	if (y <0)
		return 0;
	float otn = (float) getImageHei() / displayedHei;
	y *= otn;
	return y;
}
