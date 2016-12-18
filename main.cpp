#include "stdafx.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QtWindowInterface inter;
    engine.rootContext()->setContextProperty("cppInterface", &inter);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
