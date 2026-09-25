#pragma once

#include <QObject>
#include <QDateTime>

class QTimer;
class QMediaPlayer;
class QAudioOutput;
class SettingsManager;

class TimerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 remainingMs READ remainingMs NOTIFY remainingChanged)
    Q_PROPERTY(qint64 totalMs READ totalMs NOTIFY totalChanged)
    Q_PROPERTY(double progress READ progress NOTIFY remainingChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool finished READ finished NOTIFY finishedChanged)
    Q_PROPERTY(QString displayTime READ displayTime NOTIFY remainingChanged)
    Q_PROPERTY(QString endTimeText READ endTimeText NOTIFY endTimeChanged)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(bool hasTimer READ hasTimer CONSTANT)

public:
    TimerController(qint64 durationMs, QString label, SettingsManager *settings, QObject *parent = nullptr);

    qint64 remainingMs() const { return m_remainingMs; }
    qint64 totalMs() const { return m_totalMs; }
    double progress() const;
    bool running() const { return m_running; }
    bool finished() const { return m_finished; }
    QString displayTime() const;
    QString endTimeText() const;
    QString label() const { return m_label; }
    bool hasTimer() const { return m_totalMs > 0; }

    Q_INVOKABLE void start();
    Q_INVOKABLE void togglePaused();
    Q_INVOKABLE void adjustByConfiguredIncrement(int direction);
    Q_INVOKABLE void stopAlarm();

signals:
    void remainingChanged();
    void totalChanged();
    void runningChanged();
    void finishedChanged();
    void endTimeChanged();
    void completed();

private slots:
    void tick();

private:
    void setRemaining(qint64 ms);
    void complete();
    void playAlarm();
    void sendNotification();

    qint64 m_totalMs = 0;
    qint64 m_remainingMs = 0;
    bool m_running = false;
    bool m_finished = false;
    QString m_label;
    QDateTime m_deadline;
    QTimer *m_tickTimer = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audio = nullptr;
    SettingsManager *m_settings = nullptr;
};
