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
#include <QDir>
#include <QStandardPaths>

#include "hook.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << Q_FUNC_INFO;

    if (app.arguments().count() > 1)
    {
            qWarning() << "Shouldn't have arguments";
            return 1;
    }

    QDir contentDir(
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
        + QString("/")
        + QString("content-hub"));

    if (!contentDir.exists()) {
	return 0;
    }

    new Hook();

    app.exec();

    /* We always want to return 0 */
    return 0;
}
