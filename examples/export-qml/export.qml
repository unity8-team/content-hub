import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Rectangle {
    id: root
    width: 300
    height: 400

    ListModel {
        id: urlsModel
        ListElement {
            name: "Photo 1"
            url: "file:///home/phablet/Pictures/img0001.jpg"
            selected: false
        }
        ListElement {
            name: "Photo 2"
            url: "file:///home/phablet/Pictures/img0002.jpg"
            selected: false
        }
        ListElement {
            name: "Photo 3"
            url: "file:///home/phablet/Pictures/img0003.jpg"
            selected: false
        }
        ListElement {
            name: "Photo 4"
            url: "file:///home/phablet/Pictures/img0004.jpg"
            selected: false
        }
        ListElement {
            name: "Photo 5"
            url: "file:///home/phablet/Pictures/img0005.jpg"
            selected: false
        }
    }

    ContentExportChrome {
        id: chrome

        anchors.fill: parent
        selectedItemsCount: 0

        ListView {
            anchors.fill: parent
            model: urlsModel
            delegate: Rectangle {
                height: 40
                width: parent.width
                color: (index % 2) === 0 ? "#aaeebb" : "#99ddaa"
                CheckBox {
                    id: checkBox
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    checked: selected
                    onClicked: {
                        urlsModel.setProperty(index, "selected", checked)
                        if (checked)
                            chrome.selectedItemsCount += 1;
                        else
                            chrome.selectedItemsCount -= 1;
                    }
                }
                Label {
                    anchors.left: checkBox.right
                    anchors.leftMargin: 10
                    anchors.verticalCenter: checkBox.verticalCenter
                    text: name
                }
            }
        }

        onExportPressed: {
            var results = [];
            for (var i=0; i<urlsModel.count; ++i) {
                console.log("urlsModel.get(i).selected : "+urlsModel.get(i).selected)
                if (urlsModel.get(i).selected) {
                    var result = resultComponent.createObject(chrome);
                    result.url = urlsModel.get(i).url;
                    console.log("result.url: "+result.url)
                    results.push(result);
                }
            }
            activeTransfer.items = results;
        }
    }

    Component {
        id: resultComponent
        ContentItem {}
    }
}
