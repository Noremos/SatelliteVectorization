#ifndef BARCODE_H
#define BARCODE_H

#include "presets.h"

#include <vector>
#include <cassert>
#include <cstring>
#include <unordered_map>
#include "include_cv.h"
#include "barscalar.h"

#define BARVALUE_RAM_OPTIMIZATION

namespace bc
{
	typedef uint poidex;


	struct barRGB : public Barscalar
	{
		uchar getR() { return this->data.b3[0]; }
		uchar getG() { return this->data.b3[1]; }
		uchar getB() { return this->data.b3[2]; }
	};

	struct barBGR : public Barscalar
	{
		uchar getB() { return this->data.b3[0]; }
		uchar getG() { return this->data.b3[1]; }
		uchar getR() { return this->data.b3[2]; }
	};

	
	struct EXPORT CachedValue
	{
		Barscalar val;
		bool isCached = false;

		void set(Barscalar val)
		{
			this->val = val;
			this->isCached = true;
		}

		Barscalar getOrDefault(Barscalar defValue) const
		{
			return isCached ? val : defValue;
		}
	};

	struct BarRect
	{
		int x, y, width, height;
		BarRect(int _x, int _y, int _width, int _height)
		{
			x = _x;
			y = _y;
			width = _width;
			height = _height;
		}

		float coof()
		{
			return width > height ? static_cast<float>(width) / height : static_cast<float>(height) / width;
		}

		int right()
		{
			return x + width;
		}
		int botton()
		{
			return y + height;
		}

		int area()
		{
			return width * height;
		}

		bool isItemInside(BarRect anItem)
		{
			return (anItem.x >= x && anItem.y >= y && anItem.right() <= right() && anItem.botton() <= botton());
		}
	};

	//	static bool operator > (BarVec3b c1, BarVec3b c2)
	//	{
	//		return c1.sum() > c2.sum();
	//	}
	//	static bool operator < (BarVec3b c1, BarVec3b c2)
	//	{
	//		return c1.sum() < c2.sum();
	//	}

	struct barstruct
	{
		ComponentType comtype;
		ProcType proctype;
		ColorType coltype;
		barstruct()
		{
			comtype = ComponentType::Component;
			proctype = ProcType::f0t255;
			coltype = ColorType::gray;
		}
		barstruct(ProcType pt, ColorType colT, ComponentType comT)
		{
			this->comtype = comT;
			this->proctype = pt;
			this->coltype = colT;
		}
	};


	
	struct BarConstructor
	{
		CachedValue stepPorog;
		CachedValue maxLen;
		float maxRadius = 999999;
	public:
		//Barscalar foneStart;
		//Barscalar foneEnd;
		std::vector<barstruct> structure;
		ReturnType returnType = ReturnType::barcode2d;
		BarType type;

		bool createGraph = false;
		bool createBinaryMasks = false;
		AttachMode attachMode = AttachMode::firstEatSecond;
		//ProcessStrategy processMode = ProcessStrategy::brightness;
		bool killOnMaxLen = false;
		bool extracheckOnPixelConnect = false;
		int colorRange = INT32_MAX;
#ifdef USE_OPENCV
		bool visualize = false;
		int waitK = 1;
#endif // USE_OPENCV

		inline void addStructure(ProcType pt, ColorType colT, ComponentType comT)
		{
			structure.push_back(barstruct(pt, colT, comT));
		}

		void checkCorrect() const
		{
			//if (returnType == ReturnType::barcode2d || returnType == ReturnType::barcode3d)
			//	createBinaryMasks = true;

			//if (createGraph && !createBinaryMasks)
			//	throw std::exception();

			if (structure.size() == 0)
				throw std::exception();

			//getStepPorog();
		}

		// разница соединяемых значений должна быть меньше этого значения
		Barscalar getMaxStepPorog() const
		{
			return stepPorog.getOrDefault(0);
		}

		void setStep(Barscalar val)
		{
			stepPorog.set(val);
		}

		Barscalar getMaxLen()
		{
			return maxLen.getOrDefault(0);
		}

		void setMaxLen(Barscalar val)
		{
			maxLen.set(val);
		}
	};

	struct EXPORT point
	{
		int x = -1;
		int y = -1;
		inline point()
		{
			x = -1; y = -1;
		}

		inline point(int x, int y) :x(x), y(y)
		{

		}

