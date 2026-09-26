#include "SettingsManager.h"
#include "DurationParser.h"

#include <QAudioOutput>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    const QString savedDefaultDuration = s.value(QStringLiteral("timer/defaultDuration"), QStringLiteral("20m")).toString();
    if (DurationParser::parse(savedDefaultDuration).ok())
        m_defaultDuration = savedDefaultDuration;
    m_adjustmentSeconds = qBound(1, s.value(QStringLiteral("timer/adjustmentSeconds"), 60).toInt(), 3600);
    m_repeatCount = qBound(0, s.value(QStringLiteral("alarm/repeatCount"), 1).toInt(), 3);
    m_customSoundPath = s.value(QStringLiteral("alarm/customSoundPath")).toString();
    m_notifyOnComplete = s.value(QStringLiteral("alarm/notifyOnComplete"), true).toBool();

    m_testPlayer = new QMediaPlayer(this);
    m_testAudio = new QAudioOutput(this);
    m_testPlayer->setAudioOutput(m_testAudio);
    m_testAudio->setVolume(1.0f);
}

QString SettingsManager::defaultDuration() const { return m_defaultDuration; }
int SettingsManager::adjustmentSeconds() const { return m_adjustmentSeconds; }
int SettingsManager::repeatCount() const { return m_repeatCount; }
QString SettingsManager::customSoundPath() const { return m_customSoundPath; }
bool SettingsManager::notifyOnComplete() const { return m_notifyOnComplete; }
bool SettingsManager::defaultSoundAvailable() const { return QFileInfo::exists(QStringLiteral(":/sounds/notify4.wav")); }

QString SettingsManager::soundDisplayName() const
{
    if (m_customSoundPath.isEmpty())
        return QStringLiteral("notify4 (CC0)");
    return QFileInfo(m_customSoundPath).fileName();
}

void SettingsManager::setDefaultDuration(const QString &value)
{
    const QString duration = value.trimmed();
    if (!DurationParser::parse(duration).ok() || m_defaultDuration == duration) return;
    m_defaultDuration = duration;
    save();
    emit defaultDurationChanged();
}

bool SettingsManager::isValidDuration(const QString &value) const
{
    return DurationParser::parse(value).ok();
}

void SettingsManager::setAdjustmentSeconds(int value)
{
    value = qBound(1, value, 3600);
    if (m_adjustmentSeconds == value) return;
    m_adjustmentSeconds = value;
    save();
    emit adjustmentSecondsChanged();
}

void SettingsManager::setRepeatCount(int value)
{
    value = qBound(0, value, 3);
    if (m_repeatCount == value) return;
    m_repeatCount = value;
    save();
    emit repeatCountChanged();
}

void SettingsManager::setCustomSoundPath(const QString &value)
{
    if (m_customSoundPath == value) return;
    m_customSoundPath = value;
    save();
    emit customSoundPathChanged();
}

void SettingsManager::setNotifyOnComplete(bool value)
{
    if (m_notifyOnComplete == value) return;
    m_notifyOnComplete = value;
    save();
    emit notifyOnCompleteChanged();
}

QUrl SettingsManager::effectiveSoundUrl() const
{
    if (!m_customSoundPath.isEmpty() && QFileInfo::exists(m_customSoundPath))
        return QUrl::fromLocalFile(m_customSoundPath);

    // notify4.wav is intentionally bundled into the binary when present.
    if (QFileInfo::exists(QStringLiteral(":/sounds/notify4.wav")))
        return QUrl(QStringLiteral("qrc:/sounds/notify4.wav"));

    return {};
}

void SettingsManager::resetSoundToDefault()
{
    setCustomSoundPath({});
}

void SettingsManager::testSound()
{
    const QUrl source = effectiveSoundUrl();
    if (source.isEmpty()) return;
    m_testPlayer->stop();
    m_testPlayer->setSource(source);
    m_testPlayer->setLoops(1);
    m_testPlayer->play();
}

QString SettingsManager::formatAdjustment(int seconds) const
{
    if (seconds % 60 == 0)
        return seconds == 60 ? QStringLiteral("1 minute") : QStringLiteral("%1 minutes").arg(seconds / 60);
    return seconds == 1 ? QStringLiteral("1 second") : QStringLiteral("%1 seconds").arg(seconds);
}

QString SettingsManager::settingsPath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/cttimer");
    QDir().mkpath(dir);
    return dir + QStringLiteral("/cttimer.conf");
}

void SettingsManager::save()
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    s.setValue(QStringLiteral("timer/defaultDuration"), m_defaultDuration);
    s.setValue(QStringLiteral("timer/adjustmentSeconds"), m_adjustmentSeconds);
    s.setValue(QStringLiteral("alarm/repeatCount"), m_repeatCount);
    s.setValue(QStringLiteral("alarm/customSoundPath"), m_customSoundPath);
    s.setValue(QStringLiteral("alarm/notifyOnComplete"), m_notifyOnComplete);
}
