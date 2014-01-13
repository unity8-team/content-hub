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
#include "../cross_process_sync.h"
#include "../fork_and_run.h"

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/share_handler.h>

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

namespace
{
QString service_name{"com.ubuntu.content.dbus.Service"};

struct MockedPeerRegistry : public cucd::PeerRegistry
{
    MockedPeerRegistry() : cucd::PeerRegistry()
    {
        using namespace ::testing;
    }

    MOCK_METHOD1(default_peer_for_type, cuc::Peer(cuc::Type t));
    MOCK_METHOD2(enumerate_known_peers_for_type, void(cuc::Type, const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD1(enumerate_known_peers, void(const std::function<void(const cuc::Peer&)>&));
    MOCK_METHOD2(install_default_peer_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD2(install_peer_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD1(remove_peer, bool(cuc::Peer));
};

struct MockedHandler : public cuc::ShareHandler
{
    MockedHandler() : cuc::ShareHandler()
    {
        using namespace ::testing;
        ON_CALL(*this, handle_share(_)).WillByDefault(Return());
    }

    MOCK_METHOD1(handle_share, void(cuc::Transfer*));
};
}

TEST(Handler, handler_on_bus)
{
    using namespace ::testing;

    QString default_peer_id{"com.does.not.exist.anywhere.application"};
    QString default_dest_peer_id{"com.also.does.not.exist.anywhere.application"};

    test::CrossProcessSync sync;
    
    auto parent = [&sync, default_peer_id]()
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();

        QSharedPointer<cucd::PeerRegistry> registry{new MockedPeerRegistry{}};
        auto app_manager = QSharedPointer<cua::ApplicationManager>(new MockedAppManager());
        auto implementation = new cucd::Service(connection, registry, app_manager, &app);
        new ServiceAdaptor(implementation);

        ASSERT_TRUE(connection.registerService(service_name));
        ASSERT_TRUE(connection.registerObject("/", implementation));

        sync.signal_ready();

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);
        delete implementation;
    };

    auto child = [&sync, default_peer_id, default_dest_peer_id]()
    {
        sync.wait_for_signal_ready();

        int argc = 0;
        QCoreApplication app(argc, nullptr);

        test::TestHarness harness;
        harness.add_test_case([default_peer_id, default_dest_peer_id]()
        {
            /* register handler on the service */
            auto mock_handler = new MockedHandler{};
            EXPECT_CALL(*mock_handler, handle_share(_)).Times(Exactly(1));
            qputenv("APP_ID", default_peer_id.toLatin1());
            auto hub = cuc::Hub::Client::instance();
            hub->register_share_handler(mock_handler);
            hub->quit();

            qputenv("APP_ID", default_dest_peer_id.toLatin1());
            hub = cuc::Hub::Client::instance();
            auto transfer = hub->create_import_for_type_from_peer(
                cuc::Type::Known::pictures(),
                cuc::Peer(default_peer_id));
            ASSERT_TRUE(transfer != nullptr);
            EXPECT_TRUE(transfer->start());
            EXPECT_EQ(cuc::Transfer::in_progress, transfer->state());
            EXPECT_EQ(cuc::Transfer::charged, transfer->state());
            hub->quit();
            delete mock_handler;
        });

        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
    };

    EXPECT_TRUE(test::fork_and_run(child, parent) != EXIT_FAILURE);
}
