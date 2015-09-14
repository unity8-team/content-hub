 /*
 * Copyright 2015 Canonical Ltd.
 *
 * This file is part of content-hub.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

/*
 * Component to use as page header in the ContentPeerPicker
 *
 * It has a cancel() signal fired when cancel button is pressed and a title
 * property to set the page title
 */

Column {
    id: root
    signal cancel()
    property string title

    height: childrenRect.height

    anchors {
        left: parent.left
        right: parent.right
    }

    Rectangle {
        id: title

        height: units.gu(7) - divider.height
        anchors { left: parent.left; right: parent.right }

        Rectangle {
            anchors.fill: parent
            //color: "#f6f6f6"
            color: Theme.palette.normal.background
        }

        AbstractButton {
            id: cancelButton
            objectName: "cancelButton"

            width: height

            onTriggered: root.cancel()
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
            }

            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: units.gu(1)
                anchors.rightMargin: units.gu(1)
                color: "#E6E6E6"
                visible: parent.pressed
            }

            Icon {
                name: "close"
                anchors {
                    fill: parent
                    topMargin: units.gu(2)
                    bottomMargin: units.gu(2)
                }
            }
        }

        Label {
            anchors {
                left: cancelButton.right
                verticalCenter: parent.verticalCenter
            }
            text: root.title
            fontSize: 'x-large'
        }
    }

    ListItem.Divider {
        id: divider
    }
}
