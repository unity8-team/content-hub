import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Rectangle {
    id: root
    width: 300
    height: 200

    property bool pickMode: false
    property list<ContentItem> selectedItems
    property var activeTransfer

    function __returnResult() {
        activeTransfer.items = selectedItems;
        activeTransfer.state = ContentTransfer.Charged;
    }

    Button {
        id: button1
        anchors.top: parent.top
        anchors.left: parent.left
        enabled: pickMode
        text: "Return URL1"
        onClicked: {
            var result = resultComponent.createObject(root);
            result.url = "file:///picture_1.jpg";
            selectedItems = [ result ];
            root.__returnResult();
        }
    }
    Button {
        id: button2
        anchors.top: parent.top
        anchors.right: parent.right
        enabled: pickMode
        text: "Return Url2"
        onClicked: {
            var results = [];

            var result = resultComponent.createObject(root);
            result.url = "file:///picture_1.jpg";
            results.push(result);

            result = resultComponent.createObject(root);
            result.url = "file:///picture_2.jpg";
            results.push(result);

            selectedItems = results;
            console.log(selectedItems[0].url + "/" + selectedItems[1].url)
            root.__returnResult();
        }
    }

    Button {
        id: buttonAbort
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: pickMode
        text: "Cancel"
        onClicked: {
            root.activeTransfer.state = ContentTransfer.Aborted;
            root.pickMode = false;
        }
    }

    Component {
        id: resultComponent
        ContentItem {}
    }

    Connections {
        target: ContentHub
        onExportRequested: {
            root.activeTransfer = transfer
            root.pickMode = true;
        }
    }
}
