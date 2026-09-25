import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: window
    visible: true
    width: settingsOnly ? 520 : 410
    height: settingsOnly ? 520 : 455
    minimumWidth: 360
    minimumHeight: 400
    title: settingsOnly ? "cttimer Settings" : (timerController.label.length ? timerController.label + " — cttimer" : "cttimer")
    flags: Qt.Window | (settingsManager.alwaysOnTop && !settingsOnly ? Qt.WindowStaysOnTopHint : 0)

    onClosing: timerController.stopAlarm()

    Shortcut {
        sequence: "Ctrl+,"
        onActivated: {
            if (settingsOnly)
                return
            settingsDialog.open()
        }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: settingsOnly ? settingsComponent : timerComponent
    }

    Component {
        id: timerComponent
        TimerPage {
            onRequestSettings: settingsDialog.open()
        }
    }

    Component {
        id: settingsComponent
        SettingsPage { }
    }

    SettingsDialog {
        id: settingsDialog
    }
}
