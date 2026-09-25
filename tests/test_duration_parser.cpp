#include <QtTest>
#include "DurationParser.h"

class DurationParserTest : public QObject
{
    Q_OBJECT
private slots:
    void parsesCommonFormats()
    {
        QCOMPARE(DurationParser::parse("10m").milliseconds, qint64(600000));
        QCOMPARE(DurationParser::parse("45s").milliseconds, qint64(45000));
        QCOMPARE(DurationParser::parse("1h30m").milliseconds, qint64(5400000));
        QCOMPARE(DurationParser::parse("25:00").milliseconds, qint64(1500000));
        QCOMPARE(DurationParser::parse("1:30:00").milliseconds, qint64(5400000));
        QCOMPARE(DurationParser::parse("10").milliseconds, qint64(600000));
    }

    void rejectsInvalidFormats()
    {
        QVERIFY(!DurationParser::parse("").ok());
        QVERIFY(!DurationParser::parse("ten minutes").ok());
        QVERIFY(!DurationParser::parse("0s").ok());
        QVERIFY(!DurationParser::parse("1m1m").ok());
    }
};

QTEST_MAIN(DurationParserTest)
#include "test_duration_parser.moc"
