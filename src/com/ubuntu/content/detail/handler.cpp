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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include "transfer_p.h"
#include "handler.h"
#include "utils.cpp"

#include <QObject>

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

struct cucd::Handler::Private : public QObject
{
    Private(QDBusConnection connection,
            const QString& peer_id,
            QObject* parent)
            : QObject(parent),
              connection(connection),
              peer_id(peer_id)
    {
        qDebug() << Q_FUNC_INFO;
    }

    QDBusConnection connection;
    const QString peer_id;
};

cucd::Handler::Handler(QDBusConnection connection, const QString& peer_id, cuc::ImportExportHandler* handler)
        : d(new Private{connection, peer_id, this})
{
    qDebug() << Q_FUNC_INFO;
    m_handler = handler;
    Q_FOREACH (QDBusObjectPath p, get_transfers(d->peer_id, "export"))
    {
        qDebug() << Q_FUNC_INFO << p.path();
        this->HandleExport(p);
    }
    Q_FOREACH (QDBusObjectPath p, get_transfers(d->peer_id, "import"))
    {
        qDebug() << Q_FUNC_INFO << p.path();
        this->HandleImport(p);
    }
}

cucd::Handler::~Handler() {}

void cucd::Handler::HandleImport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    cuc::Transfer* t = cuc::Transfer::Private::make_transfer(transfer, this);

    qDebug() << Q_FUNC_INFO << "State:" << t->state();
    if (t->state() == cuc::Transfer::charged)
        m_handler->handle_import(t);
}

void cucd::Handler::HandleExport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    cuc::Transfer* t = cuc::Transfer::Private::make_transfer(transfer, this);

    qDebug() << Q_FUNC_INFO << "State:" << t->state();
    if (t->state() == cuc::Transfer::initiated)
        m_handler->handle_export(t);
}

QList<QDBusObjectPath> cucd::Handler::get_transfers(const QString& peer_id, QString type)
{
    qDebug() << Q_FUNC_INFO << peer_id;
    QList<QDBusObjectPath> pending_transfers;

    const QString path_pattern{"/transfers/%1/%2"};
    const QString path = path_pattern.arg(sanitize_path(peer_id)).arg(type);

    QDBusMessage msg = QDBusMessage::createMethodCall(HUB_SERVICE_NAME, path,
                                                      "org.freedesktop.DBus.Introspectable",
                                                      "Introspect");
    QDBusReply<QString> res = d->connection.call(msg);

    QDomDocument doc;
    doc.setContent(res);
    QDomElement node = doc.documentElement();
    QDomElement child = node.firstChildElement();
    while (!child.isNull()) {
        if (child.tagName() == "node") {
            QString transfer_path = path + "/" + child.attribute("name");
            pending_transfers.append(QDBusObjectPath{transfer_path});
        }
        child = child.nextSiblingElement();
    }
    return pending_transfers;
}
