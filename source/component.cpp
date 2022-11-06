#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>


void bc::Component::init(BarcodeCreator* factory, const Barscalar& val)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curIndexInSortedArr;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline(factory->workingImg->wid());
	resline->start = val;
	resline->m_end = val;
	lastVal = val;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter();
}


bc::Component::Component(poidex pix, const Barscalar& val, bc::BarcodeCreator* factory)
{
	init(factory, val);

	// factory->lastB++;

	add(pix, factory->getPoint(pix), val);
}


bc::Component::Component(bc::BarcodeCreator* factory, bool /*create*/)
{
	init(factory, factory->curbright);
}

Barscalar bc::Component::getStart()
{
	assert(resline != NULL);
	return resline->start;
}

bool bc::Component::justCreated()
{
	return resline->start == resline->m_end && resline->start == factory->curbright;
}


bool bc::Component::isContain(poidex index)
{
	return factory->getComp(index) == this;
}


bool bc::Component::add(const poidex index, const point p, const Barscalar& col, bool forsed)
{
	assert(lived);

	if (!forsed)
	{
		if (!canBeConnected(p, true))
			return false;

		if (cashedSize == factory->settings.colorRange)
		{
			return false;
		}
	}

#ifndef POINTS_ARE_AVAILABLE
	assert(getMaxparent() == this);
	++getMaxparent()->totalCount;
#endif // !POINTS_ARE_AVAILABLE


	factory->setInclude(index, this);

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, col);
		avgSr += col;
	}
	// 3d barcode/ —читаем кол-во добавленных значений
	if (factory->settings.returnType == ReturnType::barcode3d)
	{
		if (col != lastVal)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); //всего
		}
	}
	else if (col != lastVal)
	{
		if (factory->settings.returnType == ReturnType::barcode3dold)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); // сколкьо было доабвлено
		}
		cashedSize = 0;
	}
	++cashedSize;
	lastVal = col;

	return true;
}

void bc::Component::kill()
{
	kill(lastVal);
}

void bc::Component::kill(const Barscalar& endScalar)
{
	if (!lived)
		return;
	lived = false;

	resline->m_end = endScalar;

	//if (col < resline->start)
	//	resline->start = col;
	//if (col > resline->m_end)
	//	resline->m_end = col;

//	assert(resline->len() != 0);

	if (factory->settings.returnType == ReturnType::barcode3dold)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}
	else if (factory->settings.returnType == ReturnType::barcode3d)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}

	if (parent == nullptr && factory->settings.createBinaryMasks)
	{
		for (barvalue& a : resline->matr)
		{
			a.value = resline->m_end - a.value;
		}
	}

	lastVal = endScalar;
	cashedSize = 0;
}


void bc::Component::setParent(bc::Component* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;

#ifndef POINTS_ARE_AVAILABLE
	this->parent->totalCount += totalCount;
	parnt->startIndex = MIN(parnt->startIndex, startIndex);
	parnt->sums += this->sums;
#endif // ! POINTS_ARE_AVAILABLE

	// at moment when this must be dead
	assert(lived);

	const Barscalar& endScalar = lastVal;
	if (factory->settings.createBinaryMasks)
	{
		parnt->resline->matr.reserve(parnt->resline->matr.size() + resline->matr.size() + 1);

		for (barvalue& val : resline->matr)
		{
			// Записываем длину сущщетвования точки
			val.value = endScalar > val.value ? endScalar - val.value : val.value - endScalar;
			//val.value = col - val.value;

			avgSr += val.value;
			// Эти точки сичтаются как только что присоединившиеся
			parnt->resline->addCoord(barvalue(val.getPoint(), endScalar));
		}
	}

	kill(endScalar);

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}


bool bc::Component::canBeConnected(const bc::point& p, bool incrSum)
{
	return true;

	//	if (factory->settings.maxRadius < (lastVal.val_distance(factory->curbright)))
	//		return false;
	//
	////	if (!factory->settings.maxLen.isCached)
	////		return true;
	//	if (totalCount == 0)
	//		return true;

	return true;
}

bc::Component::~Component()
{
	//	factory->components[index] = nullptr;
}

