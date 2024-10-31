import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: customProgressBar
    width: 300
    height: 40
    color: "#F0F0F0"
    radius: 8
    border.color: "#D0D0D0"
    border.width: 2

    // Properties for customizing
    property int progress: 0
    property bool isLoading: false
    property color barColor: "#3A86FF"
    property color loadingColor: "#FFAB40" // Loading color
    property int cornerRadius: 8

    // Define the states
    states: [
        State {
            name: "loading"
            when: customProgressBar.isLoading
            PropertyChanges { target: progressBar; color: customProgressBar.loadingColor }
        },
        State {
            name: "complete"
            when: !customProgressBar.isLoading && customProgressBar.progress >= 100
            PropertyChanges { target: progressBar; width: customProgressBar.width; color: "green" }
        },
        State {
            name: "inProgress"
            when: !customProgressBar.isLoading && customProgressBar.progress < 100
            PropertyChanges { target: progressBar; color: customProgressBar.barColor }
        }
    ]

    // Smooth transition between states
    transitions: [
        Transition {
            NumberAnimation { properties: "width,color"; duration: 400; easing.type: Easing.InOutQuad }
        }
    ]

    // Progress bar background
    Rectangle {
        id: progressBar
        width: parent.width * customProgressBar.progress / 100
        height: parent.height
        color: barColor
        radius: cornerRadius
        smooth: true

        // Animation for smooth width change
        Behavior on width {
            NumberAnimation { duration: 400; easing.type: Easing.InOutQuad }
        }
    }

    // Progress percentage text
    Text {
        anchors.centerIn: parent
        text: customProgressBar.isLoading ? "Loading..." : customProgressBar.progress + "%"
        color: "white"
        font.pixelSize: 16
    }
}
