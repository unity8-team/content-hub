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

TEST(Hub, querying_default_peer_returns_correct_value)
{
    using namespace ::testing;

    test::CrossProcessSync sync;
    
    QString default_peer_id{"com.does.not.exist.anywhere.application"};

    auto parent = [&sync, default_peer_id]()
    {
        int argc = 0;
        QCoreApplication app{argc, nullptr};

        QDBusConnection connection = QDBusConnection::sessionBus();        
        
        auto mock = new MockedPeerRegistry{};
        EXPECT_CALL(*mock, default_peer_for_type(_)).
        Times(Exactly(1)).
        WillRepeatedly(Return(cuc::Peer{default_peer_id}));

        QSharedPointer<cucd::PeerRegistry> registry{mock};
        
        auto implementation = new cucd::Service(registry, &app);
        new ServiceAdaptor(implementation);

        ASSERT_TRUE(connection.registerService(service_name));
        ASSERT_TRUE(connection.registerObject("/", implementation));

        sync.signal_ready();

        app.exec();

        connection.unregisterObject("/");
        connection.unregisterService(service_name);        
    };

    auto child = [&sync, default_peer_id]()
    {
        sync.wait_for_signal_ready();
        
        int argc = 0;
        QCoreApplication app(argc, nullptr);
        
        auto hub = cuc::Hub::Client::instance();
        
        test::TestHarness harness;
        harness.add_test_case([hub, default_peer_id]()
        {            
            EXPECT_EQ(default_peer_id, hub->default_peer_for_type(cuc::Type::Known::documents()).id());
        });
        
        EXPECT_EQ(0, QTest::qExec(std::addressof(harness)));
        
        hub->quit();

    };

    EXPECT_TRUE(test::fork_and_run(child, parent) != EXIT_FAILURE);
}
