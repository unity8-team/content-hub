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

#ifndef COMMON_H
#define COMMON_H

#include <QLatin1String>
#include <QString>

const QLatin1String HUB_SERVICE_NAME = QLatin1String("com.ubuntu.content.dbus.Service");
const QLatin1String HUB_SERVICE_PATH = QLatin1String("/");
const QLatin1String HANDLER_NAME_TEMPLATE = QLatin1String("com.ubuntu.content.handler.%1");
const QLatin1String HANDLER_BASE_PATH = QLatin1String("/com/ubuntu/content/handler");
const QString CLIPBOARD_APP_ID = QLatin1String("content-hub-clipboard");
const QString PEER_PICKER_APP_ID = QLatin1String("content-hub-peer-picker");

#endif // COMMON_H
