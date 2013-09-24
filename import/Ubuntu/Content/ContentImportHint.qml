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
import Ubuntu.Content 0.1

/*!
    \qmltype ContentImportHint
    \inqmlmodule Ubuntu.Content 0.1
    \brief Component that indicates that a trasnfer is active

    This component shows, that the transfer is currently running, and the source
    application is active. It blocks all input during that time.
    Place this component on top of your view.

    See documentation for \ContentHub to see an example
*/
Item {
    id: root

    /// The current running transfer
    property var activeTransfer

    opacity: internal.isTransferRunning ? 1.0 : 0.0

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.7
    }

    Label {
        anchors.centerIn: parent
        text: i18n.dtr("content-hub", "Importing")
        fontSize: "large"
    }

    MouseArea {
        anchors.fill: parent
        enabled: internal.isTransferRunning
    }

    Behavior on opacity {
        NumberAnimation {
            duration: UbuntuAnimation.FastDuration
            easing: UbuntuAnimation.StandardEasing
        }
    }

    QtObject {
        id: internal
        property bool isTransferRunning: root.activeTransfer ?
                                             root.activeTransfer.state === ContentTransfer.InProgress
                                           : false
    }
}
