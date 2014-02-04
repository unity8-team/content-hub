/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
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

#include "com/ubuntu/applicationmanager/application_manager.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cua = com::ubuntu::ApplicationManager;

namespace
{
struct MockedAppManager : public cua::ApplicationManager
{
    MockedAppManager() : cua::ApplicationManager()
    {
        using namespace ::testing;

        ON_CALL(*this, start_application(_,_)).WillByDefault(Return("SOMEINSTANCE"));
        ON_CALL(*this, invoke_application(_,_)).WillByDefault(Return(true));
        ON_CALL(*this, stop_application(_,_)).WillByDefault(Return(true));
        ON_CALL(*this, is_application_started(_)).WillByDefault(Return(true));
    }

    MOCK_METHOD2(start_application, std::string(const std::string &, const std::string &));
    MOCK_METHOD2(invoke_application, bool(const std::string &, const std::string &));
    MOCK_METHOD2(stop_application, bool(const std::string &, const std::string &));
    MOCK_METHOD1(is_application_started, bool(const std::string &));
};
}
