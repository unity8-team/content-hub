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

// NEEDED for libnotify include
#define QT_NO_KEYWORDS

#include "debug.h"
#include "service.h"
#include "peer_registry.h"
#include "i18n.h"
#include "paste.h"
#include "pasteadaptor.h"
#include "transfer.h"
#include "transferadaptor.h"
#include "utils.cpp"
#include "ContentHandlerInterface.h"

#include <glib.h>
#include <unistd.h>
#include <libnotify/notify.h>

#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/paste.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/transfer.h>

#include <QDBusInterface>
#include <QDBusMetaType>
#include <QCache>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QUuid>

#include <cassert>

namespace cua = com::ubuntu::ApplicationManager;
namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

struct cucd::Service::RegHandler
{
    RegHandler(QString id, QString service, cuc::dbus::Handler* handler) : id(id),
        service(service),
        handler(handler)
    {
    }

    QString id;
    QString service;
    cuc::dbus::Handler* handler;
};

struct cucd::Service::Private : public QObject
{
    Private(QDBusConnection connection,
            const QSharedPointer<cucd::PeerRegistry>& registry,
            QSharedPointer<cua::ApplicationManager>& application_manager,
            QObject* parent)
            : QObject(parent),
              connection(connection),
              registry(registry),
              app_manager(application_manager)
    {
        unityFocus = new QDBusInterface("com.canonical.Unity.FocusInfo" /* service */,
                                        "/com/canonical/Unity/FocusInfo" /* object path */,
                                        "com.canonical.Unity.FocusInfo" /* interface */,
                                        QDBusConnection::sessionBus(),
                                        this);
    }

    QDBusConnection connection;
    QSharedPointer<cucd::PeerRegistry> registry;
    QSet<cucd::Transfer*> active_transfers;
    QList<cucd::Paste*> active_pastes;
    QMap<QString, PromptSessionP> inactive_sessions;
    QMap<QString, PromptSessionP> active_sessions;
    QMap<QString, std::string> peer_picker_instances;
    QStringList pasteFormats;
    QSet<RegHandler*> handlers;
    QSharedPointer<cua::ApplicationManager> app_manager;
    QDBusInterface *unityFocus;
    const int maxActivePastes = 5;
};

cucd::Service::Service(QDBusConnection connection, const QSharedPointer<cucd::PeerRegistry>& peer_registry,
                       QSharedPointer<cua::ApplicationManager>& application_manager, QObject* parent)
        : QObject(parent),
          m_watcher(new QDBusServiceWatcher()),
          d(new Private{connection, peer_registry, application_manager, this})
{
    assert(!peer_registry.isNull());

    qDBusRegisterMetaType<cuc::Peer>();
    qDBusRegisterMetaType<cuc::Item>();

    m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    m_watcher->setConnection(d->connection);
    QObject::connect(m_watcher, SIGNAL(serviceUnregistered(const QString&)),
            this,
            SLOT(handler_unregistered(const QString&)));
}

cucd::Service::~Service()
{
    TRACE() << Q_FUNC_INFO;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        TRACE() << Q_FUNC_INFO << "Destroying transfer:" << t->Id();
        delete t;
    }
}

void cucd::Service::Quit()
{
    QCoreApplication::instance()->quit();
}

QVariantList cucd::Service::KnownSourcesForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_sources_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QVariantList cucd::Service::KnownDestinationsForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_destinations_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QVariantList cucd::Service::KnownSharesForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_shares_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QDBusVariant cucd::Service::DefaultSourceForType(const QString& type_id)
{
    cuc::Peer peer = d->registry->default_source_for_type(Type(type_id));

    return QDBusVariant(QVariant::fromValue(peer));
}

QDBusVariant cucd::Service::PeerForId(const QString& app_id)
{
    cuc::Peer peer = cuc::Peer{app_id};

    return QDBusVariant(QVariant::fromValue(peer));
}

