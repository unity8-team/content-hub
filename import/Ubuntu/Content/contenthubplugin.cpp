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
#include "../../../src/com/ubuntu/content/debug.h"

#include "contenthandler.h"
#include "contenthub.h"
#include "contenticonprovider.h"
#include "contentitem.h"
#include "contentpeer.h"
#include "contentpeermodel.h"
#include "contentscope.h"
#include "contentstore.h"
#include "contenttransfer.h"
#include "contenttype.h"

#include <qqml.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <QIcon>

/*!
 * \brief qml_content_hub function to unstatinate the ContentHub as a singleton in QML
 */
static QObject *qml_content_hub(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    TRACE() << Q_FUNC_INFO;
    return ContentHub::instance();
}

/*!
 * \reimp
 */
void ContentHubPlugin::initializeEngine(QQmlEngine * engine, const char * uri)
{
    Q_UNUSED(uri)
    TRACE() << Q_FUNC_INFO;
    QIcon::setThemeName("suru");
    QIcon::setThemeSearchPaths(QStringList() << ("/usr/share/icons/"));
    ContentIconProvider *iconProvider = ContentIconProvider::instance();
    engine->addImageProvider("content-hub", iconProvider);
}

/*!
 * \reimp
 */
void ContentHubPlugin::registerTypes(const char *uri)
{
    TRACE() << Q_FUNC_INFO;
    Q_ASSERT(uri == QLatin1String("Ubuntu.Content"));

    const int versionMajor = 0;
    const int versionMinor = 1;

    qmlRegisterUncreatableType<ContentHandler>(uri, versionMajor, versionMinor, "ContentHandler", "Not creatable as an object, use only to retrieve handler enums (e.g. ContentHandler.Source)");
    qmlRegisterSingletonType<ContentHub>(uri, versionMajor, versionMinor, "ContentHub", qml_content_hub);
    qmlRegisterType<ContentItem>(uri, versionMajor, versionMinor, "ContentItem");
    qmlRegisterType<ContentPeer>(uri, versionMajor, versionMinor, "ContentPeer");
    qmlRegisterType<ContentPeerModel>(uri, versionMajor, versionMinor, "ContentPeerModel");
    qmlRegisterType<ContentScope>(uri, versionMajor, versionMinor, "ContentScope");
    qmlRegisterType<ContentStore>(uri, versionMajor, versionMinor, "ContentStore");
    qmlRegisterUncreatableType<ContentTransfer>(uri, versionMajor, versionMinor, "ContentTransfer", "created by hub");
    qmlRegisterUncreatableType<ContentType>(uri, versionMajor, versionMinor, "ContentType", "Use only the type");
}
