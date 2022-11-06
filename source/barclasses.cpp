#include "barclasses.h"
#include <math.h>

#include <unordered_map>


bc::Barbase::~Barbase() {}


bc::Baritem::Baritem(int wid, BarType type)
{
	this->wid = wid;
	this->type = type;
}



void bc::Baritem::add(Barscalar st, Barscalar len)
{
	barlines.push_back(new barline(st, len, wid));
}



void bc::Baritem::add(bc::barline* line)
{
	barlines.push_back(line);
}


Barscalar bc::Baritem::sum() const
{
	Barscalar ssum = 0;
	for (const barline* l : barlines)
		ssum += l->len();

	return ssum;
}

void bc::Baritem::getBettyNumbers(int* bs)
{
	memset(bs, 0, 256 * sizeof(int));

	for (const barline* l : barlines)
	{
		for (size_t i = l->start.data.b1; i < l->end().data.b1; ++i)
		{
			++bs[i];
		}
	}
}




//
//void cloneGraph(bc::barline* old, bc::barline* newone)
//{
//	for (size_t i = 0; i < old->children.size(); i++)
//	{
//		cloneGraph(old->children[i], newone->children[i]);
//		old->children[i] = newone->children[i]->clone();
//	}
//}


bc::Baritem* bc::Baritem::clone() const
{
	std::unordered_map<barline*, barline*> oldNew;
	Baritem* nb = new Baritem(wid);
	nb->barlines.insert(nb->barlines.begin(), barlines.begin(), barlines.end());
	bool createGraph = false;
	if ((barlines.size() > 0 && barlines[0]->parent != nullptr) || barlines[0]->children.size() > 0)
		createGraph = true;

	for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
	{
		auto* nnew = nb->barlines[i]->clone();
		if (createGraph)
			oldNew.insert(std::pair<barline*, barline*>(nb->barlines[i], nnew));

		nb->barlines[i] = nnew;
	}
	if (createGraph)
	{
		for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
		{
			auto* nline = nb->barlines[i];
			nline->parent = oldNew[nline->parent];

			for (size_t i = 0; i < nline->children.size(); i++)
				nline->children[i] = oldNew[nline->children[i]];
		}
	}
	return nb;
}


Barscalar bc::Baritem::maxLen() const
{
	Barscalar max = 0;
	for (const barline* l : barlines)
		if (l->len() > max)
			max = l->len();

	return max;
}


void bc::Baritem::relen()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	for (size_t i = 1; i < barlines.size(); ++i)
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;

	for (size_t i = 0; i < barlines.size(); ++i)
		barlines[i]->start -= mini;

	//mini = std::min_element(arr.begin(), arr.end(), [](barline &b1, barline &b2) { return b1.start < b2.start; })->start;
	//std::for_each(arr.begin(), arr.end(), [mini](barline &n) {return n.start - uchar(mini); });
}


void bc::Baritem::removePorog(const Barscalar porog)
{
	if (porog == 0)
		return;
	std::vector<barline*> res;
	for (size_t i = 0; i < barlines.size(); i++)
	{
		barline* line = barlines[i];
		if (line->len() >= porog)
			res.push_back(line);
		else// if (line->isCopy)
			delete line;
	}
	barlines.clear();
	barlines.insert(barlines.begin(), res.begin(), res.end());
}


void bc::Baritem::preprocessBar(const Barscalar& porog, bool normalize)
{
	this->removePorog(porog);

	if (normalize)
		this->relen();
}


float findCoof(bc::barline* X, bc::barline* Y, bc::CompireStrategy& strat)
{
	const Barscalar &Xst = X->start < X->end() ? X->start : X->end();
	const Barscalar &Xed = X->start < X->end() ? X->end() : X->start;

	const Barscalar &Yst = Y->start < Y->end() ? Y->start : Y->end();
	const Barscalar &Yed = Y->start < Y->end() ? Y->end() : Y->start;


	float maxlen, minlen;
	if (strat == bc::CompireStrategy::CommonToSum)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st;

		st = MIN(Xst, Yst).getAvgFloat();
		ed = MAX(Xed, Yed).getAvgFloat();
		maxlen = ed - st;
	}
	else if (strat == bc::CompireStrategy::CommonToLen)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		// Start всегда меньше end
		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st; // Может быть меньше 0, если воообще не перекасаются
		maxlen = MAX(X->lenFloat(), Y->lenFloat());
	}
	else
	{
		return X->compire3dbars(Y, strat);
	}

	if (minlen <= 0 || maxlen <= 0)
		return -1;

	assert(minlen <= maxlen);
	return minlen / maxlen;
}