void cucd::Service::RequestPeerForTypeByAppId(const QString& type_id, const QString& handler_id, const QString& app_id)
{
    TRACE() << Q_FUNC_INFO << app_id;
    // FIXME: add logic to launch peer picker
    if (d->app_manager->is_application_started(PEER_PICKER_APP_ID.toStdString()))
        d->app_manager->stop_application(PEER_PICKER_APP_ID.toStdString());
    gchar * uris[] = {
        g_strdup(app_id.toStdString().c_str()),
        g_strdup(type_id.toStdString().c_str()),
        g_strdup(handler_id.toStdString().c_str()),
        NULL
    };

    if (!d->active_sessions.keys().contains(app_id)) {
        uint clientPid = d->connection.interface()->servicePid(this->message().service());
        setupPromptSession(app_id, clientPid);
    }

    PromptSessionP session = d->active_sessions.value(app_id);
    if (!session) {
        TRACE() << Q_FUNC_INFO << "Invoking peer picker";
        d->app_manager->invoke_application(PEER_PICKER_APP_ID.toStdString(), uris);
    } else {
        TRACE() << Q_FUNC_INFO << "Invoking application with session";
        std::string instance_id = d->app_manager->invoke_application_with_session(PEER_PICKER_APP_ID.toStdString(), session, uris);
        d->peer_picker_instances[app_id] = instance_id;
    }
}

void cucd::Service::SelectPeerForAppId(const QString& app_id, const QString& peer_id)
{
    TRACE() << Q_FUNC_INFO << app_id << peer_id;
    // FIXME: lock this down to only all the peer picker APP_ID to call this
    if (d->peer_picker_instances.contains(app_id)) {
        /*
        if (d->active_sessions.keys().contains(app_id)) {
                PromptSessionP pSession = d->active_sessions.value(app_id);
                PromptSession* session = pSession.data();
                if (session)
                    session->release();
        }
        */
        std::string instance_id = d->peer_picker_instances.value(app_id);
        d->app_manager->stop_application_with_helper(PEER_PICKER_APP_ID.toStdString(), instance_id);
        d->peer_picker_instances.remove(app_id);
    }
    Q_EMIT(PeerSelected(app_id, peer_id));
}

void cucd::Service::SelectPeerForAppIdCancelled(const QString& app_id)
{
    TRACE() << Q_FUNC_INFO << app_id;
    if (d->peer_picker_instances.contains(app_id)) {
        std::string instance_id = d->peer_picker_instances.value(app_id);
        d->app_manager->stop_application_with_helper(PEER_PICKER_APP_ID.toStdString(), instance_id);
        d->peer_picker_instances.remove(app_id);
        if (d->active_sessions.keys().contains(app_id)) {
                PromptSessionP pSession = d->active_sessions.value(app_id);
                PromptSession* session = pSession.data();
                if (session)
                    session->release();
        }
    }
    Q_EMIT(PeerSelectionCancelled(app_id));
}

QDBusObjectPath cucd::Service::CreateImportFromPeer(const QString& peer_id, const QString& app_id, const QString& type_id)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id << "SERVICE:" << this->message().service();
    QString dest_id = app_id;
    if (dest_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        dest_id = aa_profile(this->message().service());
    }
    return CreateTransfer(dest_id, peer_id, cuc::Transfer::Import, type_id);
}

bool cucd::Service::should_cancel (int st)
{
    TRACE() << Q_FUNC_INFO << "State:" << st;

    return (st != cuc::Transfer::finalized
            && st != cuc::Transfer::collected
            && st != cuc::Transfer::downloaded
            && st != cuc::Transfer::downloading
            && st != cuc::Transfer::aborted);
}

void action_dismiss(NotifyNotification *notification, char *action, gpointer data)
{
    TRACE() << Q_FUNC_INFO;
    Q_UNUSED(notification);
    Q_UNUSED(action);

    cucd::Transfer* t = (cucd::Transfer*)data;
    t->SetShouldBeStartedByContentHub(false);
    t->Charge(QVariantList());
}

void action_accept(NotifyNotification *notification, char *action, gpointer data)
{
    TRACE() << Q_FUNC_INFO;
    Q_UNUSED(notification);
    Q_UNUSED(action);

    cucd::Transfer* t = (cucd::Transfer*)data;
    t->Charge(QVariantList());
}

