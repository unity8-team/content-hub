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
        slectedItemsCount: 0

        CheckBox {
            id: option1
            anchors.top: parent.top
            anchors.left: parent.left
            text: "file:///picture_1.jpg"
            onCheckedChanged: {
                if (checked)
                    chrome.slectedItemsCount += 1;
                else
                    chrome.slectedItemsCount -= 1;
            }
        }
        CheckBox {
            id: option2
            anchors.top: parent.top
            anchors.right: parent.right
            text: "file:///picture_2.jpg"
            onCheckedChanged: {
                if (checked)
                    chrome.slectedItemsCount += 1;
                else
                    chrome.slectedItemsCount -= 1;
            }
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
