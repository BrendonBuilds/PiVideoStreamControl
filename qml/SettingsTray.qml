import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtGraphicalEffects 1.0

Item {
    id: settingsTray

    width: 300 / mainWindow.dWindowScaling
    height: settingsColumn.childrenRect.height

    property bool bHidden: false
    property double leftMargin: bHidden ? (-1 * settingsTray.width) : 0

    property bool bRunning: false

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
        spacing: 5
        property int iSpacing: 2
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
            text: "Device Username"
            color: colorConfig.textColorLight
            font.italic: true
        }

        TextField {
            id: usernameEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "pi"
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "Device IP address"
            color: colorConfig.textColorLight
            font.italic: true
        }

        TextField {
            id: ipAddrEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "cm4"
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "Resolution"
            color: colorConfig.textColorLight
            font.italic: true
        }

        Item {
            width: settingsColumn.itemWidth
            height: widthEntry.height

            anchors.horizontalCenter: parent.horizontalCenter

            TextField {
                id: widthEntry
                width: settingsColumn.itemWidth * 0.45
                height: settingsColumn.spacing * 5
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "1456"
            }

            Text {
                anchors.centerIn: parent
                text: "x"
                color: colorConfig.textColorLight
            }

            TextField {
                id: heightEntry
                width: settingsColumn.itemWidth * 0.45
                height: settingsColumn.spacing * 5
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: "1088"
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "Framerate"
            color: colorConfig.textColorLight
            font.italic: true
        }

        TextField {
            id: framerateEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "60"
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

        Text {
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.05
            text: "White Balance"
            color: colorConfig.textColorLight
            font.italic: true
        }

        ComboBox {
            id: awbEntry
            width: settingsColumn.itemWidth
            height: settingsColumn.spacing * 5
            anchors.horizontalCenter: parent.horizontalCenter
            model: ["auto", "incandescent", "tungsten", "fluorescent", "indoor", "daylight", "cloudy", "custom"]

            currentIndex: 4
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
                onCheckedChanged: {
                    imageProvider.setOutputOptions(overlayCheckbox.checked, histogramCheckbox.checked)
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

        Item {
            height: mainWindow.height * 0.035
            width: settingsColumn.itemWidth
            anchors.horizontalCenter: parent.horizontalCenter

            CheckBox {
                id: histogramCheckbox
                anchors.left: parent.left
                height: parent.height
                onCheckedChanged: {
                    imageProvider.setOutputOptions(overlayCheckbox.checked, histogramCheckbox.checked)
                }
            }

            Text {
                height: parent.height
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight

                color: colorConfig.textColorLight
                text: "Histogram"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    histogramCheckbox.checked = !histogramCheckbox.checked
                }
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: !settingsTray.bRunning

            text: "Start Camera Stream"

            onClicked: {
                settingsTray.bRunning = true
                app.runRemoteCommand(ipAddrEntry.text, usernameEntry.text, "libcamera-vid --camera 0 -v 0 -t 0 --width " + widthEntry.text + " --height " + heightEntry.text + " --awb " + awbEntry.currentText + " --inline --listen -o tcp://0.0.0.0:8888 --framerate " + framerateEntry.text + " --shutter " + shutterTimeEntry.text + " --gain " + gainEntry.text)
                app.connectToStream(ipAddrEntry.text)
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.iSpacing
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: settingsTray.bRunning

            text: "Stop Camera Stream"

            onClicked: {
                settingsTray.bRunning = false
                app.stopStream()
                app.runRemoteCommand(ipAddrEntry.text, usernameEntry.text, "killall libcamera-vid")
            }
        }

        Item {
            width: parent.width
            height: settingsColumn.spacing
        }

        Rectangle {
            width: parent.width
            height: histogramCheckbox.checked ? (settingsTrayTitle.height * 3) : 0

            visible: histogramCheckbox.checked

            color: "black"

            Image {
                id: histogramImage
                anchors.fill: parent
                fillMode: Image.Stretch

                source: ""
                cache: false

                Connections {
                    target: imageProvider
                    ignoreUnknownSignals: true

                    onSignal_newData: {
                        if(sTitle === "histo")
                        {
                            histogramImage.source = ""
                            histogramImage.source = "image://imageProvider/" + sTitle;
                        }
                    }
                }
            }
        }
    }
}
