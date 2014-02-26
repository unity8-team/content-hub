/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1

/*!
    \qmltype ContentPeerPicker
    \inqmlmodule Ubuntu.Content 0.1
    \brief Component that allows users to select a source/destination for content transfer

    This component displays a list of applications, devices and services which
    are appropriate for transfering a given content type with.
*/
Item {
    id: root

    property alias contentType: peerModel.contentType
    property alias handler: peerModel.handler
    property var peer

    signal peerSelected

    ContentPeerModel {
        id: peerModel
    }

    Component {
        id: peerDelegate
        Item {
            width: icon.width
            height: icon.height + peerLabel.height
            UbuntuShape {
                id: icon
                image: Image {
                    source: "image://content-hub/" + modelData.appId
                }
                width: 196;
                height: 196;
            }
            Label {
                id: peerLabel
                anchors.top: icon.bottom
                width: icon.width
                text: modelData.name || modelData.appId
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    peer = modelData
                    peerSelected()
                }
            }
        }
    }

    ListItem.Header {
        id: appTitle
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        text: "Applications"
    }


    GridView {
        id: appPeers
        anchors {
            left: parent.left
            right: parent.right
            top: appTitle.bottom
        }
        cellWidth: 196
        cellHeight: 196
        model: peerModel.peers
        delegate: peerDelegate
    }

}