		inline void init(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		inline void init(int p[2])
		{
			this->x = p[0];
			this->y = p[1];
		}

#ifdef USE_OPENCV
		inline cv::Point cvPoint() const
		{
			return cv::Point(x, y);
		}
#endif // USE_OPENCV

		poidex getLiner(int wid) const
		{
			assert(x >= 0);
			assert(y >= 0);

			return y * wid + x;
		}

		point operator+(int* xy) const
		{
			return point(x + xy[0], y + xy[1]);
		}
		point operator+(char* xy) const
		{
			return point(x + xy[0], y + xy[1]);
		}
		point operator*(int c) const
		{
			return point(x * c, y * c);
		}
		point operator+(point p) const
		{
			return point(x + p.x, y + p.y);
		}
		int operator[](int i) const
		{
			return i == 0 ? x : y;
		}
		bool operator==(const point& p) const
		{
			return x == p.x && y == p.y;
		}
		bool operator!=(const point& p) const
		{
			return x != p.x || y != p.y;
		}
	};


	class EXPORT pointHash
	{
	public:
		size_t operator()(const point& p) const
		{
			return ((size_t)p.x << 32) + (size_t)p.y;
		}
	};

	typedef std::unordered_map<point, bool, pointHash> pmap;
	typedef std::pair<point, bool> ppair;

	
	struct bar3dvalue
	{
		size_t count;
		Barscalar value;

		bar3dvalue(Barscalar value, size_t count)
		{
			this->count = count;
			this->value = value;
		}
		bar3dvalue()
		{
			this->count = 0;
		}
	};


	
	struct pybarvalue
	{
		Barscalar value;
		int x, y;

		pybarvalue()
		{}

		pybarvalue(int x, int y, Barscalar value)
		{
			this->x = x;
			this->y = y;
			this->value = value;
		}
	};

#ifdef BARVALUE_RAM_OPTIMIZATION
	const uint MAX_WID = 65535;
#define OPTTIF(A, B) (A)
#else
#define OPTTIF((A), (B)) (B)
#endif

	
	struct barvalue
	{
#ifdef BARVALUE_RAM_OPTIMIZATION
		uint index;
#else
		uint x;
		uint y;
#endif
		Barscalar value;

		barvalue(int x, int y, Barscalar value)
		{
			assert(x >= 0);
			assert(y >= 0);
			OPTTIF(index = y * MAX_WID + x, this->x = x; this->y = y);
			this->value = value;
		}

		barvalue(bc::point p, Barscalar value)
		{
			assert(p.x >= 0);
			assert(p.y >= 0);

			OPTTIF(index = p.y * MAX_WID + p.x,
				x = p.x; y = p.y);
			this->value = value;
		}


		barvalue()
		{
			index = 0;
			value.type = BarType::NONE;
		}

		barvalue(const barvalue& other) /*: s(other.s)*/
		{
			OPTTIF(index = this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;
		}

		barvalue(barvalue&& other) /*: s(std::move(o.s))*/
		{
			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;
		}

		// copy assignment
		barvalue& operator=(const barvalue& other)
		{
			// Guard self assignment
			if (this == &other)
				return *this;

			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;

			return *this;
		}

		// move assignment
		barvalue& operator=(barvalue&& other) noexcept
		{
			// Guard self assignment
			if (this == &other)
				return *this; // delete[]/size=0 would also be ok

			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;

			return *this;
		}

		bc::point getPoint() const
		{
			return bc::point(getX(), getY());
		}

		uint getIndex(int widr = MAX_WID) const
		{
			return OPTTIF(index, y * widr + x);
		}

		static uint getStatInd(int x, int y, int widr = MAX_WID)
		{
			return y * widr + x;
		}

		static bc::point getStatPoint(uint index)
		{
			return bc::point(index % MAX_WID, index / MAX_WID);
		}

		int getX() const
		{
			return OPTTIF(index % MAX_WID, x);
		}

		void setX(int x)
		{
			OPTTIF(index = getY() * MAX_WID + x, this->x = x);
		}

		int getY() const
		{
			return OPTTIF(index / MAX_WID, y);
		}

		void setY(int y)
		{
			OPTTIF(index = y * MAX_WID + getX(), this->y = y);
		}

		pybarvalue getPyValue() const
		{
			return pybarvalue(getX(), getY(), value);
		}
	};

	
	using barvector = std::vector<barvalue>;

	
	using barcounter = std::vector<bar3dvalue>;

	//**********************************************
}

#endif // BARCODE_H
