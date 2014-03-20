/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COM_UBUNTU_CONTENTHANDLER_H_
#define COM_UBUNTU_CONTENTHANDLER_H_

#include <QObject>

class ContentHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS(Handler)

public:
    enum Handler {
        Source = 0,
        Destination = 1,
        Share = 2
    };

    ContentHandler(QObject *parent = nullptr);
};

#endif // COM_UBUNTU_CONTENTHANDLER_H_
