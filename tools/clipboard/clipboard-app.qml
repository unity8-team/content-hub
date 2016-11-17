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
    applicationName: "content-hub-clipboard"
    width: units.gu(48)
    height: units.gu(60)

    Connections {
        target: application
        onSurfaceIdChanged: mainPageLoader.loadMainPage()
    }

    PageStack {
        id: pageStack
    }

    Loader {
        id: mainPageLoader
        active: false

        function loadMainPage() {
            if (!mainPageLoader.active) {
                mainPageLoader.active = true
            }
        }

        onStatusChanged: {
            if (mainPageLoader.status == Loader.Ready) {
                pageStack.push(mainPageLoader.item)
            }
        }

        sourceComponent: Page {
            id: mainPage

            property bool editMode: false

            visible: false

            PasteDataFilterModel {
                id: pasteDataFilterModel
                sourceModel: PasteDataModel {
                    id: pasteDataModel
                    surfaceId: application.surfaceId
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
                                pasteDataModel.cancelEntriesDeleted()
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
                                pasteDataModel.saveEntriesDeleted()
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
                                pasteDataModel.setSelectedEntriesDeleted()
                            }
                        }

                        trailingActionBar.actions: Action {
                            iconName: pasteDataModel.allEntriesSelected ? "select-none" : "select"
                            text: i18n.tr("Select All")
                            onTriggered: pasteDataModel.setAllEntriesSelected(!pasteDataModel.allEntriesSelected)
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
                    title: pasteData
                    summary: source
                    imageSource: dataType === pasteDataModel.ImageType ? pasteData : ""

                    Binding {
                        target: delegate
                        property: "selected"
                        value: itemSelected
                    }

                    Component.onCompleted: {
                        selectMode = Qt.binding(function() { return mainPage.editMode })
                    }

                    onSelectedChanged: pasteDataModel.setEntrySelectedByIndex(index, selected)

                    onPressAndHold: {
                        if (!mainPage.editMode) {
                            mainPage.editMode = true
                        }
                    }
                    onClicked: {
                        if (!selectMode) {
                            pasteDataModel.pasteEntryByIndex(index)
                        } 
                    }
                    onDeleteClicked: pasteDataModel.removeEntryByIndex(index)
                    onPreviewClicked: {
                        if (dataType === pasteDataModel.ImageType) {
                            previewImageLoader.loadPreview(index, pasteData)
                        } else {
                            previewTextLoader.loadPreview(index, pasteData)
                        }
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
            onPasteClicked: pasteDataModel.pasteEntryByIndex(previewTextLoader.index)
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
            onPasteClicked: pasteDataModel.pasteEntryByIndex(previewImageLoader.index)
        }

        onStatusChanged: {
            if (previewImageLoader.status == Loader.Ready) {
                pageStack.push(previewImageLoader.item)
            }
        }
    }
}
