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

TEST(Hub, querying_known_peers_returns_correct_value)
{
    using namespace ::testing;

    test::CrossProcessSync sync;
    
    QVector<cuc::Peer> default_peers;
    default_peers << cuc::Peer("com.does.not.exist.anywhere.application1");
    default_peers << cuc::Peer("com.does.not.exist.anywhere.application2");
    default_peers << cuc::Peer("com.does.not.exist.anywhere.application3");

    auto parent = [&sync, default_peers]()
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();        

        auto enumerate = [default_peers](cuc::Type, const std::function<void(const cuc::Peer&)>& f)
        {
            Q_FOREACH(const cuc::Peer& peer, default_peers)
            {
                f(peer);
            }
        };
        
        auto mock = new MockedPeerRegistry{};
        EXPECT_CALL(*mock, enumerate_known_peers_for_type(_, _)).
        Times(Exactly(1)).
        WillRepeatedly(Invoke(enumerate));

        EXPECT_CALL(*mock, install_peer_for_type(_, _)).
        Times(Exactly(1)).
        WillRepeatedly(Return(true));

        ASSERT_TRUE(mock->install_peer_for_type(cuc::Type::Known::documents(),
                                                cuc::Peer("com.does.not.exist.anywhere.application4")));

        QSharedPointer<cucd::PeerRegistry> registry{mock};
        
        auto app_manager = QSharedPointer<cua::ApplicationManager>(new MockedAppManager());

        auto implementation = new cucd::Service(connection, registry, app_manager, &app);
        new ServiceAdaptor(implementation);

        ASSERT_TRUE(connection.registerService(service_name));
        ASSERT_TRUE(connection.registerObject("/", implementation));

        sync.signal_ready();

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);
    };

    auto child = [&sync, default_peers]()
    {
        sync.wait_for_signal_ready();
        
        int argc = 0;
        QCoreApplication app(argc, nullptr);
        
        auto hub = cuc::Hub::Client::instance();
        
        test::TestHarness harness;
        harness.add_test_case([hub, default_peers]()
        {            
            auto peers = hub->known_peers_for_type(cuc::Type::Known::documents());
            ASSERT_EQ(default_peers, peers);
        });
        
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
        
        hub->quit();

    };

    EXPECT_TRUE(test::fork_and_run(child, parent) != EXIT_FAILURE);
}
