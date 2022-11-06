#include "geodataprocessor.h"

#include <QDebug>
#include <QPointF>
#include <QStack>


double getPsa(const bc::barvector &matr)
{
	std::unordered_map<uint, bool> map;

	int minX = 10000, maxX = 0, minY = 1000, maxY = 0;
	for (const auto &pm : matr)
	{
		map[pm.index] = true;
		int xa = pm.getX();
		int ya = pm.getY();
		if (xa > maxX)
			maxX = xa;
		if (xa < minX)
			minX = xa;

		if (ya > maxY)
			maxY = ya;
		if (ya < minY)
			minY = ya;
	}

	double entr = 0;
	bool foundPrev = map.find(bc::barvalue::getStatInd(minX, minY)) != map.end();
	for (int x = minX; x < maxX; ++x)
	{
		double locEntr = 1.0;
		for (int y = minY; y < maxY; ++y)
		{
			bool foundCur = map.find(bc::barvalue::getStatInd(x, y)) != map.end();

			locEntr *= 0.5;
			if (foundPrev != foundCur)
			{
				entr += locEntr;
				locEntr = 1.0;
			}
		}

		if (locEntr != 1.0)
			entr += locEntr;
	}

	for (int y = minY; y < maxY; ++y)
	{
		double locEntr = 1.0;
		for (int x = minX; x < maxX; ++x)
		{
			bool foundCur = map.find(bc::barvalue::getStatInd(x, y)) != map.end();
			locEntr *= 0.5;
			if (foundPrev != foundCur)
			{
				entr += locEntr;
				locEntr = 1.0;
			}
		}

		if (locEntr != 1.0)
			entr += locEntr;
	}

	return entr;
}


#define DICT_EXISTS(DICT, X, Y) (DICT.find(bc::barvalue::getStatInd(X, Y)) != DICT.end())

class MapCountur
{
	int x = 0, y = 0;
	int stIndex = 0;
	mcountor &contur;
	std::unordered_map<uint, bool> points;

	enum StartPos : char { LeftMid = 0, LeftTop = 1, TopMid = 2, RigthTop = 3, RigthMid = 4, RigthBottom = 5, BottomMid = 6, LeftBottom = 7 };

	StartPos dirct = RigthMid;
	QStack<StartPos> dirs;
	QStack<uint> pointsStack;

public:
	MapCountur(mcountor &contur) : contur(contur) {}

	void run(const bool aproxim = false)
	{
		static int poss[16][2] = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};

		dirct = RigthMid;
		while (true)
		{
			char start = (dirct + 6) % 8; // Safe minus 2
			char end = start + 5;
			// Check
			// 1 2 3
			// 0 X 4
			int prevS = getIndex();

			for (; start < end; ++start)
			{
				int *off = poss[(int) start];
				if (tryVal(off[0], off[1]))
				{
					break;
				}
			}

			if (start != end)
			{
				int s = getIndex();
				StartPos old = dirct;
				dirs.push(dirct);
				dirct = (StartPos)(start % 8);

				// Check new disk with the old one
				if (dirct != old || !aproxim)
				{
					contur.push_back(prevS);
//					contur.push_back(s);
				}

				if (s == stIndex)
				{
					break;
				}
			}
			else
			{
				unset();

				if (pointsStack.size() < 1)
					break;
				//				if (addIndex < 1)
				//					break;

				StartPos old = dirct;
				dirct = dirs.pop();

				if (dirct != old || !aproxim)
					contur.pop_back();

				auto p = bc::barvalue::getStatPoint(pointsStack.pop());
				//contur[addIndex]);
				x = p.x;
				y = p.y;
			}
		}
	}

	void set(const bc::barvalue &p) { points[p.getIndex()] = true; }

	void setStart(int x, int y)
	{
		this->x = x;
		this->y = y;
		stIndex = bc::barvalue::getStatInd(x, y);
	}

private:

	void unset() { points[bc::barvalue::getStatInd(x, y)] = false; }

	bool exists(int xl, int yl)
	{
		auto ds = points.find(bc::barvalue::getStatInd(xl, yl));
		if (ds == points.end())
			return false;

		return ds->second;
	}

	bool tryVal(int oX, int oY)
	{
		if (exists(x + oX, y + oY))
		{
			pointsStack.push(getIndex());
			x += oX;
			y += oY;
			return true;
		}
		return false;
	}

	uint getIndex()
	{
		return bc::barvalue::getStatInd(x, y);
	}
};


void getCountour(const bc::barvector &points, mcountor &contur, bool aproximate)
{
	contur.clear();

	int rect[4]{99999999, 99999999, 0, 0};
	int stY;
	MapCountur dictPoints(contur);
	for (auto &p : points)
	{
		dictPoints.set(p);
		int x = p.getX();
		int y = p.getY();
		if (x < rect[0])
		{
			rect[0] = x;
			stY = y;
		}
		if (x > rect[2])
		{
			rect[2] = x;
		}
		if (y < rect[1])
		{
			rect[1] = y;
		}
		if (y > rect[3])
		{
			rect[3] = y;
		}
	}

	int wid = rect[2] - rect[0];
	int hei = rect[3] - rect[1];

	if (wid < 5 || hei < 5)// || (wid > 1000 && hei > 1000))
		return;

	dictPoints.setStart(rect[0], stY);
	dictPoints.run(aproximate);
}

void saveAllJsons(bc::barlinevector *geojson, QString path)
{
	QPointF start(55.578100, 42.052772);
	double coof = 0.00001;

	saveAsGeojson(geojson[1], path + "2skat.geojson", start, coof);
	saveAsGeojson(geojson[2], path + "1skat.geojson", start, coof);
}

void saveAsGeojson(const bc::barlinevector &lines, QString savePath, QPointF startOffset, double coof)
{
	QString json = "{\"type\":\"FeatureCollection\","
				   "\"name\":\"Roofs\","
				   "\"crs\": { \"type\": \"name\", \"properties\":{\"name\": \"urn:ogc:def:crs:EPSG::3857\" } }," //3857
				   "\"features\":[ ";

	json += "";

	for (int i = 0, total = lines.size(); i < total; ++i)
	{
		QString safsd = "{ \"type\": \"Feature\",";
		safsd += QString("\"properties\":{\"id\": %1 },").arg(i + 1);
		safsd += "\"geometry\": {\"type\":\"MultiPolygon\", \"coordinates\":[[[ ";

		auto &allPoints = lines[i]->getMatrix();
		mcountor cont;
		getCountour(allPoints, cont, true);

		int totalc = cont.size();
		for (int j = 0; j < totalc; ++j)
		{
			const auto &p = bc::barvalue::getStatPoint(cont[j]);
			double x = startOffset.x() - p.y * coof;
			double y = startOffset.y() + p.x * coof;
			safsd += QString("[%1, %2],").arg(y, 0, 'g', 8).arg(x, 0, 'g', 8);
		}

		if (totalc > 0)
		{
			safsd[safsd.length() - 1] = ']';
			safsd += "]]}},";
		}
		else
		{
			safsd += "]]]}},";
		}

		json += safsd;
	}

	json[json.length() - 1] = ']';
	json += "}";

	QFile file(savePath);
	if (file.open(QIODevice::WriteOnly)) {
		file.write(json.toUtf8());
		file.close();
	}
	//	widget->importedMakrers->release();
	//	//	Size2 size = imgsrch.getTileSize();
}
