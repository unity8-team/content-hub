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
 */

#include "app_manager.h"
#include "debug.h"

#include <ubuntu-app-launch.h>

namespace cucd = com::ubuntu::content::detail;

/*!
 * \reimp
 */
bool cucd::AppManager::invoke_application(const std::string &app_id)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    gchar ** uris = NULL;
    gboolean ok = ubuntu_app_launch_start_application(app_id.c_str(), (const gchar * const *)uris);
    return static_cast<bool>(ok);
}

/*!
 * \reimp
 */
bool cucd::AppManager::invoke_application_with_socket(const std::string &app_id, const std::string &socket)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    QVector<const gchar*> uris;
    uris.append(g_strdup_printf("%s", socket.c_str()));
    uris.append("");
    uris.append(NULL);
    TRACE() << Q_FUNC_INFO << "URIS:" << g_strdup_printf("%s", socket.c_str());
    gchar *instanceId = ubuntu_app_launch_start_multiple_helper("content-hub",
                                                                app_id.c_str(),
                                                                uris.constData());

    if (instanceId == NULL) {
        g_free(instanceId);
        return false;
    }
    g_free(instanceId);
    return true;
}

/*!
 * \reimp
 */
bool cucd::AppManager::stop_application(const std::string &app_id)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();

    gboolean ok = ubuntu_app_launch_stop_application(app_id.c_str());
    return static_cast<bool>(ok);
}

/*!
 * \reimp
 */
bool cucd::AppManager::is_application_started(const std::string &app_id)
{
    GPid pid = ubuntu_app_launch_get_primary_pid(app_id.c_str());
    return pid != 0;
}
