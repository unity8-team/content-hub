Content Management & Exchange {#mainpage}
=============================

Unity and the overall Ubuntu experience put heavy emphasis on the
notion of content, with Unity’s dash offering streamlined access to
arbitrary content, both local to the device or online. More to this,
Unity’s dash is the primary way of surfacing content on mobile form
factors without the need to access individual applications and their
respective content silos. The content-hub deals with
application-specific content management and implements an architecture
that allows an app to define its own content silo, exchange content
with other applications/the system, and a way to provide the user with
content picking functionality.

To ease conversations, we start over with a set of definitions:

 - Content item: A content item is an entity that consists of meta-data
and data. E.g., an image is a content item, where the actual pixels
are the data, and information like size, image format, bit depth,
location etc. is considered meta data. \sa com::ubuntu::content::Item.

 - Content types: A set of well-known content types. E.g., images or
   music files. \sa com::ubuntu::content::Type.

 - Content set: A set of unique content items. Can be considered a
   content item itself, e.g., in the case of playlists.

 - Content owner: The unique owner of a content item. A content item
   has to be owned by exactly one app. \sa com::ubuntu::content::Peer

 - Content store: A container (think of it as a top-level folder in
   the filesystem) that contains content items of a certain
   type. Different stores exist for different scopes, where scope
   refers to either system-wide, user-wide or app-specific storage
   locations. \sa com::ubuntu::content::Store, com::ubuntu::content::Scope

 - Content transfer: Transferring content item/s to and from a source
   or destination. A transfer is uniquely defined by:
   * The content source
   * The content destination
   * The transfer direction, either import or export
   * The set of items that should be exchanged
   \sa com::ubuntu::content::Transfer, com::ubuntu::content::Hub::create_import_for_type_from_peer

 - Content picking: Operation that allows a user to select content for
   subsequent import from a content source (e.g., an application). The
   content source is responsible for providing a UI to the user.

Architectural Overview
----------------------

The architecture enforces complete application isolation, both in
terms of content separation, sandboxing/confinement and in terms of
the application lifecycle. As we cannot assume that two apps that want
to exchange content are running at the same time, a system-level
component needs to mediate and control the content exchange operation,
making sure that neither app instance assumes the existence of the
other one. We refer to this component as the content hub. \sa com::ubuntu::content::Hub

Example usage - Importing Pictures
----------------------------------

\snippet acceptance-tests/app_hub_communication_transfer.cpp Importing pictures


Example usage - Exporting Pictures
----------------------------------

\snippet acceptance-tests/app_hub_communication_transfer.cpp Exporting pictures
