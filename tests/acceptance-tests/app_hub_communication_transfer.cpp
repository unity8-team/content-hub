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
#include "../cross_process_sync.h"
#include "../fork_and_run.h"

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/type.h>

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

        ON_CALL(*this, default_peer_for_type(_)).WillByDefault(Return(cuc::Peer::unknown()));
        ON_CALL(*this, install_default_peer_for_type(_,_)).WillByDefault(Return(false));
        ON_CALL(*this, install_peer_for_type(_,_)).WillByDefault(Return(false));
    }

    MOCK_METHOD1(default_peer_for_type, cuc::Peer(cuc::Type t));
    MOCK_METHOD2(enumerate_known_peers_for_type, void(cuc::Type, const std::function<void(const cuc::Peer&)>&));
    
    MOCK_METHOD2(install_default_peer_for_type, bool(cuc::Type, cuc::Peer));
    MOCK_METHOD2(install_peer_for_type, bool(cuc::Type, cuc::Peer));
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

        QString default_peer_id{"com.does.not.exist.anywhere.application"};

        QDBusConnection connection = QDBusConnection::sessionBus();        
        
        auto mock = new ::testing::NiceMock<MockedPeerRegistry>{};
        EXPECT_CALL(*mock, default_peer_for_type(_)).
        Times(Exactly(2)).
        WillRepeatedly(Return(cuc::Peer{default_peer_id}));
        
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
        app.setApplicationName("com.some.test.app");

        sync.wait_for_signal_ready();
        
        test::TestHarness harness;
        harness.add_test_case([]()
        {
            QVector<cuc::Item> source_items;
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file1").absoluteFilePath()));
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file2").absoluteFilePath()));
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file3").absoluteFilePath()));
            
            QVector<cuc::Item> expected_items;
            expected_items << cuc::Item(QUrl("file:///tmp/Incoming/file1"));
            expected_items << cuc::Item(QUrl("file:///tmp/Incoming/file2"));
            expected_items << cuc::Item(QUrl("file:///tmp/Incoming/file3"));

            /** [Importing pictures] */
            auto hub = cuc::Hub::Client::instance();
            auto transfer = hub->create_import_for_type_from_peer(
                cuc::Type::Known::pictures(),
                hub->default_peer_for_type(cuc::Type::Known::pictures()));
            ASSERT_TRUE(transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, transfer->state());
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::multiple));
            ASSERT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            transfer->setStore(new cuc::Store{"/tmp/Incoming"});
            EXPECT_TRUE(transfer->start());
            EXPECT_EQ(cuc::Transfer::initiated, transfer->state());
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::single));
            ASSERT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            EXPECT_TRUE(transfer->charge(source_items));
            EXPECT_EQ(cuc::Transfer::charged, transfer->state());
            EXPECT_EQ(expected_items, transfer->collect());
            /** [Importing pictures] */
            hub->quit();
        });
        harness.add_test_case([]()
        {
            QVector<cuc::Item> source_items;
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file1").absoluteFilePath()));
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file2").absoluteFilePath()));
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file3").absoluteFilePath()));

            QVector<cuc::Item> expected_items;
            expected_items << cuc::Item(QUrl("file:///tmp/Outgoing/file1"));
            expected_items << cuc::Item(QUrl("file:///tmp/Outgoing/file2"));
            expected_items << cuc::Item(QUrl("file:///tmp/Outgoing/file3"));

            /** [Exporting pictures] */
            auto hub = cuc::Hub::Client::instance();
            auto transfer = hub->create_export_for_type_from_peer(
                cuc::Type::Known::pictures(),
                hub->default_peer_for_type(cuc::Type::Known::pictures()));
            ASSERT_TRUE(transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, transfer->state());
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::multiple));
            ASSERT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            transfer->setStore(new cuc::Store{"/tmp/Outgoing"});
            EXPECT_TRUE(transfer->start());
            EXPECT_EQ(cuc::Transfer::initiated, transfer->state());
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::single));
            ASSERT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            EXPECT_TRUE(transfer->charge(source_items));
            EXPECT_EQ(cuc::Transfer::charged, transfer->state());
            EXPECT_EQ(expected_items, transfer->collect());
            /** [Exporting pictures] */
            hub->quit();
        });
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
    };
    
    EXPECT_EQ(EXIT_SUCCESS, test::fork_and_run(child, parent));
}