void cucd::Service::download_notify (cucd::Transfer* t)
{
    TRACE() << Q_FUNC_INFO;
    notify_init(t->source().toStdString().c_str());
    NotifyNotification* notification;

    notification = notify_notification_new (__("Download Complete").toStdString().c_str(),
                                            "",
                                            "save");

    notify_notification_set_hint_string(notification,
                                        "x-canonical-snap-decisions",
                                        "true");

    notify_notification_set_hint_string(notification,
                                        "x-canonical-non-shaped-icon",
                                        "true");

    notify_notification_set_hint_string(notification,
                                        "x-canonical-private-button-tint",
                                        "true");

    notify_notification_add_action (notification,
                                    "action_accept",
                                    __("Open").toStdString().c_str(),
                                    action_accept,
                                    t,
                                    NULL);

    notify_notification_add_action (notification,
                                    "action_dismiss",
                                    __("Dismiss").toStdString().c_str(),
                                    action_dismiss,
                                    t,
                                    NULL);

    GError *error = NULL;
    if (!notify_notification_show(notification, &error)) {
        qWarning() << "Failed to show snap decision:" << error->message;
        g_error_free (error);
    }

}

void cucd::Service::DownloadManagerError(QString errorMessage)
{
    notify_init("Download Manager");
    NotifyNotification* notification;

    notification = notify_notification_new (__("Download Failed").toStdString().c_str(),
                                            errorMessage.toStdString().c_str(),
                                            "save");

    GError *error = NULL;
    if (!notify_notification_show(notification, &error)) {
        qWarning() << "Failed to show download manager error:" << error->message;
        g_error_free (error);
    }

}

QDBusObjectPath cucd::Service::CreateExportToPeer(const QString& peer_id, const QString& app_id, const QString& type_id)
{
    TRACE() << Q_FUNC_INFO;
    QString src_id = app_id;
    if (src_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        src_id = aa_profile(this->message().service());
    }
    return CreateTransfer(peer_id, src_id, cuc::Transfer::Export, type_id);
}

QDBusObjectPath cucd::Service::CreateShareToPeer(const QString& peer_id, const QString& app_id, const QString& type_id)
{
    TRACE() << Q_FUNC_INFO;
    QString src_id = app_id;
    if (src_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        src_id = aa_profile(this->message().service());
    }
    return CreateTransfer(peer_id, src_id, cuc::Transfer::Share, type_id);
}

bool cucd::Service::CreatePaste(const QString& app_id, const QString& surfaceId, const QByteArray& mimeData,
                                const QStringList& types)
{
    TRACE() << Q_FUNC_INFO << app_id << types;

    if (!verifiedSurfaceIsFocused(surfaceId)) {
        return false;
    }

    static size_t import_counter{0}; import_counter++;

    pid_t pid = d->connection.interface()->servicePid(this->message().service());
    qWarning() << Q_FUNC_INFO << "PID: " << pid;
    QString effective_app_id;
    if (app_id_matches(app_id, pid)) {
        effective_app_id = app_id;
    } else {
        qWarning() << "APP_ID" << app_id << "doesn't match requesting APP";
        effective_app_id = "?";
    }

    auto paste = new cucd::Paste(import_counter, effective_app_id, this);
    new PasteAdaptor(paste);
    d->active_pastes.append(paste);

    paste->Charge(mimeData);

    if (d->active_pastes.count() > d->maxActivePastes) {
        // get rid of the oldest one
        delete d->active_pastes.takeFirst();
    }

    Q_EMIT(PasteboardChanged());

    bool pendingPasteFormatsChangedSignal = false;
    Q_FOREACH (QString t, types) {
        TRACE() << Q_FUNC_INFO << "Type: " << t;
        if (!d->pasteFormats.contains(t)) {
            d->pasteFormats.append(t);
            pendingPasteFormatsChangedSignal = true;
        }
    }
    if (pendingPasteFormatsChangedSignal) {
        Q_EMIT(PasteFormatsChanged(d->pasteFormats));
    }

    return true;
}

