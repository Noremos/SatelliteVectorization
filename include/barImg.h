#pragma once
//#define USE_OPENCV
#include "presets.h"

#include <memory>
#include <iterator>
#include <cassert>
#include <cstring>

#include "include_cv.h"
#include "include_py.h"
#include "barstrucs.h"



namespace bc {


	/*struct BarVec3f
	{
	public:
		float r, g, b;
	};

	struct BarVec3i
	{
	public:
		int r, g, b;
	};*/


	class EXPORT DatagridProvider
	{
	public:
		virtual int wid() const = 0;
		virtual int hei() const = 0;
		virtual int channels() const = 0;

		virtual void maxAndMin(Barscalar& min, Barscalar& max) const = 0;
		virtual size_t typeSize() const = 0;


		virtual Barscalar get(int x, int y) const = 0;

		virtual Barscalar get(bc::point p) const
		{
			return get(p.x, p.y);
		}

		virtual size_t length() const
		{
			return static_cast<size_t>(wid()) * hei();
		}

		// wid * y + x;
		virtual Barscalar getLiner(size_t pos) const
		{
			return get((int)(pos % wid()), (int)(pos / wid()));
		}

		inline BarType getType() const
		{
			return type;
		}

		point getPointAt(size_t iter) const
		{
			return point((int)(iter % wid()), (int)(iter / wid()));
		}
		virtual ~DatagridProvider()
		{ }

	public:
		BarType type = BarType::BYTE8_1;
	};



	class EXPORT BarImg : public DatagridProvider
	{
		typedef Barscalar* bar_iterator;
		typedef const Barscalar* const_bar_iterator;
	public:
		Barscalar* values = nullptr;
	protected:

	private:
		mutable CachedValue cachedMax;
		mutable CachedValue cachedMin;

		int _wid;
		int _hei;
		int _channels;
		int TSize;

		bool _deleteData = true;
	public:
		bool diagReverce = false;
		virtual int width() { return wid(); }
		virtual int height() { return hei(); }

	protected:

		void setMetadata(int width, int height, int chnls)
		{
			this->_wid = width;
			this->_hei = height;
			this->_channels = chnls;
			if (chnls == 3)
				type = BarType::BYTE8_3;
			else
				type = BarType::BYTE8_1;
			TSize = sizeof(Barscalar);
			diagReverce = false;
		}

		void valZerofy()
		{
			memset(values, 0, this->length() * TSize);
		}

		//same pointer
		Barscalar* valGetShadowsCopy() const
		{
			return values;
		}

		void valInit()
		{
			valDelete();
			values = new Barscalar[this->length()];
			_deleteData = true;
			valZerofy();
		}
		//copy
		Barscalar* valGetDeepCopy() const
		{
			Barscalar* newVals = new Barscalar[this->length()];
			memcpy(newVals, values, this->length() * TSize);
			return newVals;
		}

		void valAssignCopyOf(Barscalar* newData)
		{
			valDelete();
			values = new Barscalar[this->length()];
			_deleteData = true;
			memcpy(values, newData, this->length() * TSize);
		}

		void valDelete()
		{
			if (values != nullptr && _deleteData)
				delete[] values;

			cachedMax.isCached = false;
			cachedMin.isCached = false;
		}

		void valAssignInstanceOf(Barscalar* newData, bool deleteData = true)
		{
			valDelete();
			_deleteData = deleteData;
			values = newData;
		}

		//std::unique_ptr<Barscalar*> data;



	public:
		BarImg(int width = 1, int height = 1, int chnls = 1)
		{
			setMetadata(width, height, chnls);
			valInit();
		}


		BarImg(int width, int height, int chnls, uchar* _data, bool copy = true, bool deleteData = true)
		{
			if (copy)
			{
				copyFromRawData(width, height, chnls, _data);
			}
			else
				assignRawData(width, height, chnls, _data, deleteData);
		}

		BarImg* getCopy()
		{
			BarImg* nimg = new BarImg(*this, true);
			return nimg;
		}

		void assign(const BarImg& copy)
		{
			assignCopyOf(copy);
		}

		BarImg(const BarImg& copyImg)
		{
			assignCopyOf(copyImg);
		}

		BarImg(const BarImg& copyImg, bool copy)
		{
			if (copy)
				assignCopyOf(copyImg);
			else
				assignInstanceOf(copyImg);
		}

