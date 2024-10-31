#include <QApplication>
#include <QQmlApplicationEngine>
#include "qhome.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QHome::registerType();
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/BaClass/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
