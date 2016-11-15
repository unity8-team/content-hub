import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

import Ubuntu.Content 0.1

MainView {
    id: mainView
    applicationName: "picker-qml"

    width: units.gu(100)
    height: units.gu(75)

    PageStack {
        id: pageStack
        Component.onCompleted: pageStack.push(root)

        Page {
            id: root
            title: i18n.tr("Peer Picker Example")
            visible: false

            property list<ContentItem> importItems
            property var activeTransfer
            property list<ContentPeer> peers

            Column {
                anchors.fill: parent

                ListItem.Standard {
                    id: peerListHeader
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: i18n.tr("Sources")
                    control: Button {
                        text: i18n.tr("Select source")
                        onClicked: {
                            ContentHub.requestPeerForType(ContentType.Pictures, ContentHandler.Source);
                        }
                    }
                }

                ListItem.Header {
                    id: titleItem
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: i18n.tr("Results")
                }

                GridView {
                    id: resultList
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: childrenRect.height
                    cellWidth: units.gu(20)
                    cellHeight: cellWidth

                    model: root.importItems
                    delegate: Item {
                        id: result
                        height: units.gu(19)
                        width: height
                        UbuntuShape {
                            width: parent.width
                            height: width
                            image: Image {
                                id: image
                                source: url
                                sourceSize.width: width
                                sourceSize.height: height
                                height: parent.height
                                width: height
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                            }
                        }
                    }
                }
            }

            ContentTransferHint {
                anchors.fill: root
                activeTransfer: root.activeTransfer
            }

            Connections {
                target: root.activeTransfer
                onStateChanged: {
                    console.log("StateChanged: " + root.activeTransfer.state);
                    if (root.activeTransfer.state === ContentTransfer.Charged)
                        root.importItems = root.activeTransfer.items;
                }
            }

        }

        Page {
            id: picker
            visible: false

            Connections {
                target: ContentHub
                onPeerSelected: {
                    console.debug("onPeerSelected: " + peer);
                    root.activeTransfer = peer.request();
                }
            }
        }
    }
}
