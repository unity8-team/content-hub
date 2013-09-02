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

//#include <ubuntu/ui/ubuntu_ui_session_service.h>

#include <libupstart-app-launch-1/upstart-app-launch.h>

namespace cucd = com::ubuntu::content::detail;

cucd::AppManager::AppManager()
{
}

/*!
 * \reimp
 */
bool cucd::AppManager::invoke_application(const std::string &app_id)
{
//    ubuntu_ui_session_trigger_switch_to_well_known_application(GALLERY_APP);

    gchar ** uris = NULL;
    gboolean ok = upstart_app_launch_start_application(app_id.c_str(), (const gchar * const *)uris);

    return static_cast<bool>(ok);
}
