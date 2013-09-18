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
#include <com/ubuntu/content/type.h>

#include "hook.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().count() <= 1)
    {
        qWarning() << "USAGE:" << app.arguments().first() << "APP_ID";
        return 1;
    }

    Hook hook(app.arguments().at(1));
    return app.exec();
}
