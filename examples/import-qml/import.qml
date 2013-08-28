import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Rectangle {
    width: 300
    height: 200

    property list<ContentItem> importItems
    property var activeTransfer

    Button {
        text: "Import from default"
        onClicked: {
            var peer = ContentHub.defaultSourceForType(ContentType.Pictures);
            var transfer = ContentHub.importContent(ContentType.Pictures, peer);
            if (transfer !== null) {
                activeTransfer = transfer
                transfer.start()
            }
        }
    }
    Label {
        id: result
        anchors.bottom: parent.bottom
        text: importItems.length > 0 ? importItems[0].url : "Nothing imported yet"
    }

    Connections {
        target: activeTransfer
        onStateChanged: {
            if (activeTransfer.state === ContentTransfer.Charged)
                importItems = activeTransfer.items;
        }
    }
}
