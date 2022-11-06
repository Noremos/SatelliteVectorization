#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQuickItem>
#include <imageitem.h>
#include "barcodeCreator.h"
#include "MatrImg.h"

Q_DECLARE_METATYPE(bc::bline)
Q_DECLARE_METATYPE(QVector<bc::bline*>)

struct Cound;

// Todo.
// 2 режима
class backend : public QObject
{
    Q_OBJECT
public:
    explicit backend(QObject *parent = nullptr);
	~backend()
	{
		delete[] resLinesMap;
	}

	QObject *root = nullptr;
	ImageItem *mainImage = nullptr;
	ImageItem *processedImage = nullptr;
	QQuickItem *sliderP = nullptr;

	MatrImg mainMat;
	MatrImg sourceBack;
	MatrImg resultMart;
	MatrImg maskImg;

	QImage result;
	std::unique_ptr<bc::Barcontainer> barcode = nullptr;

	std::vector<Barscalar> colors;

	Q_INVOKABLE void setup(QQuickItem  *mainImage, QQuickItem* processedImage, QQuickItem *sliderPanel);
	Q_INVOKABLE void run(int procType, int colType, int compType);
	Q_INVOKABLE void run2(QObject *obj);
	Q_INVOKABLE void loadImage(QString path);
	Q_INVOKABLE void exportResult(QString path);
	Q_INVOKABLE void restoreSource();
	Q_INVOKABLE void setTempDir(QString path);

	inline bc::Baritem *getBaritem()
	{
		return barcode ? barcode->getItem(0) : nullptr;
	}

	inline bc::Baritem *getSortedBaritem()
	{
		return barcode ? barcode->getItem(1) : nullptr;
	}

	// //////////////////////////////////////////////
	Q_INVOKABLE void processMain(QQuickItem *iamge, QString extra);
	Q_INVOKABLE void click(int x, int y);
	Q_INVOKABLE void addClass(int classIndex);
	Q_INVOKABLE void undoAddClass();

private:
	QString base_root;
	QString auto_root;
	bc::barline *curSelected = nullptr;
	bc::barline** resLinesMap = nullptr;
	int lastIndex = 0;

	MatrImg mask(bc::Baritem *bar, MatrImg &mat, int st, int ed, QString extra);

	static QImage matToQImage( const MatrImg &mat, bool matHaveAlpha, bool needAlpha = false);
	static MatrImg imread(QString path);
	static void imwrite(QString path,  const MatrImg&  mat, bool saveWithAlpha);
signals:

};

#endif // BACKEND_H
