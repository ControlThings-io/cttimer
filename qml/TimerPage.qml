import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import "components"

Kirigami.Page {
    id: root
    padding: Kirigami.Units.largeSpacing

    signal requestSettings()

    Keys.onSpacePressed: timerController.togglePaused()
    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_Plus || event.key === Qt.Key_Equal) {
            timerController.adjustByConfiguredIncrement(1)
            event.accepted = true
        } else if (event.key === Qt.Key_Minus || event.key === Qt.Key_Underscore) {
            timerController.adjustByConfiguredIncrement(-1)
            event.accepted = true
        }
    }

    Component.onCompleted: forceActiveFocus()

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Controls.ToolButton {
                focusPolicy: Qt.NoFocus
                icon.name: "settings-configure"
                text: "Settings"
                display: Controls.AbstractButton.IconOnly
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: "Settings (Ctrl+,)"
                onClicked: root.requestSettings()
            }
        }

        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Math.min(root.width - Kirigami.Units.gridUnit * 4, 300)
            Layout.preferredHeight: width

            ProgressRing {
                anchors.fill: parent
                progress: timerController.progress
                ringColor: timerController.finished
                    ? Kirigami.Theme.positiveTextColor
                    : (!timerController.running ? Kirigami.Theme.neutralTextColor : Kirigami.Theme.highlightColor)
            }

            Column {
                anchors.centerIn: parent
                spacing: Kirigami.Units.smallSpacing

                Controls.Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: timerController.finished ? "Done" : (!timerController.running ? "Paused" : (timerController.label.length ? timerController.label : "Timer"))
                    opacity: 0.75
                    font.pixelSize: Kirigami.Theme.defaultFont.pixelSize * 1.1
                }

                Controls.Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: timerController.displayTime
                    font.pixelSize: timerController.displayTime.length > 5 ? 42 : 52
                    font.weight: Font.Light
                }
            }

            TapHandler {
                enabled: !timerController.finished
                onTapped: timerController.togglePaused()
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.smallSpacing
            Kirigami.Icon {
                source: "alarm-symbolic"
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: implicitWidth
                color: Kirigami.Theme.disabledTextColor
            }
            Controls.Label {
                text: timerController.finished ? "Finished" : (!timerController.running ? "Paused" : "Ends at " + timerController.endTimeText)
                opacity: 0.65
            }
        }

        Item { Layout.fillHeight: true }
    }
}
