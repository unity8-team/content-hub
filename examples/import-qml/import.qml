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
    property var peers


    function _importFromPeer(peer) {
        /* if peer is null, choose default */
        if (peer === null)
            peer = ContentHub.defaultSourceForType(ContentType.Pictures);
        var transfer = ContentHub.importContent(ContentType.Pictures, peer);
        var store = ContentHub.defaultStoreForType(ContentType.Pictures);
        console.log("Store is: " + store.uri);
        if (transfer !== null) {
            transfer.selectionType = ContentTransfer.Multiple;
            transfer.setStore(store);
            activeTransfer = transfer;
            activeTransfer.start();
        }
    }

    Component.onCompleted: {
        peers = ContentHub.knownSourcesForType(ContentType.Pictures);
    }

    ListView {
        id: peerList
        anchors {
            left: parent.left
            right: parent.right
            top: importButtons.bottom
        }
        height: childrenRect.height
        model: peers

        delegate: ListItem.Standard {
            text: modelData.name
            control: Button {
                text: "Import"
                onClicked: {
                    _importFromPeer(modelData);
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
                _importFromPeer(null);
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

    ContentImportHint {
        anchors.fill: parent
        activeTransfer: root.activeTransfer
    }

    Connections {
        target: root.activeTransfer
        onStateChanged: {
            console.log("StateChanged: " + activeTransfer.state);
            if (activeTransfer.state === ContentTransfer.Charged)
                importItems = activeTransfer.items;
        }
    }
}
