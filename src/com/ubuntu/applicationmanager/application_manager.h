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
    virtual std::string start_application(const std::string &app_id, const std::string &uri) = 0;
    /*!
     * \brief invoke_application starts an application, and brings it to foreground
     * \param app_id ID for the application (for example "gallery-app" - used for the desktop)
     */
    virtual bool invoke_application(const std::string &app_id, const std::string &uri) = 0;
    /*!
     * \brief stop_application stops an application started by upstart
     * \param app_id ID for the application (for example "gallery-app" - used for the desktop)
     * \param instance_id ID for the instance assigned by upstart
     */
    virtual bool stop_application(const std::string &app_id, const std::string &instance_id) = 0;
};

}
}
}

#endif // COM_UBUNTU_APPLICATION_MANAGER_H_
