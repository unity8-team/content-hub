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
#include <QDebug>
#include <QStringList>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/paste.h>

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (qgetenv("APP_ID").isEmpty()) {
        qputenv("APP_ID", "copy-to-pasteboard");
    }

    QString text("Some text");

    if (a.arguments().size() > 1)
        text = a.arguments().at(1);

    QMimeData data;
    data.setText(text);

    auto hub = cuc::Hub::Client::instance();

    /* Won't work unless you disable the surface Id verification with CONTENT_HUB_TESTING=1 env var
       To get the surface Id of a MirSurface in a real GUI app you should to it like the following:
          MirWindowId* mirWindowId = mir_window_request_window_id_sync(mirWindow);
          QString windowId = mir_window_id_as_string(mirWindowId);
          mir_window_id_release(mirWindowId);
    */
    QString surfaceId("some-bogus-fake-surface-id");

    hub->createPasteSync(surfaceId, const_cast<const QMimeData&>(data));

    qDebug() << text;
    qDebug() << hub->pasteFormats();
}
