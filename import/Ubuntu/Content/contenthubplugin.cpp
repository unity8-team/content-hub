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

#include "contenthubplugin.h"

#include "contenthub.h"
#include "contentitem.h"
#include "contentpeer.h"
#include "contentstore.h"
#include "contenttransfer.h"
#include "contenttype.h"

#include <qqml.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

/*!
 * \brief qml_content_hub function to unstatinate the ContentHub as a singleton in QML
 * \param engine
 * \param scriptEngine
 * \return
 */
static QObject *qml_content_hub(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    qDebug() << Q_FUNC_INFO;
    return new ContentHub();
}

/*!
 * \reimp
 */
void ContentHubPlugin::registerTypes(const char *uri)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(uri == QLatin1String("Ubuntu.Content"));

    const int versionMajor = 0;
    const int versionMinor = 1;

    qmlRegisterSingletonType<ContentHub>(uri, versionMajor, versionMinor, "ContentHub", qml_content_hub);
    qmlRegisterType<ContentItem>(uri, versionMajor, versionMinor, "ContentItem");
    qmlRegisterUncreatableType<ContentStore>(uri, versionMajor, versionMinor, "ContentStore", "created by hub");
    qmlRegisterUncreatableType<ContentPeer>(uri, versionMajor, versionMinor, "ContentPeer", "created by hub");
    qmlRegisterUncreatableType<ContentTransfer>(uri, versionMajor, versionMinor, "ContentTransfer", "created by hub");
    qmlRegisterUncreatableType<ContentType>(uri, versionMajor, versionMinor, "ContentType", "Use only the type");
}
