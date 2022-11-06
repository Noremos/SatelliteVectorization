#include "backend.h"
#include "geodataprocessor.h"

#include <QTemporaryFile>
#include <QRandomGenerator>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>

barclassificator classer;
bc::barlinevector geojson[3];


bool getNumber(QString path, int &numI, QString &num)
{
	int del = path.lastIndexOf('/');
	int del2 = path.indexOf('_', del);

	num = path.mid(del + 1, del2 - del - 1);
	bool okParce = false;
	numI = num.toInt(&okParce);
	return okParce;
}

backend::backend(QObject *parent) : QObject(parent)
{
	srand(300);
	colors.push_back(Barscalar(255, 0, 0));
	colors.push_back(Barscalar(0, 255, 0));
	colors.push_back(Barscalar(0, 0, 255));
	for (int k = 0; k < 40; ++k)
		colors.push_back(Barscalar(5 +  rand() % 250, 5 + rand() % 250, 5 + rand() % 250));

	colors.push_back(Barscalar(255, 255, 255));

	for (int s = 0, total = classer.categorues.size(); s < total; ++s)
	{
		qDebug() << QString::fromStdString(classer.categorues[s]) << " : " << QString::fromStdString(colors[s].text());
	}
	//		for (int b = 0; b < 255; b += 20)
	//			for (int g = 255; g > 20; g -= 20)
	//				for (int r = 0; r < 255; r += 100)
	//					colors.push_back(Barscalar(b, g, r));

	classer.categorues.push_back("bad");
	classer.categorues.push_back("roof двускатные");
	classer.categorues.push_back("roof односкатные");

	QString ds = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	QString fileIni = QDir::cleanPath(ds + QDir::separator() + "Qbinbar.conf");
	QFile intif(fileIni);
	if (intif.open(QIODevice::ReadOnly))
	{
		QString line = QString::fromUtf8(intif.readAll());
		ds = QDir::cleanPath(line);
		intif.close();
	}
	else
	{
		ds = QDir::cleanPath(ds + QDir::separator() + "Qbinbar");
	}

	setTempDir(ds);
}

void backend::setup(QQuickItem *mainImage, QQuickItem* processedImage, QQuickItem  *sliderPanel)
{
    this->mainImage = mainImage->findChild<ImageItem*>("mainImage");
	this->processedImage = reinterpret_cast<ImageItem*>(processedImage);
    //    QtCharts::QChart *obj = barchartP->findChild<QtCharts::QChart *>("barchart");
	this->sliderP = sliderPanel;
}

void backend::run(int procType, int colType, int compType)
{
	if (root == nullptr || mainMat.width() <= 1 || mainMat.height() <= 1)
		return;

	bc::BarcodeCreator bc;
	bc::BarConstructor constr;
	constr.createBinaryMasks = true;
	constr.createGraph = true;
	constr.attachMode = bc::AttachMode::morePointsEatLow;
	constr.returnType = bc::ReturnType::barcode2d;
	constr.addStructure((bc::ProcType) procType, (bc::ColorType) colType, (bc::ComponentType) compType);
	barcode.reset(bc.createBarcode(&mainMat, constr));

	auto bar = getBaritem()->clone();

	bar->sortBySize();
	barcode->addItem(bar);

	processedImage->setImage(matToQImage(mainMat,false,false));
}

void backend::run2(QObject *obj)
{
    qDebug() << obj->objectName();
}

#define ppair(x,y,chr) (std::pair<bc::point,uchar>(bc::point(x,y), chr))

void backend::loadImage(QString path)
{
    if (root == nullptr)
		return;

	mainMat = imread(path);
	if (mainMat.width() == 0)
		return;

	if (resLinesMap != nullptr)
		delete[] resLinesMap;

	curSelected = nullptr;
	lastIndex = -1;

	resLinesMap = new bc::barline *[mainMat.length()];
	mainImage->setSource("file:///" + path);

	sourceBack.assignCopyOf(mainMat);
}


///////////////////////==============

MatrImg backend::imread(QString path)
{
	QImage img(path);

	int i = 0;
	switch (img.format())
	{
	case QImage::Format::Format_Grayscale8:
		i = 1;
		break;
	case QImage::Format::Format_RGB888:
		i = 3;
		break;
	default:
		img.convertTo(QImage::Format::Format_RGB888);
		i = 3;
		break;
	}

	MatrImg e(img.width(), img.height(), i);

	int lineLen = e.bytesInLine();
	for (int y = 0; y < img.height(); y++)
	{
		memcpy(e.lineOffet(y), img.scanLine(y), lineLen);
	}
	return e;
}


void backend::exportResult(QString path)
{
	result.save(QDir::cleanPath(path + QDir::separator() + "result.png"));
	saveAllJsons(geojson, path + QDir::separator());
}

void backend::restoreSource()
{
	mainMat.assignCopyOf(sourceBack);
}

