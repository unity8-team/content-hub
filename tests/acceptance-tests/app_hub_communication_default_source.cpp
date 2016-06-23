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
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include "app_manager_mock.h"
#include "test_harness.h"
#include <core/testing/cross_process_sync.h>
#include <core/testing/fork_and_run.h>

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/type.h>

#include "com/ubuntu/content/serviceadaptor.h"

#include "com/ubuntu/content/detail/peer_registry.h"
#include "com/ubuntu/content/detail/service.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QStandardPaths>
#include <QtTest/QTest>

#include <thread>

namespace cua = com::ubuntu::ApplicationManager;
namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

void PrintTo(const QString& s, ::std::ostream* os) {
    *os << std::string(qPrintable(s));
}

struct Hub : public ::testing::Test
{
};

namespace
{
QString service_name{"com.ubuntu.content.dbus.Service"};

struct MockedPeerRegistry : public cucd::PeerRegistry
{
    MockedPeerRegistry() : cucd::PeerRegistry()
    {
        using namespace ::testing;

        ON_CALL(*this, default_source_for_type(_)).WillByDefault(Return(cuc::Peer::unknown()));
        ON_CALL(*this, install_default_source_for_type(_,_)).WillByDefault(Return(false));
        ON_CALL(*this, install_source_for_type(_,_)).WillByDefault(Return(false));
    }

    MOCK_METHOD1(default_source_for_type, cuc::Peer(cuc::Type t));
    MOCK_METHOD1(enumerate_known_peers, void(const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD2(enumerate_known_sources_for_type, void(cuc::Type, const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD2(enumerate_known_destinations_for_type, void(cuc::Type, const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD2(enumerate_known_shares_for_type, void(cuc::Type, const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD2(install_default_source_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD2(install_source_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD2(install_destination_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD2(install_share_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD1(remove_peer, bool(cuc::Peer));
    MOCK_METHOD1(peer_is_legacy, bool(QString));
};
}

TEST(Hub, querying_default_peer_returns_correct_value)
{
    using namespace ::testing;

    core::testing::CrossProcessSync sync;
    
    QString default_peer_id{"com.does.not.exist.anywhere.application"};

    auto service = [this, &sync, default_peer_id]() -> core::posix::exit::Status
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();        
        
        auto mock = new MockedPeerRegistry{};
        EXPECT_CALL(*mock, default_source_for_type(_)).
        Times(Exactly(1)).
        WillRepeatedly(Return(cuc::Peer{default_peer_id}));

        QSharedPointer<cucd::PeerRegistry> registry{mock};

        auto app_manager = QSharedPointer<cua::ApplicationManager>(new ::testing::NiceMock<MockedAppManager>);
        auto implementation = new cucd::Service(connection, registry, app_manager, &app);
        new ServiceAdaptor(implementation);

        connection.registerService(service_name);
        connection.registerObject("/", implementation);

        sync.try_signal_ready_for(std::chrono::seconds{120});

        app.exec();

        delete implementation;
        connection.unregisterObject("/");
        connection.unregisterService(service_name);
        //return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
        return core::posix::exit::Status::success;
    };

    auto client = [this, &sync, default_peer_id]() -> core::posix::exit::Status
    {
        EXPECT_EQ(1, sync.wait_for_signal_ready_for(std::chrono::seconds{120}));

        int argc = 0;
        QCoreApplication app(argc, nullptr);
        
        auto hub = cuc::Hub::Client::instance();
        
        test::TestHarness harness;
        harness.add_test_case([hub, default_peer_id]()
        {            
            EXPECT_EQ(default_peer_id, hub->default_source_for_type(cuc::Type::Known::documents()).id());
        });
        
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
        
        hub->quit();
        return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
    };

    EXPECT_EQ(core::testing::ForkAndRunResult::empty, core::testing::fork_and_run(service, client));
}
