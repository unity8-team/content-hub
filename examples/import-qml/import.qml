import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1

MainView {
    id: root
    applicationName: "import-qml"
    width: 300
    height: 200

    property list<ContentItem> importItems
    property var activeTransfer


    ContentPeer {
        id: picSource
        // well know content type
        contentType: ContentType.Pictures
        // Type of handler: Source, Destination, or Share
        handler: ContentHandler.Source
        // Optional appId, if this isn't specified the hub will use the default
        //appId: ""
        // Optional store to use for persistent storage of content
        store: ContentStore {
            scope: ContentScope.App
        }
    }

    // Provides a list<ContentPeer> suitable for use as a model
    ContentPeerModel {
        id: picSources
        // Type of handler: Source, Destination, or Share
        handler: ContentHandler.Source
        // well know content type
        contentType: ContentType.Pictures
    }

    ListView {
        id: peerList
        anchors {
            left: parent.left
            right: parent.right
            top: importButtons.bottom
        }
        height: childrenRect.height
        model: picSources.peers

        delegate: ListItem.Standard {
            text: modelData.name
            control: Button {
                text: "Import"
                onClicked: {
                    // Request the transfer, it needs to be created and dispatched from the hub
                    activeTransfer = modelData.request();
                }
            }
        }
    }

    ListItem.Empty {
        id: importButtons
        Button {
            anchors {
                left: parent.left
                margins: units.gu(2)
            }
            text: "Import from default"
            onClicked: {
                // Request the transfer, it needs to be created and dispatched from the hub
                activeTransfer = picSource.request();
            }
        }

        Button {
            anchors {
                right: parent.right
                margins: units.gu(2)
            }
            text: "Finalize import"
            enabled: activeTransfer.state === ContentTransfer.Collected
            onClicked: activeTransfer.finalize()
        }
    }

    ListView {
        id: resultList
        anchors {
            left: parent.left
            right: parent.right
            top: peerList.bottom
        }
        height: childrenRect.height

        model: importItems
        delegate: ListItem.Empty {
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