void backend::setTempDir(QString path)
{
	QDir dir(path);
	if (!dir.exists())
		dir.mkpath(".");

	if (path == base_root)
		return;

	base_root = path;
	auto_root = path + QDir::separator();

	QString ds = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	QString fileIni = QDir::cleanPath(ds + QDir::separator() + "Qbinbar.conf");
	QFile intif(fileIni);
	ds = QDir::cleanPath(ds + QDir::separator() + "Qbinbar");
	if (intif.open(QIODevice::WriteOnly))
	{
		intif.write(auto_root.toUtf8());
		intif.close();
	}

	QDir directory(auto_root);
	QStringList images = directory.entryList(QStringList() << "*.json", QDir::Files);

	classer.clear();
	foreach (QString filename, images)
	{
		//do whatever you need to do
		int del2 = filename.indexOf('_');
		QString num = filename.left(del2);
		bool ok;
		int numi = num.toInt(&ok);
		assert(ok);
		classer.addClass(auto_root + filename, numi);
	}

	qDebug() << "Setted" << auto_root;
}

void backend::imwrite(QString path, const MatrImg& mat,bool saveWithAlpha)
{
	QImage res = matToQImage(mat, false, saveWithAlpha);
    res.save(path);
}

QImage backend::matToQImage(const MatrImg& mat, bool matHaveAlpha, bool needAlpha)
{
    QImage::Format format = QImage::Format_RGB888;
    if (mat.channels() == 1)
    {
        format = QImage::Format_Grayscale8;
    } else
	{
		format = QImage::Format_RGB888;
//        if (needAlpha) {
//            cv::cvtColor(mat, temp,matHaveAlpha?cv::COLOR_BGRA2RGBA:cv::COLOR_BGR2RGBA);
//            format = QImage::Format_RGBA8888;
//        } else
//            cv::cvtColor(mat, temp, matHaveAlpha?cv::COLOR_BGRA2RGB:cv::COLOR_BGR2RGB);
	}

	QImage res(mat.wid(), mat.hei(), format);

	for (int y = 0; y < mat.hei(); y++)
	{
		memcpy(res.scanLine(y), mat.lineOffet(y), mat.bytesInLine());
	}
	return res;
}

// /////////////////////////////
void swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}

void genLen(int *st, int *end, int max)
{
	*st = QRandomGenerator::global()->generate() % max;
	*end = QRandomGenerator::global()->generate() % max;
	if (*st > *end)
	{
		int temp = *st;
		*st = *end;
		*end = temp;
	}
}

int rng(int st,int ed)
{
    return st + QRandomGenerator::global()->generate() % (ed - st);
}


uchar maxmin(int val)
{
    if (val >= 255)
        return 255;
    if (val < 0)
        return 0;
    return val;
}

#include "MatrImg.h"


void fitInto(int sourceLen, int newLen, int &st, int &ed)
{
	st = newLen * static_cast<float>(st) / sourceLen;
	ed = newLen * static_cast<float>(ed) / sourceLen;
}

MatrImg backend::mask(bc::Baritem *bar, MatrImg &mat, int st, int ed, QString extra)
{
	MatrImg outMask(mat.wid(), mat.hei(), 1);
	outMask.fill(0);
	mat.assignCopyOf(mainMat);
	double masds = mat.length();
	auto checkLmd = [masds](bc::barline *line) -> bool {
		double dfs = line->matr.size() * 100.f / masds;
		if (dfs < 0.015) // 0.0148 // 1.2992
			return false;
		if (dfs > 5)
			return false;
		return true;
	};

	curSelected = nullptr;
	lastIndex = -1;

	std::unordered_map<size_t, char> map;
	bc::barlinevector resLines;

	for (int i = 0, total = bar->barlines.size(); i < total; ++i)
	{
		auto b = bar->barlines[i];
		if (!checkLmd(b))
			continue;

		if (b->getDeath() > 4)
			continue;
//		if (b->len() < Barscalar(5, 5, 5))
//			continue;
		if (b->start < Barscalar(16, 16, 16)) //  Barscalar(5, 5, 5)
			continue;

		resLines.push_back(b);
		map[reinterpret_cast<size_t>(b)] = -2;
	}

	int total = resLines.size();
	int rTotal = total;
//	for (int i = 0; i < total; ++i)
//	{
//		auto b = resLines[i];
//		if (b->parent && map.find(reinterpret_cast<size_t>(b->parent)) != map.end())
//		{
//			--rTotal;
//			resLines[i] = nullptr;
//		}
//	}

//	bool ent = extra.indexOf("entr;") != -1;
	bool classif = extra.indexOf("barclass;") != -1;
	bool showBad = extra.indexOf("show0;") != -1;
	bool showWhite = extra.indexOf("showw;") != -1;

	geojson[0].clear();
	geojson[1].clear();
	geojson[2].clear();

	fitInto(bar->barlines.size(), rTotal, st, ed);
	memset(resLinesMap, 0, mainMat.length() * sizeof(bc::barline *));
	std::string jsonS = "[";
	for (int i = st; i < ed; ++i)
	{
		auto b = resLines[i];

		const auto &matr = b->getMatrix();

		if (classif)
		{
			int type;
			bc::barlinevector tlines;
			{
				bc::Baritem item;
				b->getChilredAsList(item.barlines, true, true);
				type = classer.getType(item);
				tlines = std::move(item.barlines);
				item.barlines.clear();
			}

//			auto *par = b->parent;
//			while (par)
//			{
//				auto part = map.find(reinterpret_cast<size_t>(par));
//				if (part != map.end() && part->second != -2)
//				{
//					map[(size_t) par] = -2;
//					break;
//				}
//				par = par->parent;
//			}
//			if (par != nullptr)
//				continue;

			map[(size_t) b] = type;

			if (type > 0)
				geojson[type].push_back(b);
			else if (!showBad)
				continue;

			if (type == -1)
				type = colors.size() - 1;

			if (showWhite && type == 0)
				continue;

			Barscalar color = colors[type];
			for (const auto &pm : matr)
			{
				int x = pm.getX();
				int y = pm.getY();
				mat.set(x, y, color);
				outMask.set(x, y, 255);
				int ind = y * mat.wid() + x;
				auto *temp = resLinesMap[ind];
				if (temp == nullptr || /*map[(size_t)temp] == -2 ||*/ temp->getDeath() < b->getDeath())
					resLinesMap[ind] = b;
			}
		}
		else
		{
			for (const auto &pm : matr)
			{
				int x = pm.getX();
				int y = pm.getY();
				mat.set(x, y, colors[i % colors.size()]);
				outMask.set(x, y, 255);
				resLinesMap[y * mat.wid() + x] = b;
			}
		}
	}
	return outMask;
}


