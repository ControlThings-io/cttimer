import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root
    title: "Settings"

    FileDialog {
        id: soundDialog
        title: "Choose alarm sound"
        nameFilters: ["Audio files (*.wav *.ogg *.oga *.mp3 *.flac)", "All files (*)"]
        onAccepted: settingsManager.customSoundPath = decodeURIComponent(selectedFile.toString().replace(/^file:\/\//, ""))
    }

    ColumnLayout {
        width: root.availableWidth
        spacing: Kirigami.Units.largeSpacing

        Kirigami.FormLayout {
            Layout.fillWidth: true

            Controls.SpinBox {
                Kirigami.FormData.label: "Time adjustment:"
                from: 1
                to: 3600
                stepSize: 1
                value: settingsManager.adjustmentSeconds
                editable: true
                textFromValue: function(value) { return settingsManager.formatAdjustment(value) }
                valueFromText: function(text) {
                    const t = text.trim().toLowerCase()
                    if (t.endsWith("min") || t.endsWith("minute") || t.endsWith("minutes"))
                        return Math.round(parseFloat(t) * 60)
                    if (t.endsWith("s") || t.endsWith("sec") || t.endsWith("second") || t.endsWith("seconds"))
                        return Math.round(parseFloat(t))
                    return Math.round(parseFloat(t))
                }
                onValueModified: settingsManager.adjustmentSeconds = value
            }

            Controls.CheckBox {
                Kirigami.FormData.label: "Window:"
                text: "Always on top"
                checked: settingsManager.alwaysOnTop
                onToggled: settingsManager.alwaysOnTop = checked
            }

            Controls.CheckBox {
                Kirigami.FormData.label: "Completion:"
                text: "Show desktop notification"
                checked: settingsManager.notifyOnComplete
                onToggled: settingsManager.notifyOnComplete = checked
            }

            Controls.ComboBox {
                id: repeatCombo
                Kirigami.FormData.label: "Repeat sound:"
                model: ["Until dismissed", "Once", "Twice", "3 times"]
                currentIndex: settingsManager.repeatCount
                onActivated: settingsManager.repeatCount = currentIndex
            }
        }

        Kirigami.Separator { Layout.fillWidth: true }

        Controls.Label {
            text: "Alarm sound"
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            Controls.Label {
                Layout.fillWidth: true
                text: settingsManager.soundDisplayName
                elide: Text.ElideMiddle
            }
            Controls.Button {
                text: "Test"
                icon.name: "media-playback-start"
                onClicked: settingsManager.testSound()
            }
        }

        RowLayout {
            Controls.Button {
                text: "Choose file…"
                icon.name: "document-open"
                onClicked: soundDialog.open()
            }
            Controls.Button {
                text: "Use notify4"
                enabled: settingsManager.customSoundPath.length > 0
                onClicked: settingsManager.resetSoundToDefault()
            }
        }

        Controls.Label {
            Layout.fillWidth: true
            visible: settingsManager.customSoundPath.length === 0 && settingsManager.defaultSoundAvailable
            text: "Default sound: notify4 by Mihacappy (CC0)."
            color: Kirigami.Theme.disabledTextColor
            font.pixelSize: Kirigami.Theme.defaultFont.pixelSize * 0.9
            wrapMode: Text.WordWrap
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: settingsManager.customSoundPath.length === 0 && !settingsManager.defaultSoundAvailable
            type: Kirigami.MessageType.Warning
            text: "notify4 is not embedded in this build; choose a custom sound or add the CC0 source asset and rebuild."
        }

        Item { Layout.fillHeight: true }
    }
}
