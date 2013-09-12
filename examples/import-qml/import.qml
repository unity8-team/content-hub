import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

import Ubuntu.Content 0.1

Rectangle {
    id: root
    width: 300
    height: 200

    property list<ContentItem> importItems
    property var activeTransfer

    Button {
        id: importButton
        text: "Import from default"
        onClicked: {
            var peer = ContentHub.defaultSourceForType(ContentType.Pictures);
            var transfer = ContentHub.importContent(ContentType.Pictures, peer);
            if (transfer !== null) {
                transfer.selectionType = ContentTransfer.Multiple;
                activeTransfer = transfer;
                activeTransfer.start();
            }
        }
    }

    ListView {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: importButton.bottom
        }

        model: importItems
        delegate: Empty {
                id: result
                height: 128
                UbuntuShape {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: image.width
                    height: image.height
                    image: Image {
                        id: image
                        source: url
                        height: result.height
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }
                }
            }
    }

    Connections {
        target: activeTransfer
        onStateChanged: {
            if (root.activeTransfer.state === ContentTransfer.Charged) {
                importItems = root.activeTransfer.items;
                console.log("root.activeTransfer.items.length: " + root.activeTransfer.items.length)
                console.log("root.activeTransfer.items: " + root.activeTransfer.items)
                console.log("First Item URL: " + root.activeTransfer.items[0].url)
            }
        }
    }
}
