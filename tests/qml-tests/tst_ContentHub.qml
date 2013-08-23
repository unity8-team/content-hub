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
import Ubuntu.Content 0.1

TestCase {
    name: "ContentHub"

    function test_default_import() {
        var transfer = ContentHub.importContent(ContentType.Pictures)
        verify(transfer !== null, "No transer Object returned")
    }

    function test_select_for_import() {
        var peer = ContentHub.defaultSourceForType(ContentType.Pictures)
//        compare(peer.name, "com_ubuntu_gallery_app", "Wrong peer as default")
        var transfer = ContentHub.importContent(ContentType.Pictures, peer)
        verify(transfer !== null, "No transer Object returned")
    }

    function test_export_request() {
        var transfer = ContentHub.importContent(ContentType.Pictures)
        ContentHub.exportRequested(transfer)
        compare(test.exportTransfer, transfer, "Transfer object not correcty copied")
    }

    Item {
        id: test
        property variant exportTransfer
        Connections {
            target: ContentHub
            onExportRequested: {
                test.exportTransfer = transfer
            }
        }
    }
}
