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
import Ubuntu.Content 1.3
import clipboardapp.private 0.1

MainView {
    id: mainView
    applicationName: "content-hub-clipboard"
    width: units.gu(48)
    height: units.gu(60)

    PageStack {
        id: pageStack
        Component.onCompleted: push(mainPage)
    }

    Page {
        id: mainPage

        property bool editMode: false

        Binding {
            target: pasteDataModel
            property: "surfaceId"
            value: application.surfaceId
        }

        Binding {
            target: pasteDataModel
            property: "appState"
            value: application.appState
        }

        PasteDataFilterModel {
            id: pasteDataFilterModel
            sourceModel: PasteDataModel {
                id: pasteDataModel
            }
        }

        header: PageHeader {
            id: pageHeader

            property alias leadingAnchors: leadingBar.anchors
            property alias leadingDelegate: leadingBar.delegate
            property alias leadingActions: leadingBar.actions
            property alias trailingAnchors: trailingBar.anchors
            property alias trailingDelegate: trailingBar.delegate
            property alias trailingActions: trailingBar.actions

            leadingActionBar { id: leadingBar }
            trailingActionBar { id: trailingBar }
        }

        states: [
            State {
                id: defaultState
                name: "default"
                when: !mainPage.editMode
                
                property list<QtObject> leadingActions: [
                    Action {
                        iconName: "close"
                        text: i18n.tr("Close")
                        onTriggered: {
                            ContentHub.selectPasteForAppIdCancelled(requesterId)
                            Qt.quit()
                        }
                    }
                ]

                property list<QtObject> trailingActions: [
                    Action {
                        iconName: "edit"
                        text: i18n.tr("Edit")
                        enabled: pasteDataFilterModel.count != 0
                        onTriggered: mainPage.editMode = true
                    }
                ]

                PropertyChanges {
                    target: pageHeader
                    title: i18n.tr("Clipboard")
                    leadingActions: defaultState.leadingActions
                    trailingActions: defaultState.trailingActions
                }
            },

            State {
                id: editState
                name: "edit"
                when: mainPage.editMode

                property bool entriesEdited: false

                property Component delegate: Component {
                    AbstractButton {
                        id: button

                        width: label.width + units.gu(4)
                        height: parent.height

                        action: modelData

                        Rectangle {
                            color: UbuntuColors.slate
                            opacity: 0.1
                            anchors.fill: parent
                            visible: button.pressed
                        }

                        Label {
                            anchors.centerIn: parent
                            id: label
                            text: action.text
                            font.weight: action.iconName === "tick" ? Font.Normal : Font.Light
                        }
                    }
                }

                property list<QtObject> leadingActions: [
                    Action {
                        text: i18n.tr("Cancel")
                        iconName: "close"
                        onTriggered: {
                            pasteDataModel.cancelDeletion()
                            mainPage.editMode = false
                            editState.entriesEdited = false
                        }
                    }
                ]

                property list<QtObject> trailingActions: [
                    Action {
                        text: i18n.tr("Save")
                        iconName: "tick"
                        enabled: editState.entriesEdited
                        onTriggered: {
                            pasteDataModel.deleteEntries()
                            mainPage.editMode = false
                            editState.entriesEdited = false
                        }
                    }
                ]

                property Toolbar extensionToolbar: Toolbar {
                    anchors {
                        left: parent ? parent.left : undefined
                        right: parent ? parent.right : undefined
                        bottom: parent ? parent.bottom : undefined
                    }

                    leadingActionBar.actions: Action {
                        iconName: "delete"
                        text: i18n.tr("Delete")
                        enabled: pasteDataModel.anyEntrySelected
                        onTriggered: {
                            editState.entriesEdited = true
                            pasteDataModel.markSelectedForDeletion()
                        }
                    }

                    trailingActionBar.actions: Action {
                        iconName: pasteDataModel.allEntriesSelected ? "select-none" : "select"
                        text: i18n.tr("Select All")
                        onTriggered: pasteDataModel.selectAll(!pasteDataModel.allEntriesSelected)
                    }
                }

                PropertyChanges {
                    target: pageHeader
                    leadingAnchors.leftMargin: 0
                    leadingDelegate: editState.delegate
                    leadingActions: editState.leadingActions
                    trailingAnchors.rightMargin: 0
                    trailingDelegate: editState.delegate
                    trailingActions: editState.trailingActions
                    extension: editState.extensionToolbar
                }
            }
        ]

        ListView {
            id: clipboardListView

            anchors {
                top: pageHeader.bottom
                bottom: parent.bottom
            }

            width: parent.width

            model: pasteDataFilterModel

            delegate: ClipboardItemDelegate {
                id: delegate

                title: dataType === PasteDataModel.Image ? i18n.tr("Image") : textData
                summary: source
                imageSource: {
                    if (dataType === PasteDataModel.ImageUrl) {
                        return imageData
                    } else if (dataType === PasteDataModel.Image) {
                        return "image://pastedImage/" + application.surfaceId + "/" + id
                    }
                    return ""
                }

                Binding {
                    target: delegate
                    property: "selected"
                    value: entrySelected
                }

                Component.onCompleted: {
                    selectMode = Qt.binding(function() { return mainPage.editMode })
                }

                onSelectedChanged: {
                    pasteDataModel.selectByIndex(index, selected)
                }
                onPressAndHold: {
                    if (!mainPage.editMode) {
                        mainPage.editMode = true
                    }
                }
                onClicked: {
                    if (!selectMode) {
                        ContentHub.selectPasteForAppId(requesterId, application.surfaceId, id, outputType == PasteDataModel.RichText)
                        Qt.quit()
                    } 
                }
                onDeleteClicked: {
                    pasteDataModel.deleteByIndex(index)
                }
                onPreviewClicked: {
                    if (dataType === PasteDataModel.ImageUrl) {
                        previewImagePage.pasteId = id 
                        previewImagePage.imageSource = imageData
                        pageStack.push(previewImagePage)

                    } else if (dataType === PasteDataModel.Image) {
                        previewImagePage.pasteId = id 
                        previewImagePage.imageSource = "image://pastedImage/" + application.surfaceId + "/" + id
                        pageStack.push(previewImagePage)

                    } else {
                        previewTextPage.index = index
                        previewTextPage.pasteId = id
                        previewTextPage.text = textData
                        previewTextPage.richText = htmlData
                        previewTextPage.outputOption = outputType
                        pageStack.push(previewTextPage)
                    }
                }
            }
        }
    }

    PreviewTextPage {
        id: previewTextPage

        property int index
        property int pasteId

        visible: false

        onOutputOptionChanged: pasteDataModel.setOutputTypeByIndex(index, outputOption)

        onPasteClicked: {
            pageStack.pop()
            ContentHub.selectPasteForAppId(requesterId, application.surfaceId, pasteId, outputOption == PasteDataModel.RichText)
            Qt.quit()
        }
    }

    PreviewImagePage {
        id: previewImagePage

        property int pasteId

        visible: false

        onPasteClicked: {
            pageStack.pop()
            ContentHub.selectPasteForAppId(requesterId, application.surfaceId, pasteId, false)
            Qt.quit()
        }
    }
}
