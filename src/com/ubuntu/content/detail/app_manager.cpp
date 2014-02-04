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

#include <upstart-app-launch.h>
#include <QDebug>

namespace cucd = com::ubuntu::content::detail;

/*!
 * \reimp
 */
std::string cucd::AppManager::start_application(const std::string &app_id, const std::string &uri)
{
    qDebug() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();

    gchar ** uris = NULL;
    uris = g_new0(gchar *, 1);
    uris[0] = g_strdup(uri.c_str());
    //std::string instance_id = upstart_app_launch_start_multiple_helper("content-hub", app_id.c_str(), (const gchar * const *)uris);
    std::string instance_id = "";
    upstart_app_launch_start_application(app_id.c_str(), (const gchar * const *)uris);
    return instance_id;
}

bool cucd::AppManager::invoke_application(const std::string &app_id, const std::string &uri)
{
    qDebug() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();

    gchar ** uris = NULL;
    uris = g_new0(gchar *, 1);
    uris[0] = g_strdup(uri.c_str());
    gboolean ok = upstart_app_launch_start_application(app_id.c_str(), (const gchar * const *)uris);
    return static_cast<bool>(ok);
}

/*!
 * \reimp
 */
bool cucd::AppManager::stop_application(const std::string &app_id, const std::string &instance_id)
{
    qDebug() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str() << "INSTANCE_ID:" << instance_id.c_str();

    //gboolean ok = upstart_app_launch_stop_multiple_helper("content-hub", app_id.c_str(), instance_id.c_str());
    gboolean ok = upstart_app_launch_stop_application(app_id.c_str());
    return static_cast<bool>(ok);
}

/*!
 * \reimp
 */
bool cucd::AppManager::is_application_started(const std::string &app_id)
{
    GPid pid = upstart_app_launch_get_primary_pid(app_id.c_str());
    return pid != 0;
}