#include <algorithm>


void soirBarlens(bc::barlinevector& barl)
{
	std::sort(barl.begin(), barl.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->len() > b->len();
		});
}



float bc::Baritem::compireBestRes(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0.f;
	int n = static_cast<int>(MIN(barlines.size(), Ybarlines.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t ik = 0;
		size_t jk = 0;
		for (size_t i = 0, totalI = Xbarlines.size(); i < totalI; ++i)
		{
			for (size_t j = 0, totalY = Ybarlines.size(); j < totalY; ++j)
			{
                float coof = findCoof(Xbarlines[i], Ybarlines[j], strat);
				if (coof < 0)
					continue;

				if (coof > maxCoof)
				{
					maxCoof = coof;
					maxsum = Xbarlines[i]->len().getAvgFloat() + Ybarlines[j]->len().getAvgFloat();
					ik = i;
					jk = j;
				}
			}
		}
		Xbarlines.erase(Xbarlines.begin() + ik);
		Ybarlines.erase(Ybarlines.begin() + jk);
		tsum += maxsum * maxCoof;
		totalsum += maxsum;
	}
	return tsum / totalsum;
}


float bc::Baritem::compireFull(const bc::Barbase* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0;
    size_t n = MIN(Xbarlines.size(), Ybarlines.size());
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float tcoof = 0.f;
	for (size_t i = 0; i < n; ++i)
	{
        float coof = findCoof(Xbarlines[i], Ybarlines[i], strat);
		if (coof < 0)
			continue;

		float xysum = static_cast<float>(Xbarlines[i]->len()) + static_cast<float>(Ybarlines[i]->len());
		totalsum += xysum;
		tcoof += xysum * coof;
	}
	return totalsum !=0 ? tcoof / totalsum : 0;
}


float bc::Baritem::compareOccurrence(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	size_t n = static_cast<int>(MIN(Xbarlines.size(), Ybarlines.size()));
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float coofsum = 0.f, totalsum = 0.f;
	for (size_t re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t jk = 0;
		for (size_t j = 0, total2 = Ybarlines.size(); j < total2; ++j)
		{
            float coof = findCoof(Xbarlines[re], Ybarlines[j], strat);
			if (coof < 0)
				continue;

			if (coof > maxCoof)
			{
				maxCoof = coof;
				maxsum = (float)(Xbarlines[re]->len() + Ybarlines[j]->len());
				jk = j;
			}
		}
		Ybarlines.erase(Ybarlines.begin() + jk);
		totalsum += maxsum;
		coofsum += maxsum * maxCoof;
	}
	return coofsum / totalsum;
}


void bc::Baritem::normalize()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	Barscalar maxi = barlines[0]->end();
	for (size_t i = 1; i < barlines.size(); ++i)
	{
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;
		if (barlines[i]->end() > maxi)
			maxi = barlines[i]->end();
	}

	for (size_t i = 0; i < barlines.size(); ++i)
	{
		barlines[i]->start = (barlines[i]->start - mini) / (maxi - mini);
		barlines[i]->m_end = barlines[i]->start + (barlines[i]->len() - mini) / (maxi - mini);
	}
}

using std::string;


void bc::Baritem::getJsonObejct(std::string &out)
{
	string nl = "\r\n";

	out = "{" + nl + "lines: ";

	getJsonLinesArray(out);
	out += nl + '}';
}


void bc::Baritem::getJsonLinesArray(std::string &out)
{
	string nl = "\r\n";

	out = "[ ";

	for (bc::barline *line : barlines)
	{
		line->getJsonObject(out);
		out += ",";
	}

	out[out.length() - 1] = ']';
}



void bc::Baritem::sortByLen()
{
	soirBarlens(barlines);
}


void bc::Baritem::sortBySize()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->matr.size() > b->matr.size();
		});
}


void bc::Baritem::sortByStart()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->start > b->start;
		});
}


bc::Baritem::~Baritem()
{
	for (auto* bline : barlines)
	{
		if (bline!=nullptr)
			delete bline;
	}
	barlines.clear();

	if (rootNode != nullptr)
		delete rootNode;
}

//=======================barcontainer=====================


