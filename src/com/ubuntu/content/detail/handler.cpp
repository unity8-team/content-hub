/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
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

#include "handler.h"

#include "transfer.h"
#include "transferadaptor.h"

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>

#include <QCache>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QUuid>

#include <cassert>

namespace cucd = com::ubuntu::content::detail;

struct cucd::Handler::Private : public QObject
{
    Private(QDBusConnection connection,
            QObject* parent)
            : QObject(parent),
              connection(connection)
    {
    }

    QDBusConnection connection;
};

cucd::Handler::Handler(QDBusConnection connection, QObject* parent)
        : QObject(parent),
          d(new Private{connection, this})
{
}

cucd::Handler::~Handler() {}

void cucd::Handler::HandleImport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(transfer);
}

void cucd::Handler::HandleExport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(transfer);
}
