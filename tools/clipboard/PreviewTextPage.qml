/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Arthur Mello <arthur.mello@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: previewTextPage

    property alias text: textPreview.text

    signal pasteClicked()

    header: PageHeader {
        id: pageHeader
        title: i18n.tr("Preview")
        trailingActionBar.actions: [
            Action {
                iconName: "edit-paste"
                text: i18n.tr("Paste")
                onTriggered: previewTextPage.pasteClicked()
            }
        ]
    }

    TextArea {
        id: textPreview

        anchors {
            top: pageHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }

        textFormat: TextEdit.AutoText
        wrapMode: Text.WordWrap
        cursorVisible: false
        readOnly: true
        selectByMouse: false
    }

    Scrollbar {
        flickableItem: textPreview
        align: Qt.AlignTrailing
    }
}
