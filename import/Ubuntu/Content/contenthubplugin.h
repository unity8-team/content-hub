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

#ifndef COM_UBUNTU_CONTENT_PLUGIN_H_
#define COM_UBUNTU_CONTENT_PLUGIN_H_

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

class ContentHub;

class ContentHubPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine * engine, const char * uri);
    void registerTypes(const char *uri);

private:
    ContentHub *m_contentHub;
};


#endif // COM_UBUNTU_CONTENT_PLUGIN_H_
