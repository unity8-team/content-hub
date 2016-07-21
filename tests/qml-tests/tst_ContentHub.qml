/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Content 1.3

TestCase {
    name: "ContentHub"

    function test_default_import() {
        var transfer = sourcePeer.request();
        verify(transfer !== null, "No transer Object returned")
    }

    function test_select_for_import() {
        var transfer = sourcePeer.request();
        verify(transfer !== null, "No transer Object returned")
    }

    function test_export_request_pictures() {
        var filePath = "file:///foo/bar.png";
        var transfer = destPeer.request();
        transfer.items = [ resultComponent.createObject(test, {"url": filePath, "name": "test"}) ];
        transfer.state = ContentTransfer.Charged;
        // This shouldn't be necessary, but without it we compare the results to fast
        ContentHub.exportRequested(transfer);
        compare(test.exportTransfer, transfer, "Transfer object not correcty set");
        compare(test.exportTransfer.items[0].url, filePath, "Transfer url incorrect");
        compare(test.exportTransfer.items[0].name, "test", "Transfer name incorrect");
    }

    function test_export_request_text() {
        var transfer = textPeer.request();
        transfer.items = [ resultComponent.createObject(test, {"url": "", "name": "test", "text": "some text"}) ];
        transfer.state = ContentTransfer.Charged;
        // This shouldn't be necessary, but without it we compare the results to fast
        ContentHub.exportRequested(transfer);
        compare(test.exportTransfer, transfer, "Transfer object not correcty set");
        compare(test.exportTransfer.items[0].text, "some text", "Transfer text incorrect");
    }

    function test_export_request_text_and_url() {
        var transfer = textPeer.request();
        transfer.items = [ resultComponent.createObject(test, {"url": "http://www.ubuntu.com", "name": "test", "text": "some text"}) ];
        transfer.state = ContentTransfer.Charged;
        // This shouldn't be necessary, but without it we compare the results to fast
        ContentHub.exportRequested(transfer);
        compare(test.exportTransfer, transfer, "Transfer object not correcty set");
        compare(test.exportTransfer.items[0].text, "some text", "Transfer text incorrect");
        compare(test.exportTransfer.items[0].url, "http://www.ubuntu.com", "Transfer text incorrect");
    }

    Component {
        id: resultComponent
        ContentItem {}
    }

    ContentPeer {
        id: textPeer
        handler: ContentHandler.Destination
        contentType: ContentType.Text
        appId: "com.some.dest"
    }

    ContentPeer {
        id: sourcePeer
        handler: ContentHandler.Source
        contentType: ContentType.Pictures
    }

    ContentPeer {
        id: destPeer
        handler: ContentHandler.Destination
        contentType: ContentType.Pictures
        appId: "com.some.dest"
    }

    Item {
        id: test
        property variant exportTransfer
        property int numImports: 0
        Connections {
            target: ContentHub
            onExportRequested: {
                test.exportTransfer = transfer;
            }
            onFinishedImportsChanged: {
                test.numImports++;
            }
        }
    }
}
