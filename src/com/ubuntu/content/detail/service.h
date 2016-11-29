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
#ifndef SERVICE_H_
#define SERVICE_H_

#include <QObject>
#include <QStringList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusContext>
#include <QDBusUnixFileDescriptor>

#include <com/ubuntu/applicationmanager/application_manager.h>
#include "handler.h"
#include "mir-helper.h"
#include "transfer.h"

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class PeerRegistry;

class Service : public QObject, protected QDBusContext
{
    Q_OBJECT
  public:
    Service(QDBusConnection connection,
            const QSharedPointer<PeerRegistry>& registry,
            QSharedPointer<com::ubuntu::ApplicationManager::ApplicationManager>& application_manager,
            QObject* parent = nullptr);
    Service(const Service&) = delete;
    ~Service();

    Service& operator=(const Service&) = delete;

  public Q_SLOTS:
    QDBusVariant DefaultSourceForType(const QString &type_id);
    QVariantList KnownSourcesForType(const QString &type_id);
    QVariantList KnownDestinationsForType(const QString &type_id);
    QVariantList KnownSharesForType(const QString &type_id);
    QDBusObjectPath CreateImportFromPeer(const QString&, const QString&, const QString&);
    QDBusObjectPath CreateExportToPeer(const QString&, const QString&, const QString&);
    QDBusObjectPath CreateShareToPeer(const QString&, const QString&, const QString&);
    bool CreatePaste(const QString&, const QString&, const QByteArray&, const QStringList&);
    bool RemovePaste(const QString& surfaceId, const QString& pasteId);
    QString GetPasteSource(const QString& surfaceId, const QString& pasteId);
    QByteArray GetLatestPasteData(const QString& surfaceId);
    QByteArray GetPasteData(const QString& surfaceId, const QString& pasteId);
    QStringList GetAllPasteIds(const QString& surfaceId);
    QStringList PasteFormats();
    void RequestPasteByAppId(const QString&);
    void SelectPasteForAppId(const QString&, const QString&);
    void SelectPasteForAppIdCancelled(const QString&);
 
    void RegisterImportExportHandler(const QString&, const QDBusObjectPath& handler);
    void HandlerActive(const QString&);
    void Quit();
    void DownloadManagerError(QString);
    bool HasPending(const QString&);
    QDBusVariant PeerForId(const QString&);
    void RequestPeerForTypeByAppId(const QString&, const QString&, const QString&);
    void SelectPeerForAppId(const QString&, const QString&);
    void SelectPeerForAppIdCancelled(const QString&);
    void onPromptFinished();

  private:
    QString getPasteSource(const QString &surfaceId, int pasteId);
    QByteArray getPasteData(const QString &surfaceId, int pasteId);
    QStringList getAllPasteIds(const QString &surfaceId);
    bool should_cancel(int);
    bool verifiedSurfaceIsFocused(const QString &surfaceId);
    struct Private;
    struct RegHandler;
    QDBusServiceWatcher* m_watcher;
    QScopedPointer<Private> d;

  Q_SIGNALS:
    void PasteFormatsChanged(const QStringList &formats);
    void PasteboardChanged();
    void PeerSelected(const QString &app_id, const QString &peer_id);
    void PeerSelectionCancelled(const QString &app_id);
    void PasteSelected(const QString &app_id, const QString &paste);
    void PasteSelectionCancelled(const QString &app_id);

  private Q_SLOTS:
    void handle_imports(int);
    void handle_exports(int);
    void handler_unregistered(const QString&);
    QDBusObjectPath CreateTransfer(const QString&, const QString&, int, const QString&);
    void setupPromptSession(QString, uint);
    void download_notify(com::ubuntu::content::detail::Transfer*);
};
}
}
}
}

#endif // SERVICE_H_
