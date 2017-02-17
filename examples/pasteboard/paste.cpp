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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <QCoreApplication>
#include <QStringList>
#include <com/ubuntu/content/hub.h>

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (qgetenv("APP_ID").isEmpty()) {
        qputenv("APP_ID", "paste-from-pasteboard");
    }

    QString id("latest");

    if (a.arguments().size() > 1)
        id = a.arguments().at(1);

    auto hub = cuc::Hub::Client::instance();

    /* Won't work unless you disable the surface Id verification with CONTENT_HUB_TESTING=1 env var
       To get the surface Id of a MirSurface in a real GUI app you should to it like the following:
          MirWindowId* mirWindowId = mir_window_request_window_id_sync(mirWindow);
          QString windowId = mir_window_id_as_string(mirPermaId);
          mir_window_id_release(mirWindowId);
    */
    QString surfaceId("some-bogus-fake-surface-id");

    QMimeData *mimeData = nullptr;
    if (id == "latest") {
        mimeData = hub->latestPaste(surfaceId);
    } else {
        mimeData = hub->pasteById(surfaceId, id.toInt());
    }
    if (mimeData) {
        qDebug() << id << ":" << mimeData->text();
    } else {
        qDebug() << "content-hub returned no mimedata.";
    }
    return 0;
}
