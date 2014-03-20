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

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include "com/ubuntu/content/detail/peer_registry.h"
#include "com/ubuntu/content/service/hook.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QtTest/QTest>

namespace cuc = com::ubuntu::content;

void PrintTo(const QString& s, ::std::ostream* os) {
    *os << std::string(qPrintable(s));
}

namespace
{
struct MockedRegistry : public cuc::detail::PeerRegistry
{
    MockedRegistry() : PeerRegistry()
    {
        using namespace ::testing;

        ON_CALL(*this, default_source_for_type(_)).WillByDefault(Return(cuc::Peer::unknown()));
        ON_CALL(*this, install_default_source_for_type(_,_)).WillByDefault(Return(false));
        ON_CALL(*this, install_source_for_type(_,_)).WillByDefault(Return(false));
        ON_CALL(*this, remove_peer(_)).WillByDefault(Return(false));
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
};
}

TEST(Hook, parse_json)
{
    using namespace ::testing;
    
    auto mock = new MockedRegistry{};
    EXPECT_CALL(*mock, install_source_for_type(_,_)).
    Times(Exactly(2)).
    WillRepeatedly(Return(true));

    QFileInfo f("good.json");
    cucd::Hook *hook = new cucd::Hook(mock);

    EXPECT_TRUE(hook->add_peer(f));
    f.setFile("bad.json");
    EXPECT_FALSE(hook->add_peer(f));
    delete mock;
}
