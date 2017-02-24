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

        ON_CALL(*this, invoke_application(_,_)).WillByDefault(Return(nullptr));
        ON_CALL(*this, invoke_application_with_session(_,_,_)).WillByDefault(Return(nullptr));
        ON_CALL(*this, is_application_started(_)).WillByDefault(Return(true));
    }

    MOCK_METHOD2(invoke_application, std::shared_ptr<ual::Application::Instance>(const std::string &, gchar ** uris));
    MOCK_METHOD3(invoke_application_with_session, std::shared_ptr<ual::Helper::Instance>(const std::string &, PromptSessionP, gchar ** uris));
    MOCK_METHOD1(is_application_started, bool(const std::string &));
};
}
