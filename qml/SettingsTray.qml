import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtGraphicalEffects 1.0

Item {
    id: settingsTray

    width: 300
    height: settingsColumn.childrenRect.height

    property bool bHidden: false
    property double leftMargin: bHidden ? (-1 * settingsTray.width) : 0

    anchors.leftMargin: settingsTray.leftMargin

    property alias overlayChecked: overlayCheckbox.checked

    Component.onCompleted: {
    }

    Behavior on leftMargin {
        NumberAnimation {
            duration: 250
        }
    }

    Rectangle {
        anchors.fill: parent
        color: colorConfig.settingsSliderColor
    }

    Button {
        anchors.left: parent.right
        anchors.top: parent.top

        height: 30
        width: 30

        text: settingsTray.bHidden ? ">" : "<"

        onClicked: {
            settingsTray.bHidden = !settingsTray.bHidden;

        }
    }

    Column {
        id: settingsColumn
        anchors.fill: parent

        // scales everything to system font size
        spacing: mainWindow.height * 0.006
        property int iSpacing: 10
        property int itemWidth: settingsTray.width * 0.9

        Text {
            id: settingsTrayTitle
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 6

            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: settingsColumn.spacing * 3

            text: qsTr("Session Settings...")

            color: colorConfig.textColorLight

            verticalAlignment: Text.AlignBottom
            horizontalAlignment: Text.AlignLeft
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "Shutter Timer (us)"
            color: colorConfig.textColorLight
            font.italic: true
        }

        TextField {
            id: shutterTimeEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "2000"
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "Gain"
            color: colorConfig.textColorLight
            font.italic: true
        }

        TextField {
            id: gainEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "1"
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Item {
            height: mainWindow.height * 0.035
            width: settingsColumn.itemWidth
            anchors.horizontalCenter: parent.horizontalCenter

            CheckBox {
                id: overlayCheckbox
                anchors.left: parent.left
                height: parent.height
                onClicked: {
                }
            }

            Text {
                height: parent.height
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight

                color: colorConfig.textColorLight
                text: "Focus Overlay"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    overlayCheckbox.checked = !overlayCheckbox.checked
                }
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Start Camera Stream"


            onClicked: {
                app.runRemoteCommand("cm4", "pi", "libcamera-vid --camera 0 -v 0 -t 0 --width 1456 --height 1088 --awb indoor --inline --listen -o tcp://0.0.0.0:8888 --framerate 50 --shutter " + shutterTimeEntry.text + " --gain " + gainEntry.text)
                app.connectToStream()
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Stop Camera Stream"

            onClicked: {
                app.stopStream()
                app.runRemoteCommand("cm4", "pi", "killall libcamera-vid")
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.spacing
        }

        Image {
            id: currentImage
            width: parent.width
            fillMode: Image.PreserveAspectFit

            source: ""

            cache: false

            Connections {
                target: imageProvider
                ignoreUnknownSignals: true

                onSignal_newData: {
                    if(sTitle === "histo")
                    {
                        currentImage.source = ""
                        currentImage.source = "image://imageProvider/" + sTitle;
                    }
                }
            }
        }
    }
}
