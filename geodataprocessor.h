#ifndef GEODATAPROCESSOR_H
#define GEODATAPROCESSOR_H

#include "barcodeCreator.h"
#include "MatrImg.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


double getPsa(const bc::barvector &matr);
using mcountor = std::vector<uint>;
void saveAsGeojson(const bc::barlinevector &lines, QString savePath, QPointF startOffset, double coof);
void saveAllJsons(bc::barlinevector* geojson, QString path);

void getCountour(const bc::barvector &points, mcountor &contur, bool aproximate = false);

using std::string;
//static int pr = 10;
//static bool normA = false;
static const int N = 3;

class barclassificator
{
public:
	std::vector<string> categorues;
	bc::Barcontainer classes[N];

	void clear()
	{
		classes[0].clear();
		classes[1].clear();
		classes[2].clear();
	}

	void addClass(bc::barlinevector &cont, int classInd)
	{
		bc::Baritem *item = new bc::Baritem();
		item->barlines = cont;
		item->relen();
		classes[classInd].addItem(item);
	}

private:
	Barscalar asScalar(const QJsonArray &arr)
	{
		return Barscalar(arr.at(0).toDouble(), arr.at(1).toDouble(), arr.at(2).toDouble());
	}

	void parseItem(const QJsonObject &obj, bc::barlinevector &lines)
	{
		bc::barline *line = new bc::barline();
		line->start = asScalar(obj["start"].toArray());
		line->m_end = asScalar(obj["end"].toArray());

		lines.push_back(line);

		auto arrcoors = obj["children"].toArray();
		foreach (auto f, arrcoors)
		{
			parseItem(f.toObject(), lines);
		}
	}

public:
	void addClass(QString path, int classInd)
	{
		QString val;
		QFile file;
		file.setFileName(path);
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		val = file.readAll();
		file.close();

		QJsonDocument jsonDocument(QJsonDocument::fromJson(val.toUtf8()));
		// Из которого выделяем объект в текущий рабочий QJsonObject

		if (jsonDocument.isArray())
		{
			QJsonArray jsonItems = jsonDocument.array();

			foreach (auto jsItem, jsonItems)
			{
				bc::Baritem *item = new bc::Baritem();
				parseItem(jsItem.toObject(), item->barlines);
				item->relen();
				classes[classInd].addItem(item);
			}
		}
		else
		{
			bc::Baritem *item = new bc::Baritem();
			parseItem(jsonDocument.object(), item->barlines);
			item->relen();
			classes[classInd].addItem(item);
		}
	}


	void addClass(bc::barline *line, int classInd)
	{
		bc::Baritem *item = new bc::Baritem();
		line->getChilredAsList(item->barlines, true, true);
		item->relen();

		classes[classInd].addItem(item);
	}

	void removeLast(int classInd)
	{
		classes[classInd].remoeLast();
	}

//	int getType(bc::barlinevector &bar0lines)
	int getType(const bc::Baritem &bar0)
	{
		auto cp = bc::CompireStrategy::CommonToLen;
		float res = 0;

		bc::Baritem newOne(bar0);
		newOne.relen();

		int maxInd = -1;
		float maxP = res;
		for (size_t i = 0; i < N; i++)
		{
			float ps = classes[i].compireBest(&newOne, cp);
			if (ps > maxP)
			{
				maxP = ps;
				maxInd = i;
			}
		}

		assert(maxP <= 1.0);
		return maxP > 0.5 ? maxInd : -1;
	}
};

#endif // GEODATAPROCESSOR_H
