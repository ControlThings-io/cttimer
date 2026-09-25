import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Controls.Dialog {
    id: dialog
    title: "cttimer Settings"
    modal: true
    width: Math.min(520, parent ? parent.width - Kirigami.Units.largeSpacing * 2 : 520)
    height: Math.min(520, parent ? parent.height - Kirigami.Units.largeSpacing * 2 : 520)
    parent: Controls.Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    standardButtons: Controls.Dialog.Close

    SettingsPage {
        anchors.fill: parent
    }
}
