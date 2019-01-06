#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "Handler.h"
#include "UrlsModel.h"

using namespace dx_test;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<dx_test::UrlsModel>("Urls", 1, 0, "UrlsModel");
    qmlRegisterUncreatableType<dx_test::UrlsList>("Urls", 1, 0, "UrlsList",
             QStringLiteral("UrlsList shouldn't be created in QML"));

    std::shared_ptr<UrlsList> urlsList = std::make_shared<UrlsList>();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("urlsList"), urlsList.get());
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    QObject* m_rootObject = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(m_rootObject);
    std::unique_ptr<Handler> handler = std::make_unique<Handler>(urlsList, window);

    QObject::connect(window, SIGNAL(sigRun(int, int, const QString&, const QString&)),
                         handler.get(), SLOT(sltRun(int, int, const QString&, const QString&)));

    QObject::connect(window, SIGNAL(sigStop()), handler.get(), SLOT(sltStop()));
    QObject::connect(window, SIGNAL(sigPause()), handler.get(), SLOT(sltPause()));
    QObject::connect(handler.get(), SIGNAL(sig_setStatus(QVariant)), window, SLOT(setStatus(QVariant)));
    QObject::connect(handler.get(), SIGNAL(sig_onRunning()), window, SLOT(onRunning()));
    QObject::connect(handler.get(), SIGNAL(sig_onStopped()), window, SLOT(onStopped()));
    QObject::connect(handler.get(), SIGNAL(sig_onResume()), window, SLOT(onResume()));
    QObject::connect(handler.get(), SIGNAL(sig_onPaused()), window, SLOT(onPaused()));
    QObject::connect(handler.get(), SIGNAL(sig_onDone()), window, SLOT(onDone()));
    QObject::connect(handler.get(), SIGNAL(sig_onProgress(QVariant)), window, SLOT(onProgress(QVariant)));

    return app.exec();
}
