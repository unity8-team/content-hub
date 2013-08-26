/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <QtCore>

namespace {

/* sanitize the dbus object path */
QString sanitize_path(const QString& path)
{
    QString sanitized = path;

    for (int i = 0; i < sanitized.length(); ++i)
    {
        if ( !( sanitized[i].isLetter() || sanitized[i].isDigit()))
            sanitized[i] = QLatin1Char('_');
    }
    return sanitized;
}

/* define a bus_name based on our namespace and the app_id */
QString handler_address(QString app_id)
{
    const QString bus_name{"com.ubuntu.content.handler.%1"};
    return bus_name.arg(app_id);
}

QString app_id()
{
    /* FIXME: rely on APP_ID from env for now,
     * but we'll use this function as a place to
     * later use the application manager
     */
    QString app_id = qgetenv("APP_ID");
    return app_id;
}
}
