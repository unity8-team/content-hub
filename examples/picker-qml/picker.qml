import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

MainView {
    id: root
    applicationName: "picker-qml"
    width: 300
    height: 200

    property list<ContentItem> importItems
    property var activeTransfer

    ContentPeerPicker {
        anchors.fill: parent
        // Type of handler: Source, Destination, or Share
        handler: ContentHandler.Source
        // well know content type
        contentType: ContentType.Pictures

        onPeerSelected: {
            activeTransfer = peer.request();
        }
    }

    // Provides overlay showing another app is being used to complete the request
    // formerly named ContentImportHint
    ContentTransferHint {
        anchors.fill: parent
        activeTransfer: activeTransfer
    }

    Connections {
        target: activeTransfer
        onStateChanged: {
            console.log("StateChanged: " + activeTransfer.state);
            if (activeTransfer.state === ContentTransfer.Charged)
                importItems = activeTransfer.items;
        }
    }
}
