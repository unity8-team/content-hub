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

ListItem {
    id: clipboardItem

    property string title: ""
    property string summary: ""
    property url imageSource: ""

    signal deleteClicked()
    signal previewClicked()

    height: clipboardItemLayout.height + (divider.visible ? divider.height : 0)

    leadingActions: ListItemActions {
        actions: [
            Action {
                iconName: "delete"
                text: i18n.tr("Delete")
                onTriggered: clipboardItem.deleteClicked()
            }
        ]
    }

    trailingActions: ListItemActions {
        actions: [
            Action {
                iconName: "document-open"
                text: i18n.tr("Preview")
                onTriggered: clipboardItem.previewClicked()
            }
        ]
    }

    ListItemLayout {
        id: clipboardItemLayout

        title.text: {
            if (clipboardItem.title) {
                return clipboardItem.title
            } else if (clipboardItem.imageSource != "") {
                return i18n.tr("Image")
            }
            return ""
        }
        summary.text: clipboardItem.summary

        UbuntuShape {
            width: units.gu(4)
            height: width

            visible: clipboardItem.imageSource != ""

            SlotsLayout.position: SlotsLayout.Leading
            source: Image {
                source: clipboardItem.imageSource
            }
        }
    }
}
