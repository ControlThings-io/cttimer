#pragma once

#include <QObject>
#include <QUrl>

class QMediaPlayer;
class QAudioOutput;

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString defaultDuration READ defaultDuration WRITE setDefaultDuration NOTIFY defaultDurationChanged)
    Q_PROPERTY(int adjustmentSeconds READ adjustmentSeconds WRITE setAdjustmentSeconds NOTIFY adjustmentSecondsChanged)
    Q_PROPERTY(bool alwaysOnTop READ alwaysOnTop WRITE setAlwaysOnTop NOTIFY alwaysOnTopChanged)
    Q_PROPERTY(int repeatCount READ repeatCount WRITE setRepeatCount NOTIFY repeatCountChanged)
    Q_PROPERTY(QString customSoundPath READ customSoundPath WRITE setCustomSoundPath NOTIFY customSoundPathChanged)
    Q_PROPERTY(QString soundDisplayName READ soundDisplayName NOTIFY customSoundPathChanged)
    Q_PROPERTY(bool notifyOnComplete READ notifyOnComplete WRITE setNotifyOnComplete NOTIFY notifyOnCompleteChanged)
    Q_PROPERTY(bool defaultSoundAvailable READ defaultSoundAvailable CONSTANT)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QString defaultDuration() const;
    int adjustmentSeconds() const;
    bool alwaysOnTop() const;
    int repeatCount() const;
    QString customSoundPath() const;
    QString soundDisplayName() const;
    bool notifyOnComplete() const;
    bool defaultSoundAvailable() const;

    void setDefaultDuration(const QString &value);
    void setAdjustmentSeconds(int value);
    void setAlwaysOnTop(bool value);
    void setRepeatCount(int value);
    void setCustomSoundPath(const QString &value);
    void setNotifyOnComplete(bool value);

    QUrl effectiveSoundUrl() const;

    Q_INVOKABLE bool isValidDuration(const QString &value) const;
    Q_INVOKABLE void resetSoundToDefault();
    Q_INVOKABLE void testSound();
    Q_INVOKABLE QString formatAdjustment(int seconds) const;

signals:
    void defaultDurationChanged();
    void adjustmentSecondsChanged();
    void alwaysOnTopChanged();
    void repeatCountChanged();
    void customSoundPathChanged();
    void notifyOnCompleteChanged();

private:
    static QString settingsPath();
    void save();

    QString m_defaultDuration = QStringLiteral("20m");
    int m_adjustmentSeconds = 60;
    bool m_alwaysOnTop = true;
    int m_repeatCount = 1; // 0 = until dismissed
    QString m_customSoundPath;
    bool m_notifyOnComplete = true;
    QMediaPlayer *m_testPlayer = nullptr;
    QAudioOutput *m_testAudio = nullptr;
};
