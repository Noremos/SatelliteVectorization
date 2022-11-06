#pragma once
#include <vector>
#include <string>
#include "presets.h"
#include "barline.h"
#include <iostream>

namespace bc
{
	
	class EXPORT Barbase
	{
	public:
		virtual void removePorog(Barscalar const porog) = 0;
		virtual void preprocessBar(Barscalar const& porog, bool normalize) = 0;
		virtual float compireFull(const Barbase* Y, bc::CompireStrategy strat) const = 0;
		virtual Barbase* clone() const = 0;
		virtual Barscalar sum() const = 0;
		virtual void relen() = 0;
		//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
		virtual ~Barbase();
	};

	
	class EXPORT Baritem : public Barbase
	{
	public:
		barlinevector barlines;

	private:
		bc::BarRoot* rootNode = nullptr;
		int wid;
		BarType type;
	public:
		Baritem(int wid = 0, BarType type = BarType::NONE);

		//copy constr
		Baritem(Baritem const& obj)
		{
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;
			this->type = obj.type;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone());
			}
		}

		// copy
		void operator=(Baritem const& obj)
		{
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;
			this->type = obj.type;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone());
			}
		}

		// move constr
		Baritem(Baritem&& obj) noexcept
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;

			this->barlines = obj.barlines;
			obj.barlines.clear();
		}

		// move assign
		void operator=(Baritem&& obj)
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;
			this->type = obj.type;

			this->barlines = obj.barlines;
			obj.barlines.clear();
		}

		//    cv::Mat binmap;
		void add(Barscalar st, Barscalar len);
		void add(barline* line);

		Barscalar sum() const;
		void relen();
		Barscalar maxLen() const;
		Baritem* clone() const;
		inline BarType getType()
		{
			return type;
		}
		void getBettyNumbers(int* bs);

		// remove lines than less then passed value
		void removePorog(Barscalar const porog);
		void preprocessBar(Barscalar const& porog, bool normalize);
		float compireFull(const Barbase* bc, bc::CompireStrategy strat) const;
		float compireBestRes(Baritem const* bc, bc::CompireStrategy strat) const;
		float compareOccurrence(Baritem const* bc, bc::CompireStrategy strat) const;
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);

		void normalize();
		void getJsonObejct(std::string &out);
		void getJsonLinesArray(std::string &out);
		~Baritem();

		bc::BarRoot* getRootNode()
		{
			return rootNode;
		}
		void setRootNode(bc::BarRoot* root)
		{
			rootNode = root;
		}

		Barscalar getMax()
		{
			Barscalar _max{ 0 };
			for (auto* b : this->barlines)
			{
				if (b->start + b->len() > _max)
					_max = b->start + b->len();
			}
			return _max;
		}


		class EXPORT BarscalHash
		{
		public:
			size_t operator()(const Barscalar& p) const
			{
				return ((size_t)0 << 32) + ((size_t)p.data.b3[2] << 16) +
						((size_t)p.data.b3[1] << 8) + ((size_t)p.data.b3[0]);
			}
		};


		typedef std::unordered_map<Barscalar, int, BarscalHash> maphist;


		class Barlfd
		{
		public:
			barline* line = NULL;
			float acc = 0;
			maphist hist;

		/*	void caclHist()
			{
				hist.clear();
				for (size_t i = 0; i < line->matr.size(); ++i)
				{
					Barscalar& scl = line->matr.at(i).value;
					auto asd = hist.find(scl);
					if (asd != hist.end())
					{
						++asd->second;
					}
					else
						hist.insert<pair< Barscalar, int>>(pair<Barscalar, int>(scl, 1));
				}
			}

			void appendHist(maphist& outHist)
			{
				for (auto& val : hist)
				{
					auto asd = outHist.find(val.second);
					if (asd != outHist.end())
					{
						asd->second += val.second;
					}
					else
						outHist.insert<pair< Barscalar, int>>(val);
				}
			}*/

			void calculateEntropy(maphist& hist)
			{
				float s = 0;
				size_t total = line->matr.size();
				for (size_t i = 0; i < total; i++)
				{
					Barscalar& scl = line->matr.at(i).value;
					auto asd = hist.find(scl);
					float lacc = asd->second / total;
					s += lacc * log(lacc);
				}

				acc = s;
			}

			void calculateMask(DatagridProvider& mask)
			{
				float s = 0;
				for (size_t w = 0; w < line->matr.size(); ++w)
				{
					if (mask.get(line->matr[w].getPoint()) > 128)
					{
						s += 1;
					}
				}

				acc =  s / mask.length();
			}
		};

		typedef std::vector<Barlfd> barsplitvec;

		float calcEntrpyByValues(const barsplitvec::iterator begin, const barsplitvec::iterator end)
		{
			maphist hist;

			for(auto it = begin; it != end; it++)
			{
				//it->appendHist(hist);
			}

			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				it->calculateEntropy(hist);
				s += it->acc * log(it->acc);
			}
			
			return -s;
		}

		float calcEntrpyByMask(const barsplitvec::iterator begin, const barsplitvec::iterator end, DatagridProvider& mask)
		{
			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				it->calculateMask(mask);
				s += it->acc * log(it->acc);
			}

			return -s;
		}

		float calcEntropySimple(const barsplitvec::const_iterator begin, const barsplitvec::const_iterator end)
		{
			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				s += it->acc * log(it->acc);
			}

			return -s;
		}
		// left - low entorpy; ritht - big
		void splitRes(barsplitvec& input, barsplitvec& left, barsplitvec& right)
		{
			int bestI = 0; //  a = { <= I}; b = {>I}
			const barsplitvec::iterator begin = input.begin();
			const barsplitvec::iterator end = input.end();
			barsplitvec::iterator spkit = begin + 1;
			
			for (auto it = begin; it != end; ++it)
			{
				//it->caclHist();
			}
			
			if (input.size() == 1)
			{
				left.insert(left.end(), begin, begin + 1);
				return;
			}
			if (input.size() == 2)
			{
				float s0 = calcEntrpyByValues(begin, begin + 1);
				float s1 = calcEntrpyByValues(begin + 1, end);
				if (s0 < s1)
				{
					left.push_back(input[0]);
					right.push_back(input[1]);
				}
				else
				{
					left.push_back(input[1]);
					right.push_back(input[0]);
				}
				return;
			}

			float s = calcEntrpyByValues(begin, end);
			if (s == 0)
			{
				left.insert(left.end(), begin, end);
				return;
			}

			std::cout << "Start with len: " << input.size() << std::endl;
			float minS = s;
			bool leftBigger = true;
			for (auto it = begin + 1; it != end; ++it)
			{
				float s0 = calcEntrpyByValues(begin, it);
				float s1 = calcEntrpyByValues(it, end);
				float sAvg = (s0 + s1) / 2;
				if (sAvg < minS)
				{
					leftBigger = s0 > s1;
					minS = sAvg;
					spkit = it;
					//break;
				}
			}
			if (spkit == end)
			{
				left.insert(left.end(), begin, end);
				return;
			}
			barsplitvec leftInput;
			barsplitvec rightInput;
			if (leftBigger)
			{
				// ≈сли лева€ часть больше, мен€ем местами
				leftInput.insert(leftInput.end(), spkit, end);
				rightInput.insert(rightInput.end(), begin, spkit);
			}
			else
			{
				leftInput.insert(leftInput.end(), begin, spkit);
				rightInput.insert(rightInput.end(), spkit, end);
			}

			std::cout << "Left len: " << leftInput.size() << std::endl;
			std::cout << "Right len: " << rightInput.size() << std::endl;
			
			splitRes(leftInput, left, right);
			splitRes(rightInput, left, right);
		}

		void splitByValue(barsplitvec& low, barsplitvec& high)
		{
			barsplitvec input;
			for (size_t i = 0; i < barlines.size(); i++)
			{
				input.push_back({ barlines[i], 0 });
			}
			splitRes(input, low, high);
		}

		void splitByMask(barlinevector& left, barlinevector& right, DatagridProvider& mask)
		{

			barsplitvec barvec;
			for (size_t i = 0; i < barlines.size(); i++)
			{
				Barlfd as;
				as.line = barlines.at(i);
				auto& matr = as.line->matr;
				for (size_t w = 0; w < matr.size(); ++w)
				{
					if (mask.get(matr[w].getPoint()) > 128)
					{
						as.acc += 1;
					}
				}

				as.acc /= mask.length();
			}
		}

