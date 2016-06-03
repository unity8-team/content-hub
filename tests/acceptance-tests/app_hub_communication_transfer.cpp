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
#include <QTemporaryDir>
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

TEST_F(Hub, transfer_creation_and_states_work)
{
    using namespace ::testing;

    core::testing::CrossProcessSync sync;
    
    auto service = [this, &sync]() -> core::posix::exit::Status
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QString default_peer_id{"com.does.not.exist.anywhere.application"};

        QDBusConnection connection = QDBusConnection::sessionBus();        
        
        auto mock = new ::testing::NiceMock<MockedPeerRegistry>{};
        EXPECT_CALL(*mock, default_source_for_type(_)).
        Times(AtLeast(1)).
        WillRepeatedly(Return(cuc::Peer{default_peer_id}));
        
        QSharedPointer<cucd::PeerRegistry> registry{mock};
        auto app_manager = QSharedPointer<cua::ApplicationManager>(new ::testing::NiceMock<MockedAppManager>);
        cucd::Service implementation(connection, registry, app_manager, &app);
        new ServiceAdaptor(std::addressof(implementation));

        EXPECT_TRUE(connection.registerService(service_name));
        EXPECT_TRUE(connection.registerObject("/", std::addressof(implementation)));

        sync.try_signal_ready_for(std::chrono::milliseconds{500});

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);
        return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
    };

    auto client = [this, &sync]() -> core::posix::exit::Status
    {
        int argc = 0;
        QCoreApplication app(argc, nullptr);
        app.setApplicationName("com.some.test.app");

        sync.wait_for_signal_ready_for(std::chrono::milliseconds{500});
        
        test::TestHarness harness;
        harness.add_test_case([]()
        {
            QTemporaryDir store_dir;
            QVector<cuc::Item> source_items;
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file1").absoluteFilePath()));
            source_items[0].setName("name1");
            source_items << cuc::Item(QUrl::fromLocalFile(QFileInfo("file2").absoluteFilePath()));
            source_items[1].setName("name2");
            source_items << cuc::Item();
            source_items[2].setName("name3");
            source_items[2].setText("data3");
            
            QVector<cuc::Item> expected_items;
            expected_items << cuc::Item(QUrl("file://" + store_dir.path() + "/file1"));
            expected_items[0].setName("name1");
            expected_items << cuc::Item(QUrl("file://" + store_dir.path() + "/file2"));
            expected_items[1].setName("name2");
            expected_items << cuc::Item();
            expected_items[2].setName("name3");
            expected_items[2].setText("data3");

            /** [Importing pictures] */
            auto hub = cuc::Hub::Client::instance();
            auto transfer = hub->create_import_from_peer(
                hub->default_source_for_type(cuc::Type::Known::pictures()));
            EXPECT_TRUE(transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, transfer->state());
            EXPECT_FALSE(hub->has_pending(transfer->destination()));
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::multiple));
            EXPECT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            transfer->setStore(new cuc::Store{store_dir.path()});
            EXPECT_TRUE(transfer->start());
            EXPECT_EQ(cuc::Transfer::initiated, transfer->state());
            EXPECT_TRUE(hub->has_pending(transfer->destination()));
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::single));
            EXPECT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            EXPECT_TRUE(transfer->charge(source_items));
            EXPECT_EQ(cuc::Transfer::charged, transfer->state());
            EXPECT_EQ(expected_items, transfer->collect());
            /** [Importing pictures] */

            /* Test that only a single transfer exists for the same peer */
            auto single_transfer = hub->create_import_from_peer(
                hub->default_source_for_type(cuc::Type::Known::pictures()));
            EXPECT_TRUE(single_transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, single_transfer->state());
            EXPECT_TRUE(single_transfer->start());
            EXPECT_EQ(cuc::Transfer::initiated, single_transfer->state());

            auto second_transfer = hub->create_import_from_peer(
                hub->default_source_for_type(cuc::Type::Known::pictures()));
            EXPECT_TRUE(second_transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, second_transfer->state());
            /* Now that a second transfer was created, the previous
             * transfer should have been aborted */
            EXPECT_EQ(cuc::Transfer::aborted, single_transfer->state());
            /* end single transfer test */

            /* Test create_import_from_peer_for_type */
            auto type_transfer = hub->create_import_from_peer_for_type(
                hub->default_source_for_type(cuc::Type::Known::pictures()),
                cuc::Type::Known::pictures());
            EXPECT_TRUE(type_transfer != nullptr);
            EXPECT_EQ(cuc::Transfer::created, type_transfer->state());
            EXPECT_TRUE(transfer->setSelectionType(cuc::Transfer::SelectionType::multiple));
            EXPECT_EQ(cuc::Transfer::SelectionType::multiple, transfer->selectionType());
            EXPECT_TRUE(type_transfer->start());
            EXPECT_EQ(cuc::Transfer::initiated, type_transfer->state());
            EXPECT_EQ(cuc::Type::Known::pictures().id(), type_transfer->contentType());
            EXPECT_TRUE(type_transfer->abort());
            EXPECT_EQ(cuc::Transfer::aborted, type_transfer->state());
            /* end create_import_from_peer_for_type test */

            hub->quit();
        });
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
        return ::testing::Test::HasFailure() ? core::posix::exit::Status::failure : core::posix::exit::Status::success;
    };
    
    EXPECT_EQ(core::testing::ForkAndRunResult::empty, core::testing::fork_and_run(service, client));
}
