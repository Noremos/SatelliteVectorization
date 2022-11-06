#pragma once
#include "component.h"
namespace bc
{


class Hole : public Component
{
    int index = 0;
public:
    Barscalar start, end;
    bool isValid = false;
    pmap crossHoles;

    Hole(point p1, point p2, point p3, BarcodeCreator* factory);
    Hole(point p1, BarcodeCreator* factory);
    ~Hole();

//        void setOutside(bool b) {
//            isOutside = b;

//        }
//        inline bool getOutside() { return isOutside;}
//    void addAround();
    bool tryAdd(const point &p);
    //add withot checks. NOT SAFE. not add if valid
    //************************************************************************//
    bool checkValid(const bc::point& p);
    //добовляет точку в дыру, если она там не содержится. Возвращает

    //явяется ли точка точкой соединения двух дыр
    bool findCross(point p, Hole* hole);
    bool isContain(int x, int y);
    bool isContain(const bc::point& p);
};

}
