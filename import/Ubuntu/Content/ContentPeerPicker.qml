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
    are appropriate for transferring a given content type with.
*/
Item {
    id: root
    anchors.fill: parent
    visible: false

    /*!
        \qmlproperty ContentHandler handler
        \brief The ContentHandler to use when finding peers.
    */
    property var handler

    /*! \qmlproperty ContentType contentType
        \brief The ContentType to use when finding peers.
    */
    property var contentType

    /*! \qmlproperty bool showTitle
        \brief Determines whether the header should be displayed.

        This makes it possible to hide the header, which can be useful
        if embedding the picker within another page or popup.
    */
    property alias showTitle: header.visible

    /*! \qmlproperty ContentPeer peer
        \brief The peer selected by the user.

        Once the peerSelected signal has been sent, this provides the
        ContentPeer which the user has selected.
    */
    property var peer

    /*! \qmlproperty Loader customerPeerModelLoader
        \brief A Loader containing a ContentPeerModel.

        This can optionally be used to provide a pre-populated ContentPeerModel
        to this ContentPeerPicker.
    */
    property var customPeerModelLoader

    property var completed: false

    /*! \qmlsignal peerSelected
        \brief Emitted when a user selects a peer.

        Once this signal has been emitted the selected peer can be accessed via
        the peer property.
        \c onPeerSelected
    */
    signal peerSelected

    /*! \qmlsignal cancelPressed
        \brief Emitted when the user clicks the cancel button.
        
        The ContentPeerPicker will be hidden automatically when the user cancels
        the operations and the active ContentTransfer will be set to Aborted.
        \c onCancelPressed
    */
    signal cancelPressed

    Header {
        id: header
        title: (handler === ContentHandler.Source) ? i18n.tr("Choose from") : (handler === ContentHandler.Destination ? i18n.tr("Export to") : i18n.tr("Share to"))
    }

    Loader {
        id: peerModelLoader
        active: false
        sourceComponent: ContentPeerModel { }
        onLoaded: {
            item.handler = root.handler;
            item.contentType = root.contentType;
        }
    }

    Component.onCompleted: {
        if (root.visible) {
            if (customPeerModelLoader) {
                customPeerModelLoader.active = true;
            } else {
                peerModelLoader.active = true;
            }
        }
        completed = true;
    }

    onVisibleChanged: {
        if (completed) {
            if (customPeerModelLoader) {
                customPeerModelLoader.active = true;
            } else {
                peerModelLoader.active = true;
            }
        }
    }

    onHandlerChanged: {
        if (!customPeerModelLoader && peerModelLoader.item) {
            appPeers.model = undefined; // Clear grid view
            peerModelLoader.item.handler = root.handler;
            appPeers.model = peerModelLoader.item.peers;
        }
    }

    onContentTypeChanged: {
        if (!customPeerModelLoader && peerModelLoader.item) {
            appPeers.model = undefined; // Clear grid view
            peerModelLoader.item.contentType = root.contentType;
            appPeers.model = peerModelLoader.item.peers;
        }
    }

    Component {
        id: peerDelegate
        Item {
            width: units.gu(13.5)
            height: units.gu(16)
            AbstractButton {
                width: parent.width
                height: icon.height + label.height
                UbuntuShape {
                    id: icon
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                    radius: "medium"
                    width: units.gu(8)
                    height: units.gu(7.5)
                    image: Image {
                        id: image
                        objectName: "image"
                        sourceSize { width: icon.width; height: icon.height }
                        asynchronous: true
                        cache: false
                        source: "image://content-hub/" + modelData.appId
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter
                    }
               }

                Label {
                    id: label
                    objectName: "label"
                    anchors {
                        baseline: icon.bottom
                        baselineOffset: units.gu(2)
                        left: parent.left
                        right: parent.right
                        leftMargin: units.gu(1)
                        rightMargin: units.gu(1)
                    }

                    opacity: 0.9
                    fontSize: "small"
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignHCenter
                    text: modelData.name || modelData.appId
                }

                onClicked: {
                        peer = modelData
                        peerSelected()
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
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: appTitle.bottom
        }

        Flickable {
            anchors.fill: parent

            ResponsiveGridView {
                id: appPeers
                anchors.fill: parent
                minimumHorizontalSpacing: units.gu(0.5)
                maximumNumberOfColumns: 6
                delegateWidth: units.gu(11)
                delegateHeight: units.gu(9.5)
                verticalSpacing: units.gu(2)
                model: customPeerModelLoader ? customPeerModelLoader.item.peers : peerModelLoader.item.peers
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
        width: parent.width
        radius: 0
        anchors {
            left: parent.left
            right: parent.right
            top: devTitle.bottom
            bottom: cancelButton.top
            bottomMargin: units.gu(1)
        }

        Flickable {
            anchors.fill: parent

            GridView {
                id: devPeers
                header: Item { height: units.gu(2) }
                cellWidth: units.gu(13.5)
                cellHeight: units.gu(16)
                delegate: peerDelegate
            }

        }
    }

    Button {
        id: cancelButton
        text: "Cancel"
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: units.gu(1)
        }
        onClicked: {
            if(root.activeTransfer) {
                root.activeTransfer.state = ContentTransfer.Aborted;
            }
            cancelPressed();
        }
    }

}
