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

#include "clipboardapplication.h"

#include <QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include "config.h"
#include "paste-data-model.h"
#include "paste-data-filter-model.h"

ClipboardApplication::ClipboardApplication(int &argc, char **argv)
    : QGuiApplication(argc, argv),
    m_view(0)
{
    connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)), SIGNAL(applicationActiveChanged()));
}

ClipboardApplication::~ClipboardApplication()
{
}

bool ClipboardApplication::setup()
{
    const char* uri = "clipboardapp.private";
    qmlRegisterType<PasteDataModel>(uri, 0, 1, "PasteDataModel");
    qmlRegisterType<PasteDataFilterModel>(uri, 0, 1, "PasteDataFilterModel");

    m_view = new QQuickView();
    QObject::connect(m_view->engine(), SIGNAL(quit()), SLOT(quit()));
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->rootContext()->setContextProperty("application", this);
    m_view->engine()->setBaseUrl(QUrl::fromLocalFile(clipboardAppDirectory()));
    m_view->engine()->addImportPath(clipboardAppImportDirectory());
    m_view->setSource(QUrl::fromLocalFile(sourceQml()));
    m_view->show();

    return true;
}

bool ClipboardApplication::applicationActive()
{
    return QGuiApplication::applicationState() == Qt::ApplicationActive;
}