QByteArray cucd::Service::GetLatestPasteData(const QString& surfaceId)
{
    TRACE() << Q_FUNC_INFO;

    if (d->active_pastes.isEmpty())
        return QByteArray();

    return getPasteData(surfaceId, d->active_pastes.last()->Id());
}

QByteArray cucd::Service::GetPasteData(const QString& surfaceId, const QString& pasteId)
{
    TRACE() << Q_FUNC_INFO << pasteId;

    if (d->active_pastes.isEmpty())
        return QByteArray();

    return getPasteData(surfaceId, pasteId.toInt());
}

QByteArray cucd::Service::getPasteData(const QString &surfaceId, int pasteId)
{
    if (!verifiedSurfaceIsFocused(surfaceId)) {
        qWarning().nospace() << "Surface isn't focused. Denying paste.";
        return QByteArray();
    }

    Q_FOREACH (cucd::Paste *p, d->active_pastes)
    {
        if (p->Id() == pasteId)
            return p->MimeData();
    }
    return QByteArray();
}

QDBusObjectPath cucd::Service::CreateTransfer(const QString& dest_id, const QString& src_id, int dir, const QString& type_id)
{
    TRACE() << Q_FUNC_INFO << "DEST:" << dest_id << "SRC:" << src_id << "DIRECTION:" << dir;

    static size_t import_counter{0}; import_counter++;

    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        if (t->destination() == dest_id || t->source() == src_id)
        {
            TRACE() << Q_FUNC_INFO << "Found transfer for peer_id:" << src_id;
            if (should_cancel(t->State()))
            {
                TRACE() << Q_FUNC_INFO << "Aborting active transfer:" << t->Id();
                t->Abort();
            }
        }
    }

    auto transfer = new cucd::Transfer(import_counter, src_id, dest_id, dir, type_id, this);
    if (dir == cuc::Transfer::Import) {
        uint clientPid = d->connection.interface()->servicePid(this->message().service());
        setupPromptSession(dest_id, clientPid);
    }
    new TransferAdaptor(transfer);
    d->active_transfers.insert(transfer);

    auto destination = transfer->import_path();
    auto source = transfer->export_path();
    if (not d->connection.registerObject(source, transfer))
        TRACE() << "Problem registering object for path: " << source;
    d->connection.registerObject(destination, transfer);

    TRACE() << "Created transfer " << source << " -> " << destination;

    connect(transfer, SIGNAL(DownloadManagerError(QString)), this, SLOT(DownloadManagerError(QString)));

    // Content flow is different for import
    if (dir == cuc::Transfer::Import)
    {
        connect(transfer, SIGNAL(StateChanged(int)), this, SLOT(handle_imports(int)));
        return QDBusObjectPath{destination};
    }

    connect(transfer, SIGNAL(StateChanged(int)), this, SLOT(handle_exports(int)));
    return QDBusObjectPath{source};
}

void cucd::Service::setupPromptSession(QString app_id, uint clientPid)
{
    TRACE() << Q_FUNC_INFO << "APP_ID:" << app_id << "PID:" << clientPid;
    qWarning() << Q_FUNC_INFO << "APP_ID:" << app_id << "PID:" << clientPid << "SESSIONS:" << d->active_sessions.keys();
    if (d->active_sessions.keys().contains(app_id))
        return;

    if (!m_mirHelper) {
        TRACE() << "No MirHelper, creating one";
        qWarning() << "No MirHelper, creating one";
        m_mirHelper = MirHelper::instance();
    }
    PromptSessionP session = m_mirHelper->createPromptSession(clientPid);
    if (!session) return;

    QString mirSocket = session->requestSocket();
    TRACE() << Q_FUNC_INFO << "mirSocket:" << mirSocket;
    qWarning() << Q_FUNC_INFO << "mirSocket:" << mirSocket;

    QObject::connect(session.data(), SIGNAL(finished()),
                     this, SLOT(onPromptFinished()));
    d->active_sessions[app_id] = session;
}

