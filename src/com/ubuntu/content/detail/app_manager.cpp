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
#include "mir-helper.h"

#include <ubuntu-app-launch.h>

namespace cucd = com::ubuntu::content::detail;

/*!
 * \reimp
 */
bool cucd::AppManager::invoke_application(const std::string &app_id, gchar ** uris)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    gboolean ok = ubuntu_app_launch_start_application(app_id.c_str(), (const gchar * const *)uris);
    return static_cast<bool>(ok);
}

/*!
 * \reimp
 */
std::string cucd::AppManager::invoke_application_with_session(const std::string &app_id, const PromptSessionP &session)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    QVector<const gchar*> uris;
    uris.append(NULL);
    auto instance_c = ubuntu_app_launch_start_session_helper("content-hub",
                                                             session.data()->get(),
                                                             app_id.c_str(),
                                                             uris.constData());
    std::string instid = std::string(instance_c);
    return instid;
}

/*!
 * \reimp
 */
bool cucd::AppManager::stop_application_with_helper(const std::string &app_id, const std::string &instance_id)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    TRACE() << Q_FUNC_INFO << "INSTANCE_ID:" << instance_id.c_str();

    gboolean ok = ubuntu_app_launch_stop_multiple_helper("content-hub",
                                                         app_id.c_str(),
                                                         instance_id.c_str());
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
