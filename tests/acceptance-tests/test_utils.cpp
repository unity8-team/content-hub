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

#include "com/ubuntu/content/utils.cpp"

#include <gtest/gtest.h>

void PrintTo(const QString& s, ::std::ostream* os) {
    *os << std::string(qPrintable(s));
}

QString persistent_path{"/tmp/.local/share/testing/Pictures"};
QString temp_path{"/tmp/.cache/testing/HubIncoming/1"};

TEST(Utils, is_persistent)
{
    using namespace ::testing;

    EXPECT_TRUE(is_persistent(persistent_path));
    EXPECT_FALSE(is_persistent(temp_path));
}

TEST(Utils, purge_store_cache)
{
    using namespace ::testing;

    QDir persistent_store(persistent_path);
    persistent_store.mkpath(persistent_store.absolutePath());
    QDir temp_store(temp_path);
    temp_store.mkpath(temp_store.absolutePath());

    EXPECT_TRUE(purge_store_cache(temp_store.absolutePath()));
    EXPECT_FALSE(temp_store.exists());

    EXPECT_FALSE(purge_store_cache(persistent_store.absolutePath()));
    EXPECT_TRUE(persistent_store.exists());
}
