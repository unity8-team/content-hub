/*
 * Copyright © 2013,2016 Canonical Ltd.
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
#ifndef COM_UBUNTU_CONTENT_HUB_H_
#define COM_UBUNTU_CONTENT_HUB_H_

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/type.h>

#include <QObject>
#include <QMimeData>

class QStringList;
class QDBusPendingCall;

namespace com
{
namespace ubuntu
{
namespace content
{
class ImportExportHandler;
class Store;
class Transfer;

class Hub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList pasteFormats READ pasteFormats NOTIFY pasteFormatsChanged)
    Q_PROPERTY(bool valid READ isValid CONSTANT)

  public:
    struct Client
    {
        static Hub* instance();
    };

    Hub(const Hub&) = delete;
    virtual ~Hub();
    Hub& operator=(const Hub&) = delete;

    bool isValid() const;

    Q_INVOKABLE virtual void register_import_export_handler(ImportExportHandler* handler);
    Q_INVOKABLE virtual const Store* store_for_scope_and_type(Scope scope, Type type);
    Q_INVOKABLE virtual Peer default_source_for_type(Type type);
    Q_INVOKABLE virtual QVector<Peer> known_sources_for_type(Type type);
    Q_INVOKABLE virtual QVector<Peer> known_destinations_for_type(Type type);
    Q_INVOKABLE virtual QVector<Peer> known_shares_for_type(Type type);
    Q_INVOKABLE virtual Transfer* create_import_from_peer(Peer peer);
    Q_INVOKABLE virtual Transfer* create_export_to_peer(Peer peer);
    Q_INVOKABLE virtual Transfer* create_share_to_peer(Peer peer);

    Q_INVOKABLE virtual void quit();

    Q_INVOKABLE virtual Transfer* create_import_from_peer_for_type(Peer peer, Type type);
    Q_INVOKABLE virtual Transfer* create_export_to_peer_for_type(Peer peer, Type type);
    Q_INVOKABLE virtual Transfer* create_share_to_peer_for_type(Peer peer, Type type);
    Q_INVOKABLE virtual bool has_pending(QString peer_id);
    Q_INVOKABLE virtual Peer peer_for_app_id(QString app_id);
    Q_INVOKABLE virtual void requestPeerForType(Type type, QString handler_id);
    Q_INVOKABLE virtual void selectPeerForAppId(QString app_id, QString peer_id);
    Q_INVOKABLE virtual void selectPeerForAppIdCancelled(QString app_id);
    Q_INVOKABLE virtual void requestPaste();
    Q_INVOKABLE virtual void selectPasteForAppId(QString app_id, QString surface_id, QString paste_id, bool pasteAsRichText);
    Q_INVOKABLE virtual void selectPasteForAppIdCancelled(QString app_id);
 
    ///
    // Copy & Paste

    QDBusPendingCall createPaste(const QString &surfaceId, const QMimeData& data);

    QDBusPendingCall requestLatestPaste(const QString &surfaceId);
    QDBusPendingCall requestPasteById(const QString &surfaceId, int pasteId);
    QMimeData* paste(QDBusPendingCall requestPeply);

    // synchronous versions
    bool createPasteSync(const QString &surfaceId, const QMimeData& data);
    QMimeData* latestPaste(const QString &surfaceId);
    QMimeData* pasteById(const QString &surfaceId, int id);

    QStringList pasteFormats();

  Q_SIGNALS:
    void pasteFormatsChanged();
    void pasteboardChanged();
    void peerSelected(QString);
    void peerSelectionCancelled();
    void pasteSelected(QByteArray, bool);
    void pasteSelectionCancelled();

  private Q_SLOTS:
    void onPasteFormatsChanged(const QStringList &);
    void onPeerSelected(const QString &, const QString &);
    void onPeerSelectionCancelled(const QString &);

    bool isPeerValid(Peer peer);
    void onPasteSelected(const QString &, QByteArray, bool);
    void onPasteSelectionCancelled(const QString &);
  protected:
    Hub(QObject* = nullptr);

  private:
    void requestPasteFormats();
    struct Private;
    QScopedPointer<Private> d;
    bool eventFilter(QObject *obj, QEvent *event);
};
}
}
}

#endif // COM_UBUNTU_CONTENT_HUB_H_
