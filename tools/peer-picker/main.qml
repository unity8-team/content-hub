import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(75)
    Component.onCompleted: {
        console.debug("contentType: " + wellKnownType)
        console.debug("requesterId: " + requesterId)
        console.debug("handlerType: " + handlerType)
    }

    PeerPicker {
        showTitle: true
        // Type of handler: Source, Destination, or Share
        //handler: ContentHandler.Source
        handler: {
            if (handlerType === "source")
                return ContentHandler.Source
            if (handlerType === "share")
                return ContentHandler.Share
            else
                return ContentHandler.Destination
        }
        // well know content type
        contentType: {
            if (wellKnownType === "documents")
                return ContentType.Documents 
            else if (wellKnownType === "pictures")
                return ContentType.Pictures 
            else if (wellKnownType === "music")
                return ContentType.Music 
            else if (wellKnownType === "contacts")
                return ContentType.Contacts 
            else if (wellKnownType === "videos")
                return ContentType.Videos 
            else if (wellKnownType === "links")
                return ContentType.Links 
            else if (wellKnownType === "ebooks")
                return ContentType.EBooks 
            else if (wellKnownType === "text")
                return ContentType.Text 
            else if (wellKnownType === "events")
                return ContentType.Events 
            else
                return ContentType.All
        }

        onPeerSelected: {
        }

        onCancelPressed: {
            Qt.quit()
        }
    }
}
