import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: window
    visible: true
    width: settingsOnly ? 520 : 328
    height: settingsOnly ? 520 : 364
    minimumWidth: settingsOnly ? 360 : 328
    minimumHeight: settingsOnly ? 400 : 364
    title: settingsOnly ? "cttimer Settings" : (timerController.label.length ? timerController.label + " — cttimer" : "cttimer")

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
