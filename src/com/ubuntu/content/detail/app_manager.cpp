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
    QVector<const gchar*> uris;
    /* Always append this to uris, it's used by the StateSaver via the 
       UriHandler in the toolkit to clear the saved state on start.  
       Otherwise the state of the app could be restored instead of 
       updating the UI to handle the transfer.  The UriHandler 
       filters our specific URI preventing apps from handling
       it unnecessarily.
    */
    uris.append("--no-restore-state");
    uris.append(NULL);

    gboolean ok = ubuntu_app_launch_start_application(app_id.c_str(), uris.constData());
    return static_cast<bool>(ok);
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
