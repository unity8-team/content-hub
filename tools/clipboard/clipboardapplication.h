/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Arthur Mello <arthur.mello@canonical.com>
 */

#ifndef CLIPBOARDAPPLICATION_H
#define CLIPBOARDAPPLICATION_H

#include <QGuiApplication>
#include <QQuickView>

class ClipboardApplication : public QGuiApplication
{
    Q_OBJECT
    Q_PROPERTY(bool applicationActive READ applicationActive NOTIFY applicationActiveChanged)

public:
    ClipboardApplication(int &argc, char **argv);
    virtual ~ClipboardApplication();
    bool setup();
    bool applicationActive();

Q_SIGNALS:
    void applicationActiveChanged();

protected Q_SLOTS:
    void onApplicationStateChanged(Qt::ApplicationState state);

private:
    QQuickView *m_view;
};

#endif // CLIPBOARDAPPLICATION_H