		//Перегрузка оператора присваивания
		BarImg& operator= (const BarImg& drob)
		{
			if (&drob != this)
				assignCopyOf(drob);

			return *this;
		}

		//Перегрузка оператора присваивания
		BarImg& operator= (BarImg&& drob)
		{
			if (&drob != this)
				assignCopyOf(drob);

			return *this;
		}

		//#ifdef USE_OPENCV
		//		BarImg(cv::Mat img, bool copy = true)
		//		{
		//			initFromMat(img, copy);
		//		}
		//
		//		void initFromMat(cv::Mat img, bool copy = true)
		//		{
		//			if (copy)
		//				copyFromRawData(img.cols, img.rows, img.channels(), img.data);
		//			else
		//				assignRawData(img.cols, img.rows, img.channels(), img.data, false);
		//			diagReverce = false;
		//		}
		//
		//		cv::Mat getCvMat()
		//		{
		//			bool re = diagReverce;
		//			diagReverce = false;
		//			cv::Mat m = cv::Mat::zeros(_hei, _wid, CV_8UC1);
		//			for (size_t i = 0; i < this->length(); i++)
		//			{
		//				auto p = getPointAt(i);
		//				m.at(p.y, p.x) = get(p.x, p.y);
		//			}
		//			diagReverce = re;
		//
		//			return m;
		//		}
		//#endif // OPENCV

		virtual ~BarImg()
		{
			valDelete();
		}

		Barscalar* getData() const
		{
			return values;
		}

		void maxAndMin(Barscalar& _min, Barscalar& _max) const override
		{
			_max = values[0];
			_min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar val = values[i];
				if (val > _max)
					_max = val;
				if (val < _min)
					_min = val;
			}
			cachedMax.set(_max);
			cachedMin.set(_min);
		}

		Barscalar max() const
		{
			if (cachedMax.isCached)
				return cachedMax.val;

			Barscalar _max = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar val = values[i];
				if (val > _max)
					_max = val;
			}

