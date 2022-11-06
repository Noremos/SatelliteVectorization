#ifndef MATRIMG_H
#define MATRIMG_H
#include "barImg.h"


class MatrImg : public bc::DatagridProvider
{
public:
	void fill(const Barscalar &s)
	{
		for (size_t i = 0, total = length(); i < total; ++i)
		{
			setLiner(i, s);
		}
	}
	typedef Barscalar *bar_iterator;
	typedef const Barscalar *const_bar_iterator;

public:
	uchar *data = nullptr;

protected:
private:
	mutable bc::CachedValue cachedMax;
	mutable bc::CachedValue cachedMin;

	int _wid;
	int _hei;
	int _channels;
	int TSize;

	bool _deleteData = false;

public:
	bool diagReverce = false;
	virtual int width() const { return _wid; }
	virtual int height() const { return _hei; }
	inline int wid() const override  { return _wid; }
	inline int hei() const override  { return _hei; }

protected:
	void setMetadata(int width, int height, int chnls)
	{
		this->_wid = width;
		this->_hei = height;
		this->_channels = chnls;
		if (chnls == 3)
		{
			type = BarType::BYTE8_3;
			TSize = 3;
		}
		else
		{
			type = BarType::BYTE8_1;
			TSize = 1;
		}
		diagReverce = false;
	}

	void valZerofy()
	{
		memset(data, 0, this->length() * TSize);
	}

	void valInit()
	{
		valDelete();
		data = new uchar[this->length() * TSize];
		_deleteData = true;
		valZerofy();
	}
	//copy
	Barscalar *valGetDeepCopy() const
	{
		Barscalar *newVals = new Barscalar[this->length()];
		memcpy(newVals, data, this->length() * TSize);
		return newVals;
	}

	void valAssignCopyOf(uchar *newData)
	{
		valDelete();
		data = new uchar[this->length() * TSize];
		_deleteData = true;
		memcpy(data, newData, this->length() * TSize);
	}

	void valDelete()
	{
		if (data != nullptr && _deleteData)
		{
			delete[] data;
			data = nullptr;
		}
		cachedMax.isCached = false;
		cachedMin.isCached = false;
	}

	void valAssignInstanceOf(uchar *newData, bool deleteData = true)
	{
		valDelete();
		_deleteData = deleteData;
		data = newData;
	}

	//std::unique_ptr<Barscalar*> data;



public:
	MatrImg(int width = 1, int height = 1, int chnls = 1)
	{
		setMetadata(width, height, chnls);
		valInit();
	}

	MatrImg(int width, int height, int chnls, uchar *_data, bool copy = true, bool deleteData = true)
	{
		if (copy)
		{
			copyFromRawData(width, height, chnls, _data);
		}
		else
			assignRawData(width, height, chnls, _data, deleteData);
	}

	void assign(const MatrImg &copy) { assignCopyOf(copy); }

	// move
	MatrImg(MatrImg &&moveImg)
	{
		if (&moveImg != this)
		{
			assignInstanceOf(moveImg);
			moveImg._deleteData = false;
			moveImg.data = nullptr;
			moveImg.type = BarType::NONE;
		}
	}

	MatrImg &operator=(MatrImg &&moveImg)
	{
		if (&moveImg != this)
		{
			assignInstanceOf(moveImg);
			moveImg.data = nullptr;
			moveImg._deleteData = false;
			moveImg.type = BarType::NONE;
		}

		return *this;
	}

// copy


	MatrImg(const MatrImg &copyImg)
	{
		assignCopyOf(copyImg);
	}


	MatrImg(const MatrImg &copyImg, bool copy)
	{
		if (copy)
			assignCopyOf(copyImg);
		else
			assignInstanceOf(copyImg);
	}

	//Перегрузка оператора присваивания
	MatrImg &operator=(const MatrImg &drob)
	{
		if (&drob != this)
			assignCopyOf(drob);

		return *this;
	}


	virtual ~MatrImg() { valDelete(); }

	uchar *getData() const { return data; }

