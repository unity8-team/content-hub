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
#ifndef COM_UBUNTU_APPLICATION_MANAGER_H_
#define COM_UBUNTU_APPLICATION_MANAGER_H_

#include <string>
#include "../content/detail/mir-helper.h"
#include <glib.h>
#include <ubuntu-app-launch/appid.h>
#include <ubuntu-app-launch/application.h>
#include <ubuntu-app-launch/registry.h>


namespace ual = ubuntu::app_launch;

namespace com
{
namespace ubuntu
{
namespace ApplicationManager
{

class ApplicationManager
{
  public:
    ApplicationManager() = default;
    ApplicationManager(const ApplicationManager&) = default;
    virtual ~ApplicationManager() = default;
    ApplicationManager& operator=(const ApplicationManager&) = default;

    /*!
     * \brief invoke_application starts an application, and brings it to foreground
     * \param app_id ID for the application (for example "gallery-app" - used for the desktop)
     */
    virtual std::shared_ptr<ual::Application::Instance> invoke_application(const std::string &app_id, gchar ** uris) = 0;

    /*!
     * \brief invoke_application_with_session starts an application without a trusted session
     * \param app_id ID for the application (for example "gallery-app" - used for the desktop)
     * \param session
     */
    virtual std::shared_ptr<ual::Helper::Instance> invoke_application_with_session(const std::string &app_id, PromptSessionP session, gchar ** uris) = 0;

    /*!
     * \brief is_application_started returns true, if the application s already started by ubuntu
     * \param app_id ID for the application (for example "gallery-app" - used for the desktop)
     */
    virtual bool is_application_started(const std::string &app_id) = 0;
};

}
}
}

#endif // COM_UBUNTU_APPLICATION_MANAGER_H_
