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
#ifndef COM_UBUNTU_APP_MANAGER_H_
#define COM_UBUNTU_APP_MANAGER_H_

#include <com/ubuntu/applicationmanager/application_manager.h>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{

class AppManager: public com::ubuntu::ApplicationManager::ApplicationManager
{
  public:
    AppManager() = default;
    AppManager(const AppManager&) = default;
    virtual ~AppManager() = default;
    AppManager& operator=(const AppManager&) = default;

    /*!
     * \brief invoke_application starts an application, and brings it to foreground
     * \param desktopFile .desktop file for that applicaiton
     */
    virtual bool invoke_application(const std::string &app_id);
};

}
}
}
}

#endif // COM_UBUNTU_APPLICATION_MANAGER_H_
