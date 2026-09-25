#include "DurationParser.h"

#include <QRegularExpression>

ParsedDuration DurationParser::parse(const QString &input)
{
    const QString text = input.trimmed().toLower();
    if (text.isEmpty())
        return {0, QStringLiteral("No duration specified.")};

    // A bare integer is interpreted as minutes for convenient CLI use.
    static const QRegularExpression bareMinutes(QStringLiteral(R"(^\d+$)"));
    if (bareMinutes.match(text).hasMatch()) {
        bool ok = false;
        const qint64 minutes = text.toLongLong(&ok);
        if (!ok || minutes <= 0)
            return {0, QStringLiteral("Duration must be greater than zero.")};
        return {minutes * 60'000, {}};
    }

    // Also accept colon notation: MM:SS or HH:MM:SS.
    static const QRegularExpression colon(QStringLiteral(R"(^(?:(\d+):)?([0-5]?\d):([0-5]\d)$)"));
    const auto cm = colon.match(text);
    if (cm.hasMatch()) {
        const qint64 hours = cm.captured(1).isEmpty() ? 0 : cm.captured(1).toLongLong();
        const qint64 minutes = cm.captured(2).toLongLong();
        const qint64 seconds = cm.captured(3).toLongLong();
        const qint64 ms = ((hours * 60 + minutes) * 60 + seconds) * 1000;
        if (ms <= 0)
            return {0, QStringLiteral("Duration must be greater than zero.")};
        return {ms, {}};
    }

    // Compact forms such as 1h30m, 10m, 45s, 2h5m10s.
    static const QRegularExpression token(QStringLiteral(R"((\d+)(h|m|s))"));
    auto it = token.globalMatch(text);
    qint64 totalMs = 0;
    int consumed = 0;
    bool sawH = false, sawM = false, sawS = false;

    while (it.hasNext()) {
        const auto m = it.next();
        if (m.capturedStart() != consumed)
            return {0, QStringLiteral("Invalid duration: %1").arg(input)};
        consumed = m.capturedEnd();

        const qint64 value = m.captured(1).toLongLong();
        const QChar unit = m.captured(2).at(0);
        if (unit == QLatin1Char('h')) {
            if (sawH) return {0, QStringLiteral("Hours specified more than once.")};
            sawH = true;
            totalMs += value * 3'600'000;
        } else if (unit == QLatin1Char('m')) {
            if (sawM) return {0, QStringLiteral("Minutes specified more than once.")};
            sawM = true;
            totalMs += value * 60'000;
        } else {
            if (sawS) return {0, QStringLiteral("Seconds specified more than once.")};
            sawS = true;
            totalMs += value * 1000;
        }
    }

    if (consumed != text.size() || totalMs <= 0)
        return {0, QStringLiteral("Invalid duration: %1").arg(input)};

    return {totalMs, {}};
}

QString DurationParser::usage()
{
    return QStringLiteral(
        "Usage:\n"
        "  cttimer <duration> [label]\n"
        "  cttimer --settings\n\n"
        "Duration examples:\n"
        "  10m       10 minutes\n"
        "  45s       45 seconds\n"
        "  1h30m     1 hour 30 minutes\n"
        "  25:00     25 minutes\n"
        "  1:30:00   1 hour 30 minutes\n"
        "  10        10 minutes\n"
    );
}
