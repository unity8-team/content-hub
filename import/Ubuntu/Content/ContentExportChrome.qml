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
    \qmltype ContentExportChrome
    \inqmlmodule Ubuntu.Content 0.1
    \brief TODO add documentation

    Example:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1
        import Ubuntu.Content 0.1

        Rectangle {
            width: 300
            height: 400

            ContentExportChrome {
                id: chrome
                anchors.fill: parent

                MySelectionComponent {
                    anchors.fill: parent
                    // set chrome.activeTransfer.items properly when the user selects items
                    // or set the chrome.selectedItemsCount manually

                }
            }
        }
    \endqml
*/
Item {
    id: root

    /// The current running transfer, is undefined when no transfer is running
    property var activeTransfer
    /// The number of selected items.
    /// Is calculcated from activeTransfer.items, if not set manually
    property int selectedItemsCount: activeTransfer ? activeTransfer.items.length : 0
    /// If set to totrue and amultiSelect mode is used, a button to select all / none
    // is shown. The functionality needs to be hooked up using the selectAllToggled()
    /// signal
    property bool showSelectAllButton: false

    /// This signal is emitted when the user presses the cancel button
    /// The transfer is aborted automaically
    signal exportCanceled()
    /// This signal is emitted when the user presses the import/upload button
    /// The transfer is set to charged automatically. But the ctiveTranser.items
    /// needs to be filled
    signal exportPressed()
    /// Is emitted, when the user presses the button to select all/none
    signal selectAllToggled()

    /// To not be used
    // Used to put all Items to the children of the container
    // So "anchors.fill: parent" can be used by the Item(s), but the chrome is
    // still at the bottom, and the Item's sie is adapted accordingly
    default property alias content: container.children

    Item {
        id: container
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomChrome.top
        }
    }

    Rectangle {
        id: bottomChrome

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        // TODO get proper height without binding loop
        height: selectFeedback.height + cancelButton.height + internal.margin * 3
        color: UbuntuColors.warmGrey

        Label {
            id: selectFeedback
            anchors {
                left: parent.left
                leftMargin: internal.margin
                right: parent.right
                bottom: cancelButton.top
                bottomMargin: internal.margin
            }
            text: i18n.dtr("content-hub", "%1 item selected", "%1 items selected",
                           root.selectedItemsCount).arg(root.selectedItemsCount)
        }

        Button {
            id: cancelButton
            anchors {
                left: parent.left
                leftMargin: internal.margin
                bottom: parent.bottom
                bottomMargin: internal.margin
            }

            text: i18n.dtr("content-hub", "Cancel")
            enabled: internal.transferActive
            onClicked: {
                root.exportCanceled()
                if (root.activeTransfer.state === ContentTransfer.InProgress)
                    root.activeTransfer.state = ContentTransfer.Aborted;
            }
        }

        Button {
            id: selectAllButton
            anchors {
                right: okButton.left
                rightMargin: internal.margin
                bottom: parent.bottom
                bottomMargin: internal.margin
            }

            text: i18n.dtr("content-hub", "All/None")
            visible: internal.multiSelect && root.showSelectAllButton
            onClicked: {
                root.selectAllToggled
            }
        }

        Button {
            id: okButton
            anchors {
                right: parent.right
                rightMargin: internal.margin
                bottom: parent.bottom
                bottomMargin: internal.margin
            }

            text: i18n.dtr("content-hub", "Import") // TODO set text according to context of the transfer (upload/import/...)
            enabled: internal.transferActive && root.selectedItemsCount > 0
            onClicked: {
                root.exportPressed();
                if (root.activeTransfer.state === ContentTransfer.InProgress)
                    root.activeTransfer.state = ContentTransfer.Charged;
            }
        }

        Item {
            id: internal

            visible: false

            property bool transferActive: root.activeTransfer ?
                                              root.activeTransfer.state === ContentTransfer.InProgress
                                            : false
            property bool multiSelect: root.activeTransfer ?
                                           root.activeTransfer.selectionType === ContentTransfer.Multiple
                                         : false
            property int margin: units.gu(0.5)
            Connections {
                target: ContentHub
                onExportRequested: {
                    root.activeTransfer = transfer;
                }
            }
        }
    }
}
