//#include <QApplication>
#include <QQmlApplicationEngine>

#include <QGuiApplication>
#include <QQmlContext>

#include "backend.h"

#include "imageitem.h"
#include "enums.h"
//#include <iostream>

#include "geodataprocessor.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	app.setOrganizationName("somename");
	app.setOrganizationDomain("somename");

	QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

	qmlRegisterType<ImageItem>("My", 1, 0, "ImageItem");
	Enums::declateEnums();

    backend back;
    engine.rootContext()->setContextProperty("backend", &back);

    engine.load(url);
//    back.root = qobject_cast<QQuickItem *>(engine.rootObjects().first());
    back.root = engine.rootObjects().first();

    return app.exec();
}
