import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(75)

    PeerPicker {
        showTitle: true
        // Type of handler: Source, Destination, or Share
        handler: ContentHandler.Source
        // well know content type
        contentType: ContentType.Pictures

        onPeerSelected: {
        }

        onCancelPressed: {
        }
    }
}
