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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
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
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/import_export_handler.h>

#include "com/ubuntu/content/utils.cpp"
#include "com/ubuntu/content/detail/peer_registry.h"
#include "com/ubuntu/content/detail/service.h"
#include "com/ubuntu/content/serviceadaptor.h"

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

struct MockedHandler : public cuc::ImportExportHandler
{
    MockedHandler() : cuc::ImportExportHandler()
    {
        using namespace ::testing;
        ON_CALL(*this, handle_import(_)).WillByDefault(Return());
        ON_CALL(*this, handle_export(_)).WillByDefault(Return());
        ON_CALL(*this, handle_share(_)).WillByDefault(Return());
    }

    MOCK_METHOD1(handle_import, void(cuc::Transfer*));
    MOCK_METHOD1(handle_export, void(cuc::Transfer*));
    MOCK_METHOD1(handle_share, void(cuc::Transfer*));
};
}

TEST(Handler, handler_on_bus)
{
    using namespace ::testing;

    QString default_peer_id{"com.does.not.exist.anywhere.application"};
    QString default_dest_peer_id{"com.also.does.not.exist.anywhere.application"};

    core::testing::CrossProcessSync sync;
    
    auto service = [this, &sync, default_peer_id]() -> core::posix::exit::Status
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();

        QSharedPointer<cucd::PeerRegistry> registry{new MockedPeerRegistry{}};
        auto app_manager = QSharedPointer<cua::ApplicationManager>(new ::testing::NiceMock<MockedAppManager>);
        auto implementation = new cucd::Service(connection, registry, app_manager, &app);
        new ServiceAdaptor(implementation);

        EXPECT_TRUE(connection.registerService(service_name));
        EXPECT_TRUE(connection.registerObject("/", implementation));

        /* register handler on the service */
        auto mock_handler = new MockedHandler{};
        EXPECT_CALL(*mock_handler, handle_export(_)).Times(Exactly(1));
        qputenv("APP_ID", default_peer_id.toLatin1());
        auto hub = cuc::Hub::Client::instance();
        hub->register_import_export_handler(mock_handler);
        hub->quit();

        sync.try_signal_ready_for(std::chrono::seconds{120});

        app.exec();

        delete implementation;
        delete mock_handler;
        connection.unregisterObject("/");
        connection.unregisterService(service_name);
        return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
    };

    auto client = [this, &sync, default_peer_id, default_dest_peer_id]() -> core::posix::exit::Status

    {
        EXPECT_EQ(1, sync.wait_for_signal_ready_for(std::chrono::seconds{120}));

        int argc = 0;
        QCoreApplication app(argc, nullptr);

        test::TestHarness harness;
        harness.add_test_case([default_peer_id, default_dest_peer_id]()
        {
            qputenv("APP_ID", default_dest_peer_id.toLatin1());
            auto hub = cuc::Hub::Client::instance();
            hub = cuc::Hub::Client::instance();
            auto transfer = hub->create_import_from_peer(cuc::Peer(default_peer_id));
            EXPECT_TRUE(transfer != nullptr);
            EXPECT_TRUE(transfer->start());
            EXPECT_EQ(cuc::Transfer::in_progress, transfer->state());
            hub->quit();
        });

        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
        return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
    };

    EXPECT_EQ(core::testing::ForkAndRunResult::empty, core::testing::fork_and_run(service, client));
}
