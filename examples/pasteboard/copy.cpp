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

    hub->create_paste(const_cast<const QMimeData&>(data));
    qDebug() << text;
    qDebug() << hub->pasteFormats();
}
