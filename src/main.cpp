#include "DurationParser.h"
#include "SettingsManager.h"
#include "TimerController.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QProcess>
#include <QTextStream>
#include <QTimer>

#ifndef CTTIMER_VERSION
#define CTTIMER_VERSION "0.2.0"
#endif

int main(int argc, char *argv[])
{
    QStringList positional;
    bool settingsOnly = false;
    // Parse and detach before initializing display drivers or multimedia.
    {
        QCoreApplication app(argc, argv);
        QCoreApplication::setApplicationName(QStringLiteral("cttimer"));
        QCoreApplication::setApplicationVersion(QStringLiteral(CTTIMER_VERSION));
        QCoreApplication::setOrganizationName(QStringLiteral("ControlThings"));
        QCoreApplication::setOrganizationDomain(QStringLiteral("controlthings.io"));

        QCommandLineParser parser;
        parser.setApplicationDescription(QStringLiteral("A focused KDE/Kirigami countdown timer."));
        parser.addHelpOption();
        parser.addVersionOption();
        QCommandLineOption debugOption(QStringList{QStringLiteral("debug")}, QStringLiteral("Keep the terminal attached and show diagnostic output."));
        parser.addOption(debugOption);
        QCommandLineOption detachedChildOption(QStringList{QStringLiteral("cttimer-detached-child")});
        detachedChildOption.setFlags(QCommandLineOption::HiddenFromHelp);
        parser.addOption(detachedChildOption);
        QCommandLineOption settingsOption(QStringList{QStringLiteral("settings")}, QStringLiteral("Open settings instead of starting a timer."));
        parser.addOption(settingsOption);
        parser.addPositionalArgument(QStringLiteral("duration"), QStringLiteral("Timer duration, e.g. 10m, 45s, 1h30m, 25:00."), QStringLiteral("[duration]"));
        parser.addPositionalArgument(QStringLiteral("label"), QStringLiteral("Optional timer label."), QStringLiteral("[label]"));
        parser.process(app);

        positional = parser.positionalArguments();
        settingsOnly = parser.isSet(settingsOption);
        // Validate explicit input before returning control to the terminal.
        if (!settingsOnly && !positional.isEmpty()) {
            const ParsedDuration parsed = DurationParser::parse(positional.at(0));
            if (!parsed.ok()) {
                QTextStream err(stderr);
                err << "cttimer: " << parsed.error << "\n\n" << DurationParser::usage();
                return 2;
            }
        }

        if (!parser.isSet(debugOption) && !parser.isSet(detachedChildOption)) {
            QProcess child;
            child.setStandardInputFile(QProcess::nullDevice());
            child.setStandardOutputFile(QProcess::nullDevice());
            child.setStandardErrorFile(QProcess::nullDevice());
            QStringList childArguments = app.arguments().mid(1);
            childArguments.prepend(QStringLiteral("--cttimer-detached-child"));
            child.setProgram(QCoreApplication::applicationFilePath());
            child.setArguments(childArguments);
            // Use the instance overload so the standard stream redirections apply.
            if (!child.startDetached()) {
                QTextStream(stderr) << "cttimer: could not start detached process\n";
                return 1;
            }
            return 0;
        }

    }

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(QStringLiteral("cttimer"));
    QGuiApplication::setDesktopFileName(QStringLiteral("io.controlthings.cttimer"));

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
