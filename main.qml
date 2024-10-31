import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore
import org.bc.main 1.0 // Ensure this module exists in your project, as it might be custom.

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Ba Class")
    id: win

    Home {
        id: home
        onShowDialog: {
            fileDialog.open()
        }
        onLogChanged: {
            listModel.push({ messageStr: home.log })
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"

        Text {
            text: "B&C"
            font.bold: true
            font.pointSize: 30
            anchors.bottom: rect.top
            anchors.bottomMargin: 50
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: status
            text: home.status
            width: rect.width
            wrapMode: Text.WrapAnywhere
            anchors.left: rect.left
            anchors.bottom: rect.top
            anchors.bottomMargin: 5
            color: "white"
        }

        Rectangle {
            id: rect
            width: parent.width - 200
            height: 50
            radius: 8
            clip: true
            anchors.centerIn: parent

            Loader {
                id: itemLoader
                anchors.fill: parent
                sourceComponent: home.loading ? pComp : tfComp
            }

            Component {
                id: pComp
                Item {
                    anchors.fill: parent

                    CustomProgressBar {
                        id: pbar
                        visible: true
                        anchors.fill: parent
                        progress: home.readInt/home.totalInt*100
                    }

                    Text {
                        id: readtxt
                        visible: !home.processing
                        anchors.left: parent.left
                        anchors.top: parent.bottom
                        anchors.topMargin: 5
                        text: home.read
                        color: "white"
                    }

                    Text {
                        id: totaltxt
                        visible: !home.processing
                        anchors.left: readtxt.right
                        anchors.top: parent.bottom
                        anchors.topMargin: 5
                        text: " / " + home.total
                        color: "white"
                    }

                    Text {
                        id: speedtxt
                        visible: !home.processing
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        anchors.topMargin: 5
                        text: home.speed
                        color: "white"
                    }
                }
            }

            Component {
                id: tfComp
                TextField {
                    id: txtF
                    anchors.fill: parent
                    placeholderText: "Enter https://..."
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 10
                    font.pointSize: 15
                }
            }
        }

        Rectangle {
            color: "white"
            visible: home.processing
            width: rect.width
            height: 150
            radius: 8
            anchors.left: rect.left
            anchors.top: rect.bottom
            anchors.topMargin: 20

            Component {
                id: delegate
                TextInput {
                    width: view.width - 10
                    selectByMouse: true
                    text: messageStr
                    color: "red"
                    wrapMode: Text.WordWrap
                    readOnly: true
                    font.pointSize: 12
                }
            }

            ListView {
                id: view
                clip: true
                anchors.fill: parent
                model: listModel
                delegate: delegate
                onCountChanged: view.positionViewAtEnd()
                spacing: 10
            }

            ListModel {
                id: listModel
                function push(newElement) {
                    if (listModel.count >= 50)
                        listModel.remove(0);
                    listModel.append(newElement);
                }
            }
        }

        FileDialog {
            id: fileDialog
            fileMode: FileDialog.SaveFile
            nameFilters: ["Video (*.flv)"]
            currentFolder: StandardPaths.standardLocations(StandardPaths.DesktopLocation)[0]
            onAccepted: home.saveFileAs(selectedFile)
        }

        RoundButton {
            visible: !home.processing
            width: 80
            height: width
            radius: width
            scale: pressed ? 0.9 : 1.0
            enabled: !home.loading

            Behavior on scale {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            onClicked: home.downloadFile(itemLoader.item ? itemLoader.item.text : "")

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rect.bottom
            anchors.topMargin: 20

            icon.source: home.loading ? "" : "qrc:/icon/icons/download.png"
            icon.height: 50
            icon.width: 50

            BusyIndicator {
                id: busyItem
                running: home.loading
                anchors.fill: parent
            }
        }
    }
}
