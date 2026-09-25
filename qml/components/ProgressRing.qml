import QtQuick
import org.kde.kirigami as Kirigami

Item {
    id: root
    property real progress: 1.0
    property color ringColor: Kirigami.Theme.highlightColor
    property color trackColor: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.13)
    property real thickness: 10

    onProgressChanged: canvas.requestPaint()
    onRingColorChanged: canvas.requestPaint()
    onTrackColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            const cx = width / 2
            const cy = height / 2
            const radius = Math.min(width, height) / 2 - root.thickness
            const start = -Math.PI / 2
            const end = start + (Math.PI * 2 * Math.max(0, Math.min(1, root.progress)))

            ctx.lineWidth = root.thickness
            ctx.lineCap = "round"

            ctx.beginPath()
            ctx.strokeStyle = root.trackColor
            ctx.arc(cx, cy, radius, 0, Math.PI * 2)
            ctx.stroke()

            if (root.progress > 0) {
                ctx.beginPath()
                ctx.strokeStyle = root.ringColor
                ctx.arc(cx, cy, radius, start, end)
                ctx.stroke()
            }
        }
    }
}
