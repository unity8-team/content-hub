/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <com/ubuntu/content/transfer.h>
#include <QObject>
#include <QDebug>

class Example : public QObject
{
    Q_OBJECT
public:
    explicit Example(QObject *parent = 0);
    void create_share();

private:
    com::ubuntu::content::Transfer *m_transfer;
};

#endif // EXAMPLE_H
