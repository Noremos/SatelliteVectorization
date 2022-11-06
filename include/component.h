#pragma once
#include <vector>
#include "barstrucs.h"


//#define POINTS_ARE_AVAILABLE
namespace bc
{
	
	class BarcodeCreator;

	
	struct barline;

	
	class Component
	{
	public:
#ifdef POINTS_ARE_AVAILABLE
		size_t getTotalSize()
		{
			return resline->matr.size();
		}
#else
		size_t startIndex = 0;
		size_t getTotalSize()
		{
			return totalCount;
		}
	private:
		size_t totalCount = 0/*, ownSize = 0*/;
#endif // !POINTS_ARE_AVAILABLE
	protected:
		BarcodeCreator* factory;
		Component* cachedMaxParent = nullptr;

	public:
		Component* parent = nullptr;
		barline* resline = nullptr;

	protected:
		int cashedSize = 0;
		Barscalar lastVal = 0;
		Barscalar avgSr = 0;
		//point startPoint;
		bool lived = true;

		float sums = 0;

	private:
		//0 - nan

		void init(BarcodeCreator* factory, const Barscalar& val);
	public:


		Component(poidex pix, const Barscalar& val, bc::BarcodeCreator* factory);
		Component(BarcodeCreator* factory, bool create = false);

		int getLastRowSize()
		{
			return cashedSize;
		}

		Barscalar getStart();

		Barscalar getLast()
		{
			return lastVal;
		}

		bool isAlive()
		{
			return lived;
		}

		bool justCreated();

		//Barscalar len()
		//{
		//	//return round(100000 * (end - start)) / 100000;
		//	return  abs(resline->len());
		//	//return end - start;
		//}
		//    cv::Mat binmap;
		Component* getMaxparent()
		{
			if (parent == nullptr)
				return this;

			if (cachedMaxParent == nullptr)
			{
				cachedMaxParent = parent;
			}
			while (cachedMaxParent->parent)
			{
				cachedMaxParent = cachedMaxParent->parent;
				//totalCount += cachedMaxParent->coords->size();
			}
			return cachedMaxParent;
		}


		bool isContain(poidex index);
		virtual bool add(const poidex index, const point p, const Barscalar& col, bool forsed = false);
		void kill();
		virtual void kill(const Barscalar& endScalar);
		virtual void setParent(Component* parnt);


		bool canBeConnected(const bc::point& p, bool incrSum = false);

		virtual ~Component();

	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