bc::Barcontainer::Barcontainer()
{
}


Barscalar bc::Barcontainer::sum() const
{
	Barscalar sm = 0;
	for (const Baritem *it : items)
	{
		if (it!=nullptr)
			sm += it->sum();
	}
	return sm;
}


void bc::Barcontainer::relen()
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->relen();
	}
}


Barscalar bc::Barcontainer::maxLen() const
{
	Barscalar mx = 0;
	for (const Baritem* it : items)
	{
		if (it!=nullptr)
		{
			Barscalar curm = it->maxLen();
			if (curm > mx)
				mx = curm;
		}
	}

	return mx;
}


size_t bc::Barcontainer::count()
{
	return items.size();
}


bc::Baritem* bc::Barcontainer::getItem(size_t i)
{
	if (items.size() == 0)
		return nullptr;

	while (i < 0)
		i += items.size();

	while (i >= items.size())
		i -= items.size();

	return items[i];
}

//bc::Baritem *bc::Barcontainer::operator[](int i)
//{
//    if (items.size() == 0)
//        return nullptr;

//    while (i < 0)
//        i += items.size();

//    while (i >= (int) items.size())
//        i -= items.size();

//    return items[i];
//}


bc::Baritem* bc::Barcontainer::lastItem()
{
	if (items.size() == 0)
		return nullptr;

	return items[items.size() - 1];
}


void bc::Barcontainer::addItem(bc::Baritem* item)
{
	items.push_back(item);
}


void bc::Barcontainer::removePorog(const Barscalar porog)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->removePorog(porog);
	}
}


void bc::Barcontainer::preprocessBar(const Barscalar& porog, bool normalize)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->preprocessBar(porog, normalize);
	}
}


bc::Barbase* bc::Barcontainer::clone() const
{
	Barcontainer* newBar = new Barcontainer();

	for (Baritem* it : items)
	{
		if (it!=nullptr)
			newBar->items.push_back(new Baritem(*it));
	}
	return newBar;
}


float bc::Barcontainer::compireFull(const bc::Barbase* bc, bc::CompireStrategy strat) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
    float res = 0;
    float s = static_cast<float>(sum() + bcr->sum());
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		if (items[i]!=nullptr)
			res += items[i]->compireFull(bcr->items[i], strat) * static_cast<float>(items[i]->sum() + bcr->items[i]->sum()) / s;
	}

    return res;
}



float bc::Barcontainer::compireBest(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	float res = 0;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i] != nullptr)
			res = MAX(items[i]->compireFull(bc, strat), res);
	}

	return res;
}



bc::Barcontainer::Barcontainer::~Barcontainer()
{
	clear();
}


#ifdef USE_OPENCV


//-------------BARIMG

Barscalar bc::BarMat::get(int x, int y) const
{
	if (type == BarType::BYTE8_3)
	{
		cv::Vec3b v = mat.at<cv::Vec3b>(y, x);
		return Barscalar(v.val[0], v.val[1], v.val[2]);
	}
	else
	{
		return Barscalar(mat.at<uchar>(y, x), BarType::BYTE8_1);
	}
}

#endif // USE_OPENCV


#include "barImg.h"

bc::BarImg& bc::BarImg::operator+(const Barscalar& v)
{
	BarImg* box = this->getCopy();

	//box->addToMat(v);
	assert(false);

	return *box;
}

//// Overload + operator to add two Box objects.
bc::BarImg& bc::BarImg::operator-(const Barscalar& v)
{
	bc::BarImg* box = this->getCopy();

	assert(false);
	for (size_t i = 0; i < box->length(); ++i)
	{
		Barscalar val = box->getLiner(i);
		val -= v;
		assert(box->getLiner(i) == val);
	}
	return *box;
}
//
//bc::BarImg bc::BarImg::operator+(const Barscalar& c1, BarImg& c2)
//{
//	// use the Cents constructor and operator+(int, int)
//	// we can access m_cents directly because this is a friend function
//	BarImg nimg = c2;
//	nimg.addToMat(c1);
//	return nimg;
//}
//
//
//bc::BarImg bc::BarImg::operator-(const Barscalar& c1, const BarImg& c2)
//{
//	// use the Cents constructor and operator+(int, int)
//	// we can access m_cents directly because this is a friend function
//	BarImg ret(1, 1);
//	ret.assignCopyOf(c2);
//	ret.minusFrom(c1);
//	return ret;
//}
