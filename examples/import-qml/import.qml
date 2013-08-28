import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

import Ubuntu.Content 0.1

Rectangle {
    width: 300
    height: 200

    property list<ContentItem> importItems

    Button {
        id: importButton
        text: "Import from default"
        onClicked: {
            var peer = ContentHub.defaultSourceForType(ContentType.Pictures);
            var transfer = ContentHub.importContent(ContentType.Pictures, peer);
            transfer.start()
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
        target: ContentHub
        onFinishedImportsChanged: {
            var idx = ContentHub.finishedImports.length - 1;
            importItems = ContentHub.finishedImports[idx].items;
        }
    }
}