void cucd::Service::onPromptFinished()
{
    TRACE() << Q_FUNC_INFO;
    PromptSession *session = static_cast<PromptSession*>(sender());

    Q_FOREACH(QString key, d->active_sessions.keys()) {
        PromptSessionP pSession = d->active_sessions.value(key);
        if (session == pSession.data()) {
            qWarning() << "Removing session for" << key;
            d->inactive_sessions[key] = d->active_sessions.value(key);
            d->active_sessions.remove(key);
        }
    }
}

void cucd::Service::handle_imports(int state)
{
    TRACE() << Q_FUNC_INFO << state;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());
    TRACE() << Q_FUNC_INFO << "State: " << transfer->State() << "Id:" << transfer->Id();

    if (state == cuc::Transfer::initiated)
    {
        TRACE() << Q_FUNC_INFO << "initiated";
        qWarning() << "WasSourceStartedByContentHub:" << transfer->WasSourceStartedByContentHub();
        if (d->app_manager->is_application_started(transfer->source().toStdString()))
            transfer->SetSourceStartedByContentHub(false);
        else
            transfer->SetSourceStartedByContentHub(true);
        qWarning() << "WasSourceStartedByContentHub:" << transfer->WasSourceStartedByContentHub();

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << Q_FUNC_INFO << "ID:" << r->id << "Handler: " << r->service << "Transfer: " << transfer->source();
            if (r->id == transfer->source())
            {
                TRACE() << Q_FUNC_INFO << "Found handler for initiated transfer" << r->id;
                if (r->handler->isValid())
                    r->handler->HandleExport(QDBusObjectPath{transfer->export_path()});
                else
                    TRACE() << Q_FUNC_INFO << "Handler invalid";
            }
        }

        if (!d->active_sessions.keys().contains(transfer->destination())) {
            uint clientPid = d->connection.interface()->servicePid(this->message().service());
            setupPromptSession(transfer->destination(), clientPid);
        }

        qWarning() << "DEST:" << transfer->destination() << "KEYS:" << d->active_sessions.keys();
        PromptSessionP session = d->active_sessions.value(transfer->destination());
        if (!session) {
            TRACE() << Q_FUNC_INFO << "Invoking application";
            gchar ** uris = NULL;
            d->app_manager->invoke_application(transfer->source().toStdString(), uris);
        } else {
            TRACE() << Q_FUNC_INFO << "Invoking application with session";
            gchar ** uris = NULL;
            std::string instance_id = d->app_manager->invoke_application_with_session(transfer->source().toStdString(), session, uris);
            transfer->SetInstanceId(QString::fromStdString(instance_id));
        }
    }

    if (state == cuc::Transfer::charged)
    {
        TRACE() << Q_FUNC_INFO << "Charged";
        if (!transfer->InstanceId().isEmpty()) {
            d->app_manager->stop_application_with_helper(transfer->source().toStdString(), transfer->InstanceId().toStdString());
        } else {
            d->app_manager->stop_application(transfer->source().toStdString());
        }

        gchar ** uris = NULL;
        if (d->registry->peer_is_legacy(transfer->destination())) {
            TRACE() << Q_FUNC_INFO << "Destination is a legacy app, collecting";
            transfer->SetStore(shared_dir_for_peer(transfer->destination()));
            auto items = transfer->Collect();
            gchar* urls[2] = {0};
            gint i = 0;
            Q_FOREACH (QVariant item, items) {
                QString s = copy_to_store(item.value<cuc::Item>().url().toString(), transfer->Store()).split("/shared")[1];
                QUrl u = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/shared" + s);
                urls[i] = g_str_to_ascii(u.toString().toStdString().c_str(), NULL);
                i++;
            }
            uris = (gchar **)urls;
        }

        if (transfer->ShouldBeStartedByContentHub())
            d->app_manager->invoke_application(transfer->destination().toStdString(), uris);

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << Q_FUNC_INFO << "ID:" << r->id << "Handler: " << r->service << "Transfer: " << transfer->destination();
            if (r->id == transfer->destination())
            {
                TRACE() << Q_FUNC_INFO << "Found handler for charged transfer" << r->id;
                if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{transfer->import_path()});
            }
        }
    }

    if (state == cuc::Transfer::aborted)
    {
        if (transfer->WasSourceStartedByContentHub())
        {
            bool shouldStop = true;
            Q_FOREACH (cucd::Transfer *t, d->active_transfers)
            {
                if (t->Id() != transfer->Id())
                {
                    if ((t->source() == transfer->source()) && (t->State() == cuc::Transfer::in_progress))
                    {
                        TRACE() << Q_FUNC_INFO << "Source has pending transfers:" << t->Id();
                        shouldStop = false;
                    }
                    if (t->destination() == transfer->destination())
                    {
                        TRACE() << Q_FUNC_INFO << "Destination has pending transfers:" << t->Id();
                        if (should_cancel(t->State()))
                            shouldStop = false;
                    }
                }
            }
            if (shouldStop) {
                if (!transfer->InstanceId().isEmpty()) {
                    d->app_manager->stop_application_with_helper(transfer->source().toStdString(), transfer->InstanceId().toStdString());
                } else {
                    d->app_manager->stop_application(transfer->source().toStdString());
                }
            }
        }
        gchar ** uris = NULL;
        d->app_manager->invoke_application(transfer->destination().toStdString(), uris);
    }
}

