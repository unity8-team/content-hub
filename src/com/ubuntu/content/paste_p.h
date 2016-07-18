/*
 * Copyright © 2016 Canonical Ltd.
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
#ifndef COM_UBUNTU_CONTENT_PASTE_P_H_
#define COM_UBUNTU_CONTENT_PASTE_P_H_

#include "common.h"
#include "ContentPasteInterface.h"

#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/paste.h>

#include <QByteArray>
#include <QMimeData>
#include <QObject>

namespace {
    /* Used for pasteboard */
    const int maxFormatsCount = 16;
    const int maxBufferSize = 4 * 1024 * 1024;  // 4 Mb
}

namespace com
{
namespace ubuntu
{
namespace content
{
class Paste::Private : public QObject
{
    Q_OBJECT
  public:
    static Paste* make_paste(const QDBusObjectPath& paste, QObject* parent)
    {
        QSharedPointer<Private> d{new Private{paste, parent}};
        return new Paste{d, parent};
    }

    Private(const QDBusObjectPath& paste, QObject* parent)
            : QObject(parent),
              remote_paste(
                  new com::ubuntu::content::dbus::Paste(
                      HUB_SERVICE_NAME,
                      paste.path(), 
                      QDBusConnection::sessionBus(), this))
    {
    }

    int id()
    {
        auto reply = remote_paste->Id();
        reply.waitForFinished();

        return reply.value();
    }

    State state()
    {
        auto reply = remote_paste->State();
        reply.waitForFinished();

        return static_cast<Paste::State>(reply.value());
    }

    bool charge(const QMimeData& mimeData)
    {
        QMimeData *data = new QMimeData();

        Q_FOREACH(QString t, mimeData.formats()) {
            data->setData(t, mimeData.data(t));
        }

        auto serializedMimeData = serializeMimeData(data);
        if (serializedMimeData.isEmpty())
            return false;
        QVariant v(serializedMimeData);

        QVariantList d;
        d << QVariant::fromValue(v);

        auto reply = remote_paste->Charge(d);
        reply.waitForFinished();

        return not reply.isError();
    }

    QMimeData* mimeData()
    {
        auto reply = remote_paste->MimeData();
        reply.waitForFinished();

        if (reply.isError())
            return nullptr;

        QByteArray serializedMimeData = qdbus_cast<QByteArray>(reply.value().first());

        QMimeData *mimeData = deserializeMimeData(serializedMimeData);
        if (mimeData == nullptr) 
            qWarning() << "Got invalid serialized mime data. Ignoring it.";

        return mimeData;
    }

    QString source()
    {
        auto reply = remote_paste->source();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    QByteArray serializeMimeData(QMimeData *mimeData) const
    {
        Q_ASSERT(mimeData != nullptr);

        const QStringList formats = mimeData->formats();
        const int formatCount = qMin(formats.size(), maxFormatsCount);
        const int headerSize = sizeof(int) + (formatCount * 4 * sizeof(int));
        int bufferSize = headerSize;

        for (int i = 0; i < formatCount; i++) 
            bufferSize += formats[i].size() + mimeData->data(formats[i]).size();

        QByteArray serializedMimeData;
        if (bufferSize <= maxBufferSize) {
            // Serialize data.
            serializedMimeData.resize(bufferSize);
            {
                char *buffer = serializedMimeData.data();
                int* header = reinterpret_cast<int*>(serializedMimeData.data());
                int offset = headerSize;
                header[0] = formatCount;
                for (int i = 0; i < formatCount; i++) {
                    const QByteArray data = mimeData->data(formats[i]);
                    const int formatOffset = offset;
                    const int formatSize = formats[i].size();
                    const int dataOffset = offset + formatSize;
                    const int dataSize = data.size();
                    memcpy(&buffer[formatOffset], formats[i].toLatin1().data(), formatSize);
                    memcpy(&buffer[dataOffset], data.data(), dataSize);
                    header[i*4+1] = formatOffset;
                    header[i*4+2] = formatSize;
                    header[i*4+3] = dataOffset;
                    header[i*4+4] = dataSize;
                    offset += formatSize + dataSize;
                }
            }
        } else {
            qWarning("Not sending contents (%d bytes) to the global clipboard as it's"
                    " bigger than the maximum allowed size of %d bytes", bufferSize, maxBufferSize);
        }

        return serializedMimeData;
    }

    QMimeData *deserializeMimeData(const QByteArray &serializedMimeData) const
    {
        if (static_cast<std::size_t>(serializedMimeData.size()) < sizeof(int)) {
            // Data is invalid
            return nullptr;
        }

        QMimeData *mimeData = new QMimeData;
   
        const char* const buffer = serializedMimeData.constData();
        const int* const header = reinterpret_cast<const int*>(serializedMimeData.constData());

        const int count = qMin(header[0], maxFormatsCount);

        for (int i = 0; i < count; i++) {
            const int formatOffset = header[i*4+1];
            const int formatSize = header[i*4+2];
            const int dataOffset = header[i*4+3];
            const int dataSize = header[i*4+4];

            if (formatOffset + formatSize <= serializedMimeData.size()
                && dataOffset + dataSize <= serializedMimeData.size()) {

                QString mimeType = QString::fromLatin1(&buffer[formatOffset], formatSize);
                QByteArray mimeDataBytes(&buffer[dataOffset], dataSize);

                mimeData->setData(mimeType, mimeDataBytes);
            }
        }

        return mimeData;
    }

    com::ubuntu::content::dbus::Paste* remote_paste;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_PASTE_P_H_
