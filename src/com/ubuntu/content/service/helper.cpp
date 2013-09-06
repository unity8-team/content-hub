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
#include <QDebug>

#include "registry.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char** argv)
{
    QCoreApplication *app = new QCoreApplication(argc, argv);

    if (app->arguments().count() < 1)
    {
        qWarning() << "USAGE:" << app->arguments().first() << "APP_ID";
        app->exit(1);
    }

    /* FIXME: we should do a sanity check on this before installing */
    auto peer = cuc::Peer(app->arguments().at(1));

    Registry *registry = new Registry();
    registry->install_peer_for_type(cuc::Type::Known::pictures(), peer);
    return app->exec();
}