void cucd::Service::handle_exports(int state)
{
    TRACE() << Q_FUNC_INFO;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());

    TRACE() << Q_FUNC_INFO << "STATE:" << transfer->State();


    if (state == cuc::Transfer::initiated)
    {
        TRACE() << Q_FUNC_INFO << "Initiated";
        transfer->Handled();
    }

    if (state == cuc::Transfer::downloaded)
    {
        TRACE() << Q_FUNC_INFO << "Downloaded";
        download_notify(transfer);
    }

    if (state == cuc::Transfer::charged)
    {
        TRACE() << Q_FUNC_INFO << "Charged";
        if (d->app_manager->is_application_started(transfer->destination().toStdString()))
            transfer->SetSourceStartedByContentHub(false);
        else
            transfer->SetSourceStartedByContentHub(true);

        gchar ** uris = NULL;
        if (d->registry->peer_is_legacy(transfer->destination())) {
            TRACE() << Q_FUNC_INFO << "Destination is a legacy app, collecting";
            transfer->SetStore(shared_dir_for_peer(transfer->destination()));

            auto items = transfer->Collect();
            gchar* urls[2] = {0};
            gint i = 0;
            Q_FOREACH (QVariant item, items) {
                QString s = copy_to_store(item.value<cuc::Item>().url().toString(), transfer->Store()).split("/shared")[1];
                QUrl u = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/shared" + s);
                urls[i] = g_str_to_ascii(u.toString().toStdString().c_str(), NULL);
                i++;
            }
            uris = (gchar **)urls;
        }

        if (transfer->ShouldBeStartedByContentHub())
            d->app_manager->invoke_application(transfer->destination().toStdString(), uris);

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << "Handler: " << r->service << "Transfer: " << transfer->destination();
            if (r->id == transfer->destination())
            {
                TRACE() << "Found handler for charged transfer" << r->id;
                if (transfer->Direction() == cuc::Transfer::Share && r->handler->isValid())
                    r->handler->HandleShare(QDBusObjectPath{transfer->import_path()});
                else if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{transfer->import_path()});
            }
        }
    }

    if (state == cuc::Transfer::aborted)
    {
        TRACE() << Q_FUNC_INFO << "Aborted";
        if (transfer->WasSourceStartedByContentHub())
        {
            bool shouldStop = true;
            Q_FOREACH (cucd::Transfer *t, d->active_transfers)
            {
                if (t->Id() != transfer->Id())
                {
                    if ((t->source() == transfer->source()) && (t->State() == cuc::Transfer::in_progress))
                    {
                        TRACE() << Q_FUNC_INFO << "Source has pending transfers:" << t->Id();
                        shouldStop = false;
                    }
                    if (t->destination() == transfer->destination())
                    {
                        TRACE() << Q_FUNC_INFO << "Destination has pending transfers:" << t->Id();
                        if (should_cancel(t->State()))
                            shouldStop = false;
                    }
                }
            }
            if (shouldStop) {
                d->app_manager->stop_application(transfer->destination().toStdString());
            }
        }
        gchar ** uris = NULL;
        d->app_manager->invoke_application(transfer->source().toStdString(), uris);
    }
}