#ifdef _PYD
		// only for uchar
		bp::list calcHistByBarlen(/*Barscalar maxLen*/)
		{
			int maxLen = 256;
			int* hist = new int[maxLen];
			memset(hist, 0, maxLen * sizeof(int));

			for (size_t i = 0; i < barlines.size(); i++)
				++hist[static_cast<int>(barlines[i]->len())];

			bp::list pyhist;
			for (size_t i = 0; i < maxLen; i++)
				pyhist.append(hist[i]);

			delete[] hist;

			return pyhist;
		}

		bp::list PY_getBettyNumbers()
		{
			int hist[256];
			getBettyNumbers(hist);
			bp::list pyhist;
			for (size_t i = 0; i < 256; i++)
				pyhist.append(hist[i]);

			return pyhist;
		}

		// only for uchar
		bp::list calcHistByPointsSize(/*Barscalar maxLen*/)
		{
			int rm = 0;
			for (size_t i = 0; i < barlines.size(); i++)
			{
				int rf = barlines[i]->getPointsSize();
				if (rf > rm)
					rm = rf;
			}
			int* hist = new int[rm];
			memset(hist, 0, rm * sizeof(int));

			for (size_t i = 0; i < barlines.size(); i++)
				++hist[barlines[i]->getPointsSize()];

			bp::list pyhist;
			for (size_t i = 0; i < rm; i++)
				pyhist.append(hist[i]);

			delete[] hist;

			return pyhist;
		}


		bp::list getBarcode()
		{
			bp::list lines;
			for (auto* line : barlines)
			{
				// on deliting list will call ~destr for every line
				lines.append(line->clone());
			}
			return lines;
		}
		
		float cmp(const Baritem* bitem, bc::CompireStrategy strat) const
		{
			return compireFull((const Baritem*)bitem, strat);
		}


