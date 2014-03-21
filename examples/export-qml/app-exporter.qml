import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1

MainView {
    id: root
    applicationName: "com.ubuntu.developer.ken-vandine.hub-exporter"
    width: units.gu(50)
    height: units.gu(60)

    property bool pickMode: activeTransfer.state === ContentTransfer.InProgress
    property var selectedItems: []
    property var activeTransfer

    function __returnResult() {
        activeTransfer.items = selectedItems;
        activeTransfer.state = ContentTransfer.Charged;
    }

    ListModel {
        id: images

        ListElement {
            src: "file:///usr/share/icons/hicolor/128x128/apps/ubuntuone-music.png"
        }

        ListElement {
            src: "file:///usr/share/icons/hicolor/128x128/apps/ubuntuone-music.png"
        }

        ListElement {
            src: "file:///usr/share/icons/hicolor/128x128/apps/ubuntuone-music.png"
        }

        ListElement {
            src: "file:///usr/share/icons/hicolor/128x128/apps/ubuntuone-music.png"
        }
    }

    GridView {
        anchors.fill: parent
        model: images
        cellWidth: 128
        cellHeight: 128
        delegate: itemDelegate
    }

    Component {
        id: resultComponent
        ContentItem {}
    }

    Component {
        id: itemDelegate
        Item {
            width: 128
            height: 128

            property bool isSelected: false

            UbuntuShape {
                width: parent.width
                height: width
                image: Image {
                    id: image
                    source: src
                    height: parent.width
                    width: height
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: pickMode
                    onClicked: {
                        var shouldAdd = true;
                        for (var i = 0; i < selectedItems.length; i++)
                        {
                            console.log("item: ", selectedItems[i]);
                            if (selectedItems[i] === src)
                            {
                                selectedItems.pop(i);
                                shouldAdd = false;
                                isSelected = false;
                            }
                        }
                        if (shouldAdd)
                        {
                            selectedItems.push(src);
                            isSelected = true;
                        }
                    }
                }

                Image {
                    id: selectionTick
                    anchors.right: parent.right
                    anchors.top: parent.top
                    width: units.gu(5)
                    height: units.gu(5)
                    visible: isSelected
                    source: "photo-preview-selected-overlay.png"
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: !pickMode
                    onClicked: {
                        actPop.show();
                    }
                }

                ActionSelectionPopover {
                    id: actPop
                    delegate: ListItem.Standard {
                      text: action.text
                    }

                    contentWidth: childrenRect.width

                    actions: ActionList {
                      Action {
                          text: "Open with..."
                          onTriggered: {
                              print(text + ": " + src);
                              activeTransfer = ContentHub.exportContent(ContentType.Pictures, picDest);
                              activeTransfer.items = [ resultComponent.createObject(root, {"url": src}) ];
                              actPop.hide();
                          }
                      }
                      Action {
                          text: "Share"
                          onTriggered: {
                              print(text + ": " + src);
                              activeTransfer = ContentHub.shareContent(ContentType.Pictures, picShare);
                              activeTransfer.items = [ resultComponent.createObject(root, {"url": src}) ];
                              actPop.hide();
                          }
                      }
                    }
                }
            }
        }
    }

    ContentPeer {
        id: picDest
        appId: "com.ubuntu.developer.ken-vandine.hub-importer_hub-importer_0.2"
    }

    ContentPeer {
        id: picShare
        appId: "com.ubuntu.developer.ken-vandine.hub-share_hub-share_0.2"
    }

    ContentImportHint {
        anchors.fill: parent
        activeTransfer: activeTransfer
    }

    Connections {
        target: ContentHub
        onExportRequested: {
            activeTransfer = transfer
        }
    }

    ListItem.Empty {
        id: pickerButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: units.gu(2)
        }
        visible: pickMode
        Button {
            anchors {
                left: parent.left
                bottom: parent.bottom
                margins: units.gu(2)
            }
            text: "Cancel"
            onClicked: activeTransfer.state = ContentTransfer.Aborted;
        }

        Button {
            anchors {
                right: parent.right
                bottom: parent.bottom
                margins: units.gu(2)
            }
            text: "Select"
            onClicked: {
                var results = [];
                for (var i = 0; i < selectedItems.length; i++)
                    results.push(resultComponent.createObject(root, {"url": selectedItems[i]}));

                if (results.length > 0)
                    activeTransfer.items = results;
            }
        }
    }
}