void cucd::Service::handler_unregistered(const QString& s)
{
    TRACE() << Q_FUNC_INFO << s;

    if (d->handlers.isEmpty())
        return;

    Q_FOREACH (RegHandler *r, d->handlers)
    {
        TRACE() << "Handler: " << r->id;
        if (r->service == s)
        {
            TRACE() << "Found match for " << r->id;
            d->handlers.remove(r);
            m_watcher->removeWatchedService(s);
            delete r;
        }
    }
}

void cucd::Service::RegisterImportExportHandler(const QString& peer_id, const QDBusObjectPath& handler)
{
    TRACE() << Q_FUNC_INFO << peer_id;
    bool exists = false;
    RegHandler* r;
    Q_FOREACH (RegHandler *rh, d->handlers)
    {
        TRACE() << "Handler: " << rh->id;
        if (rh->id == peer_id)
        {
            TRACE() << "Found existing handler for " << rh->id;
            exists = true;
            r = rh;
        }
    }

    if (!exists)
    {
        r = new RegHandler{peer_id,
            this->message().service(),
            new cuc::dbus::Handler(
                    this->message().service(),
                    handler.path(),
                    QDBusConnection::sessionBus(),
                    0)};
        d->handlers.insert(r);
        m_watcher->addWatchedService(r->service);
    }

    TRACE() << Q_FUNC_INFO << r->id;

    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        TRACE() << Q_FUNC_INFO << "SOURCE: " << t->source() << "DEST:" << t->destination() << "STATE:" << t->State();
        if ((t->source() == peer_id) && (t->State() == cuc::Transfer::initiated))
        {
            TRACE() << Q_FUNC_INFO << "Found source:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Import)
            {
                if (r->handler->isValid())
                    r->handler->HandleExport(QDBusObjectPath{t->export_path()});
            }
        }
        else if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::charged))
        {
            TRACE() << Q_FUNC_INFO << "Found destination:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Export)
            {
                TRACE() << Q_FUNC_INFO << "Found import, calling HandleImport";
                if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{t->import_path()});
            } else if (t->Direction() == cuc::Transfer::Share)
            {
                TRACE() << Q_FUNC_INFO << "Found share, calling HandleShare";
                if (r->handler->isValid())
                    r->handler->HandleShare(QDBusObjectPath{t->import_path()});
            }
        }
        else if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::downloaded))
        {
            TRACE() << Q_FUNC_INFO << "Found destination:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Export)
            {
                TRACE() << Q_FUNC_INFO << "Found downloaded import, charging";
                if (r->handler->isValid())
                    t->Charge(QVariantList());
            }
        }
    }
}

void cucd::Service::HandlerActive(const QString& peer_id)
{
    TRACE() << Q_FUNC_INFO << peer_id;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::downloaded))
        {
            TRACE() << Q_FUNC_INFO << "Found destination:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Export)
            {
                TRACE() << Q_FUNC_INFO << "Found downloaded import, charging";
                t->Charge(QVariantList());
            }
        }
    }
}

bool cucd::Service::HasPending(const QString& peer_id)
{
    TRACE() << Q_FUNC_INFO << peer_id;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        TRACE() << Q_FUNC_INFO << "SOURCE: " << t->source() << "DEST:" << t->destination() << "STATE:" << t->State();
        if (((t->source() == peer_id) || (t->destination() == peer_id)) && (t->State() == cuc::Transfer::initiated))
        {
            TRACE() << Q_FUNC_INFO << "Has pending:" << peer_id;
            return true;
        }
    }
    return false;
}

QStringList cucd::Service::PasteFormats()
{
    TRACE() << Q_FUNC_INFO;
    return d->pasteFormats;
}

bool cucd::Service::verifiedSurfaceIsFocused(const QString &surfaceId)
{
    /* Only verify focus when not running under testing */
    if (!qgetenv("CONTENT_HUB_TESTING").isNull())
        return true;

    return d->unityFocus->call("isSurfaceFocused", surfaceId).arguments().at(0).toBool();
}
