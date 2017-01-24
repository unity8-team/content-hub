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
#include <ubuntu-app-launch/appid.h>
#include <ubuntu-app-launch/helper.h>
#include <ubuntu-app-launch/application.h>
#include <ubuntu-app-launch/registry.h>

namespace cucd = com::ubuntu::content::detail;
namespace ual = ubuntu::app_launch;

/*!
 * \reimp
 */
std::shared_ptr<ual::Application::Instance> cucd::AppManager::invoke_application(const std::string &app_id, gchar ** uris)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();

    try {
        auto registry = ual::Registry::getDefault();
        auto appId = ual::AppID::find(app_id);
        auto app = ual::Application::create(appId, registry);

        std::vector<ual::Application::URL> urivect;
        for (auto i = 0; uris != nullptr && uris[i] != nullptr; i++)
            urivect.emplace_back(ual::Application::URL::from_raw(uris[i]));

        auto instance = app->launch(urivect);

        if (instance)
            return instance;
        else
            return nullptr;
    } catch (std::runtime_error &e) {
        g_warning("Unable to start app '%s': %s", app_id, e.what());
        return nullptr;
    }
}

/*!
 * \reimp
 */
std::shared_ptr<ual::Application::Instance> cucd::AppManager::invoke_application_with_session(const std::string &app_id, PromptSessionP session, gchar ** uris)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id.c_str();
    auto psession = session.data()->get();
    if (psession == NULL)
        return nullptr;

    try {
        auto registry = ual::Registry::getDefault();
        auto appId = ual::AppID::find(app_id);
        auto app = ual::Helper::create(ual::Helper::Type::from_raw("content-hub"), appId, registry);

        std::vector<ual::Helper::URL> urivect; 
        for (auto i = 0; uris != nullptr && uris[i] != nullptr; i++)
            urivect.emplace_back(ual::Helper::URL::from_raw(uris[i]));

        std::vector<ual::Application::URL> urls = {};
        auto instance = app->launch(psession, urivect);
        
        if (instance)
            return std::dynamic_pointer_cast<ual::Application::Instance>(instance);
        else
            return nullptr;
    } catch (std::runtime_error &e) {
        g_warning("Unable to start app '%s': %s", app_id, e.what());
        return nullptr;
    }
}

/*!
 * \reimp
 */
bool cucd::AppManager::is_application_started(const std::string &app_id)
{
    GPid pid = ubuntu_app_launch_get_primary_pid(app_id.c_str());
    return pid != 0;
}