			cachedMax.set(_max);
			return _max;
		}

		Barscalar min() const
		{
			if (cachedMin.isCached)
				return cachedMin.val;

			Barscalar _min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];
			}

			cachedMin.set(_min);
			return _min;
		}

		Barscalar MaxMinMin() const
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				return cachedMax.val - cachedMin.val;
			}

			Barscalar _min = values[0];
			Barscalar _max = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];

				if (values[i] > _max)
					_max = values[i];
			}
			cachedMin.set(_min);
			cachedMax.set(_max);

			return _max - _min;
		}

		void setDataU8(int width, int height, uchar* valuesData)
		{
			setMetadata(width, height, 1);
			Barscalar* raw = new Barscalar[width * height];

			int off = 0;
			for (size_t i = 0; i < width * height; ++i)
			{
				raw[i].type = BarType::BYTE8_1;
				raw[i].data.b1 = valuesData[i];
			}

			valAssignInstanceOf(raw);
		}

		void copyFromRawData(int width, int height, int chnls, uchar* rawData)
		{
			setMetadata(width, height, chnls);
			valAssignCopyOf(reinterpret_cast<Barscalar*>(rawData));
		}

		void assignRawData(int width, int height, int chnls, uchar* rawData, bool deleteData = true)
		{
			setMetadata(width, height, chnls);

			valAssignInstanceOf(reinterpret_cast<Barscalar*>(rawData), deleteData);
		}

		inline int wid() const override
		{
			return _wid;
		}

		inline int hei() const override
		{
			return _hei;
		}
		inline int channels() const override
		{
			return _channels;
		}

		inline size_t typeSize() const override
		{
			return TSize;
		}

		inline Barscalar get(int x, int y) const override
		{
			//if (diagReverce)
			//	return values[x * _wid + y];
			//else
			return values[y * _wid + x];
		}

		inline void set(int x, int y, Barscalar val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] = val;
			//else
			values[y * _wid + x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void set(bc::point p, Barscalar val)
		{
			//if (diagReverce)
			//	values[p.x * _wid + p.y] = val;
			//else
			values[p.y * _wid + p.x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(int x, int y, Barscalar val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] += val;
			//else
			values[y * _wid + x] += val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void minus(bc::point p, Barscalar val)
		{
			minus(p.x, p.y, val);
		}

		inline void minus(int x, int y, Barscalar val)
		{
			//assert(values[y * _wid + x] >= val);
			//if (diagReverce)
			//	values[x * _wid + y] -= val;
			//else
			values[y * _wid + x] -= val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(bc::point p, const Barscalar& val)
		{
			add(p.x, p.y, val);
		}


		inline void setLiner(size_t pos, const Barscalar& val)
		{
			values[pos] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		virtual Barscalar getLiner(size_t pos) const override
		{
			return values[pos];
		}

		void minusFrom(Barscalar min)
		{
			for (size_t i = 0; i < this->length(); i++)
			{
				values[i] = min - values[i];
			}
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		//void addToMat(Barscalar value)
		//{
		//	for (size_t i = 0; i < this->length(); i++)
		//	{
		//		Barscalar sval = this->getLiner(i);
		//		sval += value;
		//		assert(this->getLiner(i) == sval);
		//	}

		//	cachedMin.isCached = false;
		//	cachedMax.isCached = false;
		//}

		void resize(int nwid, int nhei)
		{
			if (nwid == _wid && nhei == _hei)
				return;

			_wid = nwid;
			_hei = nhei;
			valAssignInstanceOf(new Barscalar[this->length()], true);
		}


		//Overload + operator to add two Box objects.
		BarImg& operator+(const Barscalar& v);

		//// Overload + operator to add two Box objects.
		BarImg& operator-(const Barscalar& v);

		void assignCopyOf(const BarImg& copy)
		{
			setMetadata(copy._wid, copy._hei, copy._channels);
			valAssignCopyOf(copy.values);

			this->diagReverce = copy.diagReverce;
			this->type = copy.type;
		}

		void assignInstanceOf(const BarImg& inst)
		{
			setMetadata(inst._wid, inst._hei, inst._channels);
			valAssignInstanceOf(inst.values);

			this->type = inst.type;
			this->diagReverce = inst.diagReverce;
		}

		bar_iterator begin() { return &values[0]; }
		const_bar_iterator begin() const { return &values[0]; }
		bar_iterator end() { return &values[this->length()]; }
		const_bar_iterator end() const { return &values[this->length()]; }
	};



#ifdef USE_OPENCV
	class BarMat : public DatagridProvider
	{
		mutable CachedValue cachedMax;
		mutable CachedValue cachedMin;
	public:
		Mat& mat;
		BarMat(Mat& _mat, BarType type = BarType::NONE) : mat(_mat)
		{
			if (type != BarType::NONE)
			{
				this->type = type;
			}
			else
			{
				switch (mat.type())
				{
				case CV_8UC1:
					this->type = BarType::BYTE8_1;
					break;

				case CV_8UC3:
					this->type = BarType::BYTE8_3;
					break;

					//case CV_32FC1:
					//	this->type = BarType::FLOAT;
					//	break;
				default:
					assert(false);
				}
			}
		}

		int wid() const
		{
			return mat.cols;
		}

		int hei() const
		{
			return mat.rows;
		}

		int channels() const
		{
			return mat.channels();
		}

		Barscalar get(int x, int y) const;

		//Перегрузка оператора присваивания
		BarMat& operator= (const BarMat& drob)
		{
			mat = drob.mat;
			type = drob.type;
			cachedMax = drob.cachedMax;
			cachedMin = drob.cachedMin;
			return *this;
		}


		void maxAndMin(Barscalar& min, Barscalar& max) const override
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				min = cachedMin.val;
				max = cachedMax.val;
			}

			double amin, amax;
			cv::minMaxLoc(mat, &amin, &amax);
			min = static_cast<float>(amin);
			max = static_cast<float>(amax);
		}

		Barscalar max() const
		{
			if (cachedMax.isCached)
			{
				return cachedMax.val;
			}
			double min, max;
			cv::minMaxLoc(mat, &min, &max);

			cachedMax.set(static_cast<uchar>(max));
			cachedMin.set(static_cast<uchar>(min));
			return max;
		}

		inline size_t typeSize() const
		{
			return mat.depth();
		}
	};
#endif // USE_OPENCV 



#ifdef _PYD
	class BarNdarray : public DatagridProvider
	{
	public:
		Py_intptr_t const* strides;

		bn::ndarray& mat;

		BarNdarray(bn::ndarray& _mat) : mat(_mat)
		{
			strides = _mat.get_strides();
			type = mat.get_nd() == 1 ? BarType::BYTE8_1 : BarType::BYTE8_3;
		}
		int wid() const
		{
			return mat.shape(1);
		}

		int hei() const
		{
			return mat.shape(0);
		}

		int channels() const
		{
			return mat.get_nd() <= 2 ? 1 : mat.shape(2);
		}

		Barscalar get(int x, int y) const
		{
			if (type == BarType::BYTE8_1)
			{
				return Barscalar(*(mat.get_data() + y * strides[0] + x * strides[1]));
			}
			else
			{
				char* off = mat.get_data() + y * strides[0] + x * strides[1];
				return Barscalar(off[0], off[1], off[2]);
			}
		}

		Barscalar max() const
		{
			if (this->length() == 0)
				return 0;

			Barscalar max = this->getLiner(0);
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar t = this->getLiner(i);
				if (t > max)
					max = t;
			}
			/*printf("max for nd: %i" + mat.attr("max"));
			return mat.attr("max"));*/
			return max;
		}

		void maxAndMin(Barscalar& min, Barscalar& max) const override
		{
			if (this->length() == 0)
				return;

			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar t = this->getLiner(i);
				if (t > max)
					max = t;
				if (t < min)
					min = t;
			}
		}

		size_t typeSize() const
		{
			return mat.get_dtype().get_itemsize();
		}
	};

#endif // USE_OPENCV 


	static inline void split(const DatagridProvider& src, std::vector<BarImg*>& bgr)
	{
		size_t step = static_cast<size_t>(src.channels()) * src.typeSize();
		for (int k = 0; k < src.channels(); k++)
		{
			BarImg* ib = new BarImg(src.wid(), src.hei());
			bgr.push_back(ib);

			for (size_t i = 0; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
			{
				ib->setLiner(i, src.getLiner(i));
			}
		}
	}

	//template<class Barscalar, class U>
	//static inline void split(const DatagridProvider<BarVec3b>& src, std::vector<DatagridProvider<U>*>& bgr)
	//{
	//}

	enum class BarConvert
	{
		BGR2GRAY,
		GRAY2BGR,
	};


	static inline void cvtColorU1C2V3B(const bc::DatagridProvider& source, bc::BarImg& dest)
	{
		assert(source.channels() == 1);

		dest.resize(source.wid(), source.hei());

		for (size_t i = 0; i < source.length(); ++i)
		{
			Barscalar u = source.getLiner(i);
			u.data.b3[0] = u.data.b1;
			u.data.b3[1] = u.data.b1;
			u.data.b3[2] = u.data.b1;
			u.type = BarType::BYTE8_3;
			dest.setLiner(i, u);
		}
	}

	static inline void cvtColorV3B2U1C(const bc::DatagridProvider& source, bc::BarImg& dest)
	{
		assert(dest.channels() == 1);
		dest.resize(source.wid(), source.hei());

		for (size_t i = 0; i < source.length(); ++i)
		{
			float accum = source.getLiner(i).getAvgFloat();
			dest.setLiner(i, accum);
		}
	}
	//template<class Barscalar, class U>
	//static inline void cvtColor(const bc::DatagridProvider& source, bc::DatagridProvider<U>& dest)
	//{

	//}

	//// note: this function is not a member function!

	//BarImg operator+(const Barscalar& c1, BarImg& c2);
	//BarImg operator-(const Barscalar& c1, const BarImg& c2);

#ifdef USE_OPENCV

	static cv::Mat convertProvider2Mat(DatagridProvider* img)
	{
		cv::Mat m = cv::Mat::zeros(img->hei(), img->wid(), CV_8UC1);
		for (size_t i = 0; i < img->length(); i++)
		{
			auto p = img->getPointAt(i);
			m.at<uchar>(p.y, p.x) = img->get(p.x, p.y).data.b1;
		}
		return m;
	}


	static cv::Mat convertRGBProvider2Mat(const DatagridProvider* img)
	{
		cv::Mat m = cv::Mat::zeros(img->hei(), img->wid(), CV_8UC3);
		for (size_t i = 0; i < img->length(); i++)
		{
			auto p = img->getPointAt(i);
			m.at<cv::Vec3b>(p.y, p.x) = img->get(p.x, p.y).toCvVec();
		}
		return m;
	}

#endif // USE_OPENCV
}
//split
//convert
