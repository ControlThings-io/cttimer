#pragma once

#include <QString>
#include <QtGlobal>

struct ParsedDuration {
    qint64 milliseconds = 0;
    QString error;
    bool ok() const { return milliseconds > 0 && error.isEmpty(); }
};

class DurationParser
{
public:
    static ParsedDuration parse(const QString &input);
    static QString usage();
};