	void maxAndMin(Barscalar &_min, Barscalar &_max) const override
	{
		_max = getLiner(0);
		_min = getLiner(0);
		for (size_t i = 1; i < this->length(); i++)
		{
			Barscalar val = getLiner(i);
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

		Barscalar _max = getLiner(0);
		for (size_t i = 1; i < this->length(); i++)
		{
			Barscalar val = getLiner(i);
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

		Barscalar _min = getLiner(0);
		for (size_t i = 1; i < this->length(); i++)
		{
			if (getLiner(i) < _min)
				_min = getLiner(i);
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

		Barscalar _min = getLiner(0);
		Barscalar _max = getLiner(0);
		for (size_t i = 1; i < this->length(); i++)
		{
			const Barscalar v = getLiner(i);
			if (v < _min)
				_min = v;

			if (getLiner(i) > _max)
				_max = v;
		}
		cachedMin.set(_min);
		cachedMax.set(_max);

		return _max - _min;
	}

	void copyFromRawData(int width, int height, int chnls, uchar *rawData)
	{
		setMetadata(width, height, chnls);
		valAssignCopyOf(rawData);
	}

	void assignRawData(int width, int height, int chnls, uchar *rawData, bool deleteData = true)
	{
		setMetadata(width, height, chnls);

		valAssignInstanceOf(rawData, deleteData);
	}

	inline int channels() const override { return _channels; }

	inline size_t typeSize() const override { return TSize; }


	inline Barscalar get(int x, int y) const override
	{
		if (type == BarType::BYTE8_1)
		{
			return Barscalar(data[(y * _wid + x) * TSize]);
		}
		else
		{
			uchar *off = data + (y * _wid + x) * TSize;
			return Barscalar(off[0], off[1], off[2]);
		}
	}

	inline uchar* lineOffet(int y) const
	{
		if (type == BarType::BYTE8_1)
		{
			return data + (y * _wid) * TSize;
		}
		else
		{
			return data + (y * _wid) * TSize;
		}
	}

	inline int bytesInLine() const
	{
		return _wid * TSize;
	}

	inline void set(int x, int y, const Barscalar& val)
	{
		//if (diagReverce)
		//	values[x * _wid + y] = val;
		//else
		if (type == BarType::BYTE8_1)
		{
			data[(y * _wid + x) * TSize] = val.data.b1;
		}
		else
		{
			uchar *off = data + (y * _wid + x) * TSize;
			if (val.type == BarType::BYTE8_3)
			{
				off[0] = val.data.b3[0];
				off[1] = val.data.b3[1];
				off[2] = val.data.b3[2];
			}
			else
			{
				off[0] = val.data.b1;
				off[1] = val.data.b1;
				off[2] = val.data.b1;
			}
		}
		cachedMin.isCached = false;
		cachedMax.isCached = false;
	}


	inline void add(int x, int y, const Barscalar& val)
	{
		//if (diagReverce)
		//	values[x * _wid + y] += val;
		//else
		if (type == BarType::BYTE8_1)
		{
			data[(y * _wid + x) * TSize] += val.data.b1;
		}
		else
		{
			uchar *off = data + (y * _wid + x) * TSize;
			if (val.type == BarType::BYTE8_3)
			{
				off[0] += val.data.b3[0];
				off[1] += val.data.b3[1];
				off[2] += val.data.b3[2];
			}
			else
			{
				off[0] += val.data.b1;
				off[1] += val.data.b1;
				off[2] += val.data.b1;
			}
		}

		cachedMin.isCached = false;
		cachedMax.isCached = false;
	}


	inline void minus(int x, int y, const Barscalar& val)
	{
		//assert(values[y * _wid + x] >= val);
		//if (diagReverce)
		//	values[x * _wid + y] -= val;
		//else
		if (type == BarType::BYTE8_1)
		{
			assert(data[(y * _wid + x) * TSize] >= val.data.b1);
			data[(y * _wid + x) * TSize] -= val.data.b1;
		}
		else
		{
			uchar *off = data + (y * _wid + x) * TSize;
			if (val.type == BarType::BYTE8_3)
			{
				off[0] -= val.data.b3[0];
				off[1] -= val.data.b3[1];
				off[2] -= val.data.b3[2];
			}
			else
			{
				off[0] -= val.data.b1;
				off[1] -= val.data.b1;
				off[2] -= val.data.b1;
			}
		}

		cachedMin.isCached = false;
		cachedMax.isCached = false;
	}


	inline void setLiner(size_t pos, const Barscalar& val)
	{
		if (type == BarType::BYTE8_1)
		{
			data[pos * TSize] = val.data.b1;
		}
		else
		{
			uchar *off = data + pos * TSize;
			if (val.type == BarType::BYTE8_3)
			{
				off[0] = val.data.b3[0];
				off[1] = val.data.b3[1];
				off[2] = val.data.b3[2];
			}
			else
			{
				off[0] = val.data.b1;
				off[1] = val.data.b1;
				off[2] = val.data.b1;
			}
		}
		cachedMin.isCached = false;
		cachedMax.isCached = false;
	}

	virtual Barscalar getLiner(size_t pos) const override
	{
		if (type == BarType::BYTE8_1)
		{
			return Barscalar(data[pos * TSize]);
		}
		else
		{
			uchar *off = data + pos * TSize;
			return Barscalar(off[0], off[1], off[2]);
		}
	}

	void assignCopyOf(const MatrImg& copy)
	{
		setMetadata(copy._wid, copy._hei, copy._channels);
		valAssignCopyOf(copy.data);

		this->diagReverce = copy.diagReverce;
		this->type = copy.type;
	}

	void assignInstanceOf(const MatrImg& inst)
	{
		setMetadata(inst._wid, inst._hei, inst._channels);
		valAssignInstanceOf(inst.data);

		this->type = inst.type;
		this->diagReverce = inst.diagReverce;
	}

};
#endif // MATRIMG_H