void backend::processMain(QQuickItem *iamge, QString extra)
{
	if (!barcode)
	{
		return;
	}

	MatrImg mat(mainMat.width(), mainMat.height(), mainMat.channels());

	auto bar = getBaritem();

	int barsize = bar->barlines.size();
	int st = 0;
	int ed = barsize;

	auto maskResult = mask(bar, mat, st, ed, extra);

	resultMart.assignCopyOf(mat);
	ImageItem *img = qobject_cast<ImageItem*>(iamge);
	result = matToQImage(mat, false, false);
	img->setImage(result);
}

void backend::click(int x, int y)
{
	if (mainImage == nullptr || resLinesMap == nullptr)
		return;

	x = mainImage->getRealX(x);
	y = mainImage->getRealY(y);

	bc::barline *line = resLinesMap[y * mainMat.wid() + x];
	if (line)
	{
		if (curSelected == line && line->parent)
			line = line->parent;

		curSelected = line;
		qDebug() << line->matr.size() * 100.f / mainMat.length();
		MatrImg temp;
		temp.assignCopyOf(resultMart);
		bc::barvector &fullmatr = line->matr;

		mcountor cont;
		getCountour(fullmatr, cont, false);
		for (int i = 0, total = cont.size(); i < total; ++i)
		{
			auto p = bc::barvalue::getStatPoint(cont[i]);
			temp.set(p.x, p.y, Barscalar(255, 191, 0));
		}

		result = matToQImage(temp, false, false);
		processedImage->setImage(result);

		int offX = 9999;
		int offY = 9999;
		int maxX = 0;
		int maxY = 0;

//		auto matr = line->getMatrix();
//		for (const auto &pm : matr)
//		{
//			int x = pm.getX();
//			int y = pm.getY();
//			offX = MIN(x, offX);
//			offY = MIN(y, offY);

//			maxX = MAX(x, maxX);
//			maxY = MAX(y, maxY);
//		}

//		MatrImg out(maxX - offX + 1, maxY - offY + 1, 3);
//		out.fill(255);

//		for (const auto &pm : matr)
//		{
//			int x = pm.getX();
//			int y = pm.getY();
//			out.set(x - offX, y - offY, mainMat.get(x, y));
//		}
//		imwrite("D:\\r.png", out, false);
	}
}

void backend::addClass(int classIndex)
{
	if (classIndex < 0 || classIndex >= N)
		return;

	std::string jsonS;
	curSelected->getJsonObject(jsonS, true, false, true);
	QString name, suff;
	name = QString::number(classIndex);
	QFile temp;
	QString doorPat = auto_root + "%1_%2.json";
	do
	{
		suff = QString::number(QRandomGenerator::global()->generate());
	} while (temp.exists(doorPat.arg(name, suff)));

	doorPat = doorPat.arg(name, suff);

	QFile file(doorPat);
	if (file.open(QIODevice::WriteOnly))
	{
		classer.addClass(curSelected, classIndex);
		file.write(jsonS.c_str(), jsonS.length());
		file.close();
		lastIndex = classIndex;
	}
}

void backend::undoAddClass()
{
	if (lastIndex != -1)
	{
		classer.removeLast(lastIndex);
		lastIndex = -1;
	}
}

