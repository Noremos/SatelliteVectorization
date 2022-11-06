// ImageItem.h
#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>

class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT

//    Q_PROPERTY(bool wisMode)// MEMBER wisMode
    float zoomer = 1.f;
    QPixmap *current_pixmap = nullptr;

//    QImage current_image;
    QString source;
//    bool wisMode = false;
//    int wisX;
//    int wisY;
//    int wisWid;
//    int wisHei;
	int displayedStartX = 0, displayedStartY = 0;
	int displayedWid, displayedHei;
	float imgCoof;
public:
    ImageItem(QQuickItem *parent = nullptr);
    Q_INVOKABLE void setImage(const QImage &image);
//    Q_INVOKABLE void setRect(int x,int y,int wid,int hei);
    Q_INVOKABLE void setSource(QString src);
    Q_INVOKABLE void setZoom(float proc);
    Q_INVOKABLE void clear();
    void paint(QPainter *painter);
    QImage image() const;
    QString getSource() const;
    int getImageWid();
    int getImageHei();
    ~ImageItem();
	int getRealX(int displayedX);
	int getRealY(int displayedy);
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QString source READ getSource WRITE setSource)
    Q_PROPERTY(int imageWidght READ getImageWid)
    Q_PROPERTY(int imageHeight READ getImageHei)
signals:
    void imageChanged();

};
#endif // IMAGEITEM_H
