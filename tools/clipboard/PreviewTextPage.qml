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
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Web 0.2
import com.canonical.Oxide 1.15
import clipboardapp.private 0.1

Page {
    id: previewTextPage

    property alias text: textPreview.text
    property string richText: ""
    property int outputOption

    onRichTextChanged: {
        if (outputOption == PasteDataModel.RichText) {
            webView.loadHtml(previewTextPage.richText)
        }
    }

    onOutputOptionChanged: {
        if (outputOption == PasteDataModel.RichText) {
            webView.loadHtml(previewTextPage.richText)
        }
    }

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

    WebView {
        id: webView
        anchors {
            top: optionsLoader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }

        visible: outputOption == PasteDataModel.RichText

        onNavigationRequested: request.action = NavigationRequest.ActionReject
    }

    Flickable {
        id: flickable
        anchors {
            top: optionsLoader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }

        visible: outputOption == PasteDataModel.PlainText

        TextArea {
            id: textPreview

            anchors.fill: parent

            textFormat: TextEdit.PlainText
            wrapMode: Text.WordWrap
            cursorVisible: false
            readOnly: true
            selectByMouse: false
        }
    }

    Loader {
        id: optionsLoader
        anchors {
            top: pageHeader.bottom
            left: parent.left
            right: parent.right
        }

        height: status == Loader.Ready ? item.height : 0

        active: previewTextPage.active && previewTextPage.richText != ""
        sourceComponent: optionsComp
    }

    Component {
        id: optionsComp
        Item {
            anchors {
                top: parent ? parent.top : undefined
                left: parent ? parent.left : undefined
                right: parent ? parent.right : undefined
            }

            height: optionsLabelItem.height + optionsDivider.height + units.gu(4)

            Item {
                id: optionsLabelItem

                anchors {
                    top: parent.top
                    left: parent.left
                    margins: units.gu(2)
                }

                width: optionsLabel.width
                height: optionSelector.itemHeight
        
                Label {
                    id: optionsLabel

                    anchors.centerIn: parent
                    text: i18n.tr("Paste options:")
                }
            }

            ListItem.ThinDivider {
                id: optionsDivider

                anchors {
                    top: optionsLabelItem.bottom
                    topMargin: units.gu(2)
                }

                width: parent.width
                height: units.gu(0.5)
            }

            OptionSelector {
                id: optionSelector

                Component.onCompleted: selectedIndex = outputOption == PasteDataModel.PlainText ? 0 : 1

                anchors {
                    top: parent.top
                    left: optionsLabelItem.right
                    right: parent.right
                    margins: units.gu(2)
                }

                expanded: false
                model: [ i18n.tr("Plain text"),
                         i18n.tr("Rich text")]

                onSelectedIndexChanged: {
                    if (selectedIndex == 0) {
                        outputOption = PasteDataModel.PlainText
                    } else {
                        outputOption = PasteDataModel.RichText
                    }
                }
            }
        }
    }
}
