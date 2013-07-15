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

#include "test_harness.h"
#include "../cross_process_sync.h"
#include "../fork_and_run.h"

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/store.h>
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

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

void PrintTo(const QString& s, ::std::ostream* os) {
    *os << std::string(qPrintable(s));
}

namespace
{
QString service_name{"com.ubuntu.content.Service"};

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

TEST(Hub, stores_are_reported_correctly_to_clients)
{
    test::CrossProcessSync sync;
    
    auto parent = [&sync]()
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();        

        QSharedPointer<cucd::PeerRegistry> registry{new MockedPeerRegistry{}};
        auto implementation = new cucd::Service(registry, &app);
        new ServiceAdaptor(implementation);

        ASSERT_TRUE(connection.registerService(service_name));
        ASSERT_TRUE(connection.registerObject("/", implementation));

        sync.signal_ready();

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);

        delete implementation;
    };

    auto child = [&sync]()
    {
        sync.wait_for_signal_ready();
        
        test::TestHarness harness;
        harness.add_test_case([]()
        {
            auto hub = cuc::Hub::Client::instance();
            
            auto system_wide_pictures_store = hub->store_for_scope_and_type(cuc::Scope::system, cuc::Type::Known::pictures());
            auto system_wide_music_store = hub->store_for_scope_and_type(cuc::Scope::system, cuc::Type::Known::music());
            auto system_wide_documents_store = hub->store_for_scope_and_type(cuc::Scope::system, cuc::Type::Known::documents());
            
            auto users_pictures_store = hub->store_for_scope_and_type(cuc::Scope::user, cuc::Type::Known::pictures());
            auto users_music_store = hub->store_for_scope_and_type(cuc::Scope::user, cuc::Type::Known::music());
            auto users_documents_store = hub->store_for_scope_and_type(cuc::Scope::user, cuc::Type::Known::documents());
            
            auto apps_pictures_store = hub->store_for_scope_and_type(cuc::Scope::app, cuc::Type::Known::pictures());
            auto apps_music_store = hub->store_for_scope_and_type(cuc::Scope::app, cuc::Type::Known::music());
            auto apps_documents_store = hub->store_for_scope_and_type(cuc::Scope::app, cuc::Type::Known::documents());
            
            EXPECT_EQ(system_wide_pictures_store->uri(), "/content/Pictures");
            EXPECT_EQ(system_wide_music_store->uri(), "/content/Music");
            EXPECT_EQ(system_wide_documents_store->uri(), "/content/Documents");
            
            auto ups = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
            auto ums = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
            auto uds = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            EXPECT_EQ(users_pictures_store->uri(), ups);
            EXPECT_EQ(users_music_store->uri(), ums);
            EXPECT_EQ(users_documents_store->uri(), uds);
            
            auto aps = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Pictures";
            auto ams = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Music";
            auto ads = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Documents";
            EXPECT_EQ(apps_pictures_store->uri(), aps);
            EXPECT_EQ(apps_music_store->uri(), ams);
            EXPECT_EQ(apps_documents_store->uri(), ads);

            hub->quit();
        });
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
    };
    
    EXPECT_EQ(EXIT_SUCCESS, test::fork_and_run(child, parent));
}
