import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Rectangle {
    id: root
    width: 300
    height: 400

    ContentExportChrome {
        id: chrome

        anchors.fill: parent

        CheckBox {
            id: option1
            anchors.top: parent.top
            anchors.left: parent.left
            text: "file:///home/schwann/Pictures/Scan200.jpg"
        }
        CheckBox {
            id: option2
            anchors.top: parent.top
            anchors.right: parent.right
            text: "file:///picture_2.jpg"
        }

        onExportPressed: {
            var results = [];
            if (option1.checked) {
                var result = resultComponent.createObject(chrome);
                result.url = option1.text;
                results.push(result);
            }
            if (option2.checked) {
                var result = resultComponent.createObject(chrome);
                result.url = option1.text;
                results.push(result);
            }
            activeTransfer.items = results;
        }
    }

    Component {
        id: resultComponent
        ContentItem {}
    }
}
