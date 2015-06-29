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

#include <com/ubuntu/content/type.h>

#include <gtest/gtest.h>

namespace cuc = com::ubuntu::content;

TEST(Types, id_documents)
{
    EXPECT_EQ(cuc::Type::Known::documents().id(), "documents");
}

TEST(Types, id_pictures)
{
    EXPECT_EQ(cuc::Type::Known::pictures().id(), "pictures");
}

TEST(Types, id_music)
{
    EXPECT_EQ(cuc::Type::Known::music().id(), "music");
}

TEST(Types, id_contacts)
{
    EXPECT_EQ(cuc::Type::Known::contacts().id(), "contacts");
}

TEST(Types, id_videos)
{
    EXPECT_EQ(cuc::Type::Known::videos().id(), "videos");
}

TEST(Types, id_links)
{
    EXPECT_EQ(cuc::Type::Known::links().id(), "links");
}

TEST(Types, id_ebooks)
{
    EXPECT_EQ(cuc::Type::Known::ebooks().id(), "ebooks");
}

TEST(Types, id_text)
{
    EXPECT_EQ(cuc::Type::Known::text().id(), "text");
}

TEST(Types, id_events)
{
    EXPECT_EQ(cuc::Type::Known::events().id(), "events");
}
