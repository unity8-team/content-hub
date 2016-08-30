/*
 * Copyright (C) 2014-2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "../../src/com/ubuntu/content/utils.cpp"

#include <QMimeData>

TEST(PasteBoardTest, MimeDataSerialization)
{
    QMimeData mimeData;
    mimeData.setData("text/plain", "Hello World!");
    mimeData.setData("text/html", "<html lang=\"en\"><body>Hello World!</body></html>");

    QByteArray serializedMimeData = serializeMimeData(const_cast<const QMimeData&>(mimeData));

    ASSERT_TRUE(serializedMimeData.size() > 0);

    QMimeData *deserializedMimeData = deserializeMimeData(serializedMimeData);

    ASSERT_TRUE(deserializedMimeData != nullptr);

    ASSERT_TRUE(deserializedMimeData->hasFormat("text/plain"));
    ASSERT_EQ(mimeData.data("text/plain"), deserializedMimeData->data("text/plain"));

    ASSERT_TRUE(deserializedMimeData->hasFormat("text/html"));
    ASSERT_EQ(mimeData.data("text/html"), deserializedMimeData->data("text/html"));

    delete deserializedMimeData;
}
