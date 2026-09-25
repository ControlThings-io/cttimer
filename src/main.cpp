#include "DurationParser.h"
#include "SettingsManager.h"
#include "TimerController.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTextStream>
#include <QTimer>

#ifndef CTTIMER_VERSION
#define CTTIMER_VERSION "0.2.0"
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("cttimer"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("cttimer"));
    QGuiApplication::setApplicationVersion(QStringLiteral(CTTIMER_VERSION));
    QGuiApplication::setOrganizationName(QStringLiteral("ControlThings"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("controlthings.io"));
    QGuiApplication::setDesktopFileName(QStringLiteral("io.controlthings.cttimer"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("A focused KDE/Kirigami countdown timer."));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption debugOption(QStringList{QStringLiteral("debug")}, QStringLiteral("Keep the terminal attached and show diagnostic output."));
    parser.addOption(debugOption);
    QCommandLineOption settingsOption(QStringList{QStringLiteral("settings")}, QStringLiteral("Open settings instead of starting a timer."));
    parser.addOption(settingsOption);
    parser.addPositionalArgument(QStringLiteral("duration"), QStringLiteral("Timer duration, e.g. 10m, 45s, 1h30m, 25:00."), QStringLiteral("[duration]"));
    parser.addPositionalArgument(QStringLiteral("label"), QStringLiteral("Optional timer label."), QStringLiteral("[label]"));
    parser.process(app);

    const QStringList positional = parser.positionalArguments();
    const bool settingsOnly = parser.isSet(settingsOption);
    SettingsManager settings;

    qint64 durationMs = 0;
    QString label;
    if (!settingsOnly) {
        const ParsedDuration parsed = DurationParser::parse(positional.isEmpty() ? settings.defaultDuration() : positional.at(0));
        if (!parsed.ok()) {
            QTextStream err(stderr);
            err << "cttimer: " << parsed.error << "\n\n" << DurationParser::usage();
            return 2;
        }
        durationMs = parsed.milliseconds;
        if (positional.size() > 1)
            label = positional.mid(1).join(QLatin1Char(' '));
    }

    if (!parser.isSet(debugOption) && !qEnvironmentVariableIsSet("CTTIMER_DETACHED")) {
        QProcess child;
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        environment.insert(QStringLiteral("CTTIMER_DETACHED"), QStringLiteral("1"));
        child.setProcessEnvironment(environment);
        child.setStandardInputFile(QProcess::nullDevice());
        child.setStandardOutputFile(QProcess::nullDevice());
        child.setStandardErrorFile(QProcess::nullDevice());
        if (!child.startDetached(QCoreApplication::applicationFilePath(), app.arguments().mid(1))) {
            QTextStream(stderr) << "cttimer: could not start detached process\n";
            return 1;
        }
        return 0;
    }

    TimerController timer(durationMs, label, &settings);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("timerController"), &timer);
    engine.rootContext()->setContextProperty(QStringLiteral("settingsManager"), &settings);
    engine.rootContext()->setContextProperty(QStringLiteral("settingsOnly"), settingsOnly);

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] {
        QCoreApplication::exit(1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if (!settingsOnly)
        QTimer::singleShot(0, &timer, &TimerController::start);

    return app.exec();
}
