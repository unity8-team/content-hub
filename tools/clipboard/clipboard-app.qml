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
import clipboardapp.private 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    PasteDataDeletedModel {
        id: pasteDataDeletedModel
        sourceModel: PasteDataModel
    }

    PageStack {
        id: pageStack
        Component.onCompleted: push(mainPage)
    }

    Page {
        id: mainPage

        property bool editMode: false

        visible: false

        header: PageHeader {
            id: pageHeader

            property alias leadingAnchors: leadingBar.anchors
            property alias leadingDelegate: leadingBar.delegate
            property alias leadingActions: leadingBar.actions
            property alias trailingAnchors: trailingBar.anchors
            property alias trailingDelegate: trailingBar.delegate
            property alias trailingActions: trailingBar.actions

            leadingActionBar {
                id: leadingBar 
            }

            trailingActionBar {
                id: trailingBar 
            }
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
                        onTriggered: Qt.quit()
                    }
                ]

                property list<QtObject> trailingActions: [
                    Action {
                        iconName: "edit"
                        text: i18n.tr("Edit")
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

                property bool allEntriesSelected: false

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
                            PasteDataModel.cancelEntriesDeleted()
                            mainPage.editMode = false
                        }
                    }
                ]

                property list<QtObject> trailingActions: [
                    Action {
                        text: i18n.tr("Save")
                        iconName: "tick"
                        onTriggered: {
                            PasteDataModel.saveEntriesDeleted()
                            mainPage.editMode = false
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
                        onTriggered: PasteDataModel.setSelectedEntriesDeleted()
                    }

                    trailingActionBar.actions: Action {
                        iconName: editState.allEntriesSelected ? "select-none" : "select"
                        text: i18n.tr("Select All")
                        onTriggered: {
                            editState.allEntriesSelected = !editState.allEntriesSelected
                            PasteDataModel.setAllEntriesSelected(editState.allEntriesSelected)
                        }
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

            model: pasteDataDeletedModel

            delegate: ClipboardItemDelegate {
                id: delegate
                title: pasteData
                summary: source
                imageSource: dataType === PasteDataModel.ImageType ? pasteData : ""

                selectMode: mainPage.editMode

                Binding {
                    target: delegate
                    property: "selected"
                    value: itemSelected
                }

                onSelectModeChanged: delegate.selected = false

                onSelectedChanged: {
                    if (!selected) {
                        editState.allEntriesSelected = false
                    }

                    PasteDataModel.setEntrySelectedByIndex(index, selected)
                }

                onClicked: {
                    if (!selectMode) {
                        PasteDataModel.pasteEntryByIndex(index)
                    } 
                }
                onDeleteClicked: PasteDataModel.removeEntryByIndex(index)
                onPreviewClicked: {
                    if (dataType === PasteDataModel.ImageType) {
                        previewImageLoader.loadPreview(index, pasteData)
                    } else {
                        previewTextLoader.loadPreview(index, pasteData)
                    }
                }
            }
        }
    }

    Loader {
        id: previewTextLoader

        property int index: -1
        property string textPreview: ""

        function loadPreview(index, text) {
            previewTextLoader.index = index
            previewTextLoader.textPreview = text
            if (!previewTextLoader.active) {
                previewTextLoader.active = true    
            } else if (previewTextLoader.status == Loader.Ready) {
                pageStack.push(previewTextLoader.item)
            }
        }

        active: false

        sourceComponent: PreviewTextPage {
            visible: false
            text: previewTextLoader.textPreview
            onPasteClicked: PasteDataModel.pasteEntryByIndex(previewTextLoader.index)
        }

        onStatusChanged: {
            if (previewTextLoader.status == Loader.Ready) {
                pageStack.push(previewTextLoader.item)
            }
        }
    }

    Loader {
        id: previewImageLoader

        property int index: -1
        property url imagePreview: ""

        function loadPreview(index, url) {
            previewImageLoader.index = index
            previewImageLoader.imagePreview = url
            if (!previewImageLoader.active) {
                previewImageLoader.active = true    
            } else if (previewImageLoader.status == Loader.Ready) {
                pageStack.push(previewImageLoader.item)
            }
        }

        active: false

        sourceComponent: PreviewImagePage {
            visible: false
            imageSource: previewImageLoader.imagePreview
            onPasteClicked: PasteDataModel.pasteEntryByIndex(previewImageLoader.index)
        }

        onStatusChanged: {
            if (previewImageLoader.status == Loader.Ready) {
                pageStack.push(previewImageLoader.item)
            }
        }
    }
}
