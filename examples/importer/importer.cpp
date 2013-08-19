/*
 * Copyright (C) 2013 Canonical, Ltd.
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
#include "example.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //a.setApplicationName("com.example.pictures");
    Example *e = new Example();

    cuc::Transfer *transfer = e->create_import();

        transfer->start();

    if (transfer->state() == cuc::Transfer::charged)
    {
        qDebug() << "CHARGED";
        //e->import(transfer);
    }
    //qDebug() << "STATE: " << transfer->state();

    transfer->abort();

    //e->import(transfer);

    return a.exec();
}