#endif // _PYD

		void sortByLen();
		void sortBySize();
		void sortByStart();
	};

	//template<size_t N>
	
	class EXPORT Barcontainer : public Barbase
	{
		std::vector<Baritem*> items;
	public:
		Barcontainer();

		Barscalar sum() const;
		void relen();
		Barbase* clone() const;
		Barscalar maxLen() const;
		size_t count();
		//    Baritem *operator [](int i);
		Baritem *getItem(size_t i);

		void setItem(size_t index, Baritem *newOne)
		{
			if (index < items.size())
			{
				auto *item = items[index];
				delete item;
				items[index] = newOne;
			}
		}

		Baritem *exractItem(size_t index)
		{
			if (index < items.size())
			{
				auto *item = items[index];
				items[index] = nullptr;
				return item;
			}
			return nullptr;
		}

		void remoeLast()
		{
			int s = items.size();
			if (s > 0)
			{
				delete items[s - 1];
				items[s - 1] = nullptr;
				items.pop_back();
			}
		}

		void exractItems(std::vector<Baritem *> extr)
		{
			for (size_t i = 0; i < items.size(); ++i)
			{
				if (items[i]!=nullptr)
					extr.push_back(items[i]);
			}
			items.clear();
		}
		Baritem* lastItem();
		void addItem(Baritem* item);
		// remove lines than less then passed value
		void removePorog(Barscalar const porog);
		void preprocessBar(Barscalar const& porog, bool normalize);

		float compireFull(const Barbase* bc, bc::CompireStrategy strat) const;
		float compireBest(const Baritem* bc, bc::CompireStrategy strat) const;

		void clear()
		{
			for (size_t i = 0; i < items.size(); ++i)
			{
				if (items[i] != nullptr)
					delete items[i];
			}
			items.clear();
		}
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		~Barcontainer();

		// Barbase interface
	};

}
