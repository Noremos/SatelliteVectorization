#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>
#include <QQmlEngine>
#include "presets.h"

using namespace bc;
class Enums : public QObject
{
    Q_OBJECT
public:
    explicit Enums(QObject *parent = nullptr){}
    Q_ENUMS(ColorType)
    Q_ENUMS(ProcType)
    Q_ENUMS(ComponentType)

    // Do not forget to declare your class to the QML system.
    static void declateEnums(){
        qmlRegisterType<Enums>("MyEnums", 1, 0, "ColorType");
        qmlRegisterType<Enums>("MyEnums", 1, 0, "ProcType");
        qmlRegisterType<Enums>("MyEnums", 1, 0, "ComponentType");
    }

signals:

};

#endif // ENUMS_H
