import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import QtGraphicalEffects 1.15

Window {
    id: mainWindow

    visible: true

    // account for windows scaling for high res screens
    property double dWindowScaling: 1.25

    width: 1456 / dWindowScaling
    height: 1088 / dWindowScaling

    title: qsTr("Stream Viewer")

    ColorConfig {
        id: colorConfig
    }

    color: colorConfig.backgroundColor

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: mainWindow.width
        contentHeight: mainWindow.height

        boundsBehavior: Flickable.DragOverBounds

        PinchArea {
            id: pinchArea
            width: Math.max(flick.contentWidth, flick.width)
            height: Math.max(flick.contentHeight, flick.height)

            property real maxZoom: 3
            property real initialWidth
            property real initialHeight

            onPinchStarted: {
                initialWidth = flick.contentWidth
                initialHeight = flick.contentHeight
            }

            onPinchUpdated: {

                // adjust content pos due to drag
                flick.contentX += pinch.previousCenter.x - pinch.center.x
                flick.contentY += pinch.previousCenter.y - pinch.center.y

                if(((initialWidth * pinch.scale) >= mainWindow.width)) {
                    if((initialWidth * pinch.scale) <= (mainWindow.width * maxZoom)) {
                        // resize content
                        flick.resizeContent(initialWidth * pinch.scale, initialHeight * pinch.scale, pinch.center)
                    } else {
                        flick.resizeContent(mainWindow.width * maxZoom, mainWindow.height * maxZoom, pinch.center)
                    }
                } else {
                    flick.resizeContent(mainWindow.width, mainWindow.height, pinch.center)
                }
            }

            onPinchFinished: {
                flick.returnToBounds()
            }

            Item {
                width: flick.contentWidth
                height: flick.contentHeight

                Image {
                    id: currentFrame
                    height: parent.height
                    width: parent.width

                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit

                    source: ""

                    cache: false

                    Connections {
                        target: imageProvider
                        ignoreUnknownSignals: true

                        onSignal_newData: {
                            if(sTitle === "original")
                            {
                                currentFrame.source = ""
                                currentFrame.source = "image://imageProvider/" + sTitle;
                            }
                        }
                    }
                }

                GaussianBlur {
                    cached: false
                    source: currentFrame
                    anchors.fill: parent
                    radius: 60
                    samples: 90
                    transparentBorder: false
                    visible: settingsTray.overlayChecked
                }

                Rectangle {
                    anchors.fill: currentFrame
                    color: "black"
                    opacity: 0.65
                    visible: settingsTray.overlayChecked
                }

                Image {
                    id: currentFrameFocus
                    height: parent.height
                    width: parent.width

                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit

                    source: ""
                    visible: settingsTray.overlayChecked

                    cache: false

                    Connections {
                        target: imageProvider
                        ignoreUnknownSignals: true

                        onSignal_newData: {
                            if(sTitle === "focusOverlay")
                            {
                                currentFrameFocus.source = ""
                                currentFrameFocus.source = "image://imageProvider/" + sTitle;
                            }
                        }
                    }
                }

                MouseArea {
                    id: mouse
                    anchors.fill: parent

                    hoverEnabled: true

                    onDoubleClicked: {
                        // if we are already zoomed, zoom out
                        if(flick.contentWidth > mainWindow.width) {
                            // reset the zoom
                            flick.contentWidth = mainWindow.width
                            flick.contentHeight = mainWindow.height
                            flick.contentX = 0;
                            flick.contentY = 0;
                        } else {
                            flick.resizeContent(mainWindow.width * pinchArea.maxZoom, mainWindow.height * pinchArea.maxZoom, Qt.point(mouse.x, mouse.y))
                        }
                    }

                    onWheel: {
                        if (wheel.angleDelta.y/120*flick.contentWidth*0.1+flick.contentWidth > flick.width && wheel.angleDelta.y/120*flick.contentHeight*0.1+flick.contentHeight > flick.height)
                        {
                            flick.resizeContent(wheel.angleDelta.y/120*flick.contentWidth*0.1+flick.contentWidth, wheel.angleDelta.y/120*flick.contentHeight*0.1+flick.contentHeight, Qt.point(mouse.mouseX, mouse.mouseY))
                            flick.returnToBounds()
                        }
                        else {
                            flick.resizeContent(flick.width, flick.height, Qt.point(mouse.x, mouse.y))
                            flick.returnToBounds()
                        }
                    }
                }
            }
        }
    }

    Text {
        anchors.right: parent.right
        anchors.top: parent.top
        color: colorConfig.textColorLight

        text: imageProvider.status
    }

    SettingsTray {
        id: settingsTray
        anchors.left: parent.left
        anchors.top: parent.top
    }
}
