import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

MainView {
    id: root
    width: 300
    height: 200

    property bool pickMode: activeTransfer.state === ContentTransfer.InProgress
    property list<ContentItem> selectedItems
    property var activeTransfer

    Button {
        id: button1
        anchors.top: parent.top
        anchors.left: parent.left
        enabled: pickMode
        text: "Return URL1"
        onClicked: {
            selectedItems = [ resultComponent.createObject(root, {"url": "file:///picture_1.jpg"}) ];
            activeTransfer.items = selectedItems;
        }
    }

    Button {
        id: button2
        anchors.top: parent.top
        anchors.right: parent.right
        enabled: pickMode
        text: "Return Url2"
        onClicked: {
            selectedItems.push(resultComponent.createObject(root, {"url": "file:///picture_1.jpg"}));
            selectedItems.push(resultComponent.createObject(root, {"url": "file:///picture_2.jpg"}));
            console.log(selectedItems[0].url + "/" + selectedItems[1].url)
            activeTransfer.items = selectedItems;        }
        }
    }

    Button {
        id: buttonAbort
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: pickMode
        text: "Cancel"
        onClicked: {
            activeTransfer.state = ContentTransfer.Aborted;
        }
    }

    Component {
        id: resultComponent
        ContentItem {}
    }

    Connections {
        target: ContentHub
        onExportRequested: {
            activeTransfer = transfer
        }
    }
}
