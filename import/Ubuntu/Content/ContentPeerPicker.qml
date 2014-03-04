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
StyledItem {
    id: root
    anchors.fill: parent
    visible: false
    property alias contentType: peerModel.contentType
    property alias handler: peerModel.handler
    property alias showTitle: header.visible
    property var peer

    signal peerSelected

    Header {
        id: header
        title: (handler === ContentHandler.Source) ? i18n.tr("Choose from") : i18n.tr("Share to")
    }

    ContentPeerModel {
        id: peerModel
    }

    Component {
        id: peerDelegate
        Item {
            width: units.gu(14)
            height: units.gu(16)
            Item {
                width: icon.width
                height: icon.height + peerLabel.height
                anchors.centerIn: parent
                UbuntuShape {
                    id: icon
                    image: Image {
                        source: "image://content-hub/" + modelData.appId
                    }
                    width: units.gu(12);
                    height: units.gu(12);
                }
                Label {
                    id: peerLabel
                    anchors.top: icon.bottom
                    width: icon.width
                    elide: Text.ElideRight
                    font.weight: Font.Bold
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
    }

    ListItem.Header {
        id: appTitle
        anchors.top: header.visible ? header.bottom : parent.top
        text: "Apps"
    }

    Rectangle {
        id: apps
        color: "#FFFFFF"
        height: (parent.height / 2.4)
        width: parent.width
        radius: 0
        anchors {
            left: parent.left
            right: parent.right
            top: appTitle.bottom
        }

        Flickable {
            anchors.fill: parent

            GridView {
                id: appPeers
                cellWidth: units.gu(14)
                cellHeight: units.gu(16)
                model: peerModel.peers
                delegate: peerDelegate
            }

        }
    }

    ListItem.Header {
        id: devTitle
        anchors {
            left: parent.left
            right: parent.right
            top: apps.bottom
        }
        text: "Devices"
    }

    Rectangle {
        id: devices
        color: "#FFFFFF"
        height: (parent.height - devTitle.y - devTitle.height - units.gu(1))
        width: parent.width
        radius: 0
        anchors {
            left: parent.left
            right: parent.right
            top: devTitle.bottom
        }

        Flickable {
            anchors.fill: parent

            GridView {
                id: devPeers
                cellWidth: units.gu(14)
                cellHeight: units.gu(16)
                delegate: peerDelegate
            }

        }
    }


}
