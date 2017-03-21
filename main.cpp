#include "stdafx.h"
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlContext>
#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QTranslator translator;
    translator.load("messenger_qt_" + QLocale::system().name(), ":/languages/");
    app.installTranslator(&translator);

    QQmlApplicationEngine engine;
    QtWindowInterface inter;
    engine.rootContext()->setContextProperty("cppInterface", &inter);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
