/*
 * Copyright © 2016 Canonical Ltd.
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
#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/paste.h>

#include "com/ubuntu/content/detail/peer_registry.h"
#include "com/ubuntu/content/detail/service.h"
#include "com/ubuntu/content/serviceadaptor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QStandardPaths>
#include <QTemporaryDir>
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

TEST(Hub, transfer_creation_and_states_work)
{
    using namespace ::testing;

    test::CrossProcessSync sync;

    auto parent = [&sync]()
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();

        auto mock = new ::testing::NiceMock<MockedPeerRegistry>{};

        QSharedPointer<cucd::PeerRegistry> registry{mock};
        auto app_manager = QSharedPointer<cua::ApplicationManager>(new MockedAppManager());
        cucd::Service implementation(connection, registry, app_manager, &app);
        new ServiceAdaptor(std::addressof(implementation));

        ASSERT_TRUE(connection.registerService(service_name));
        ASSERT_TRUE(connection.registerObject("/", std::addressof(implementation)));

        sync.signal_ready();

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);
    };

    auto child = [&sync]()
    {
        int argc = 0;
        QCoreApplication app(argc, nullptr);

        sync.wait_for_signal_ready();

        test::TestHarness harness;
        harness.add_test_case([]()
        {
            qputenv("APP_ID", "some-app");

            QMimeData data;
            data.setText("some text");
            auto hub = cuc::Hub::Client::instance();
            QDBusPendingCall reply = hub->createPaste(const_cast<const QMimeData&>(data));
            reply.waitForFinished();
            ASSERT_FALSE(reply.isError());
            EXPECT_EQ(QString(data.text()), QString(hub->latestPaste()->text()));
            EXPECT_EQ(QString(data.text()), QString(hub->pasteById(1)->text()));

            hub->quit();
        });
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
    };

    EXPECT_EQ(EXIT_SUCCESS, test::fork_and_run(child, parent));
}
