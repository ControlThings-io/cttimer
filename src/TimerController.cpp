#include "TimerController.h"
#include "SettingsManager.h"

#include <QAudioOutput>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QMediaPlayer>
#include <QLocale>
#include <QTimer>
#include <QVariantMap>

TimerController::TimerController(qint64 durationMs, QString label, SettingsManager *settings, QObject *parent)
    : QObject(parent),
      m_totalMs(durationMs),
      m_remainingMs(durationMs),
      m_label(std::move(label)),
      m_settings(settings)
{
    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(100);
    m_tickTimer->setTimerType(Qt::PreciseTimer);
    connect(m_tickTimer, &QTimer::timeout, this, &TimerController::tick);

    m_player = new QMediaPlayer(this);
    m_audio = new QAudioOutput(this);
    m_audio->setVolume(1.0f);
    m_player->setAudioOutput(m_audio);
}

double TimerController::progress() const
{
    if (m_totalMs <= 0) return 0.0;
    return qBound(0.0, static_cast<double>(m_remainingMs) / static_cast<double>(m_totalMs), 1.0);
}

QString TimerController::displayTime() const
{
    qint64 totalSeconds = (m_remainingMs + 999) / 1000; // visually count down on whole seconds
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;
    const qint64 seconds = totalSeconds % 60;

    if (hours > 0)
        return QStringLiteral("%1:%2:%3").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
    return QStringLiteral("%1:%2").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
}

QString TimerController::endTimeText() const
{
    if (!hasTimer() || !m_running || m_finished) return {};
    return m_deadline.time().toString(QLocale().timeFormat(QLocale::ShortFormat));
}

void TimerController::start()
{
    if (!hasTimer() || m_finished || m_running) return;
    m_deadline = QDateTime::currentDateTime().addMSecs(m_remainingMs);
    m_running = true;
    m_tickTimer->start();
    emit runningChanged();
    emit endTimeChanged();
}

void TimerController::togglePaused()
{
    if (!hasTimer() || m_finished) return;

    if (m_running) {
        setRemaining(qMax<qint64>(0, QDateTime::currentDateTime().msecsTo(m_deadline)));
        m_running = false;
        m_tickTimer->stop();
    } else {
        m_deadline = QDateTime::currentDateTime().addMSecs(m_remainingMs);
        m_running = true;
        m_tickTimer->start();
    }
    emit runningChanged();
    emit endTimeChanged();
}

void TimerController::adjustByConfiguredIncrement(int direction)
{
    if (!hasTimer() || m_finished || direction == 0) return;

    if (m_running)
        setRemaining(qMax<qint64>(0, QDateTime::currentDateTime().msecsTo(m_deadline)));

    const qint64 delta = static_cast<qint64>(m_settings->adjustmentSeconds()) * 1000 * (direction > 0 ? 1 : -1);
    constexpr qint64 floorMs = 10'000;

    const qint64 oldRemaining = m_remainingMs;
    if (direction < 0 && oldRemaining <= floorMs)
        return;
    const qint64 newRemaining = direction < 0 ? qMax(floorMs, oldRemaining + delta) : oldRemaining + delta;
    const qint64 applied = newRemaining - oldRemaining;

    m_remainingMs = newRemaining;
    m_totalMs = qMax<qint64>(m_remainingMs, m_totalMs + applied);

    if (m_running)
        m_deadline = QDateTime::currentDateTime().addMSecs(m_remainingMs);

    emit remainingChanged();
    emit totalChanged();
    emit endTimeChanged();
}

void TimerController::stopAlarm()
{
    m_player->stop();
}

void TimerController::tick()
{
    if (!m_running) return;
    const qint64 left = QDateTime::currentDateTime().msecsTo(m_deadline);
    if (left <= 0) {
        setRemaining(0);
        complete();
        return;
    }
    setRemaining(left);
}

void TimerController::setRemaining(qint64 ms)
{
    ms = qMax<qint64>(0, ms);
    if (m_remainingMs == ms) return;
    m_remainingMs = ms;
    emit remainingChanged();
}

void TimerController::complete()
{
    if (m_finished) return;
    m_tickTimer->stop();
    m_running = false;
    m_finished = true;
    emit runningChanged();
    emit finishedChanged();
    emit endTimeChanged();
    playAlarm();
    sendNotification();
    emit completed();
}

void TimerController::playAlarm()
{
    const QUrl source = m_settings->effectiveSoundUrl();
    if (source.isEmpty()) return;

    m_player->stop();
    m_player->setSource(source);
    const int repeats = m_settings->repeatCount();
    m_player->setLoops(repeats == 0 ? QMediaPlayer::Infinite : repeats);
    m_player->play();
}

void TimerController::sendNotification()
{
    if (!m_settings->notifyOnComplete()) return;

    QDBusInterface notifications(
        QStringLiteral("org.freedesktop.Notifications"),
        QStringLiteral("/org/freedesktop/Notifications"),
        QStringLiteral("org.freedesktop.Notifications"),
        QDBusConnection::sessionBus());

    if (!notifications.isValid()) return;

    const QString body = m_label.isEmpty()
        ? QStringLiteral("Your timer has finished.")
        : QStringLiteral("%1 has finished.").arg(m_label);

    notifications.asyncCall(
        QStringLiteral("Notify"),
        QStringLiteral("cttimer"),
        uint(0),
        QStringLiteral("io.controlthings.cttimer"),
        QStringLiteral("Timer complete"),
        body,
        QStringList{},
        QVariantMap{},
        7000);
}
