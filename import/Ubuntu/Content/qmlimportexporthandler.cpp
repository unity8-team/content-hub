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

#include "qmlimportexporthandler.h"
#include "../../../src/com/ubuntu/content/debug.h"

#include <com/ubuntu/content/transfer.h>

namespace cuc = com::ubuntu::content;

/*!
 * QmlImportExportHandler is for internal use only
 */

QmlImportExportHandler::QmlImportExportHandler(QObject *parent)
    : cuc::ImportExportHandler(parent)
{
    TRACE() << Q_FUNC_INFO;
}

/*!
 * \reimp
 */
void QmlImportExportHandler::handle_import(com::ubuntu::content::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO;
    Q_EMIT importRequested(transfer);
}

/*!
 * \reimp
 */
void QmlImportExportHandler::handle_export(com::ubuntu::content::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO;
    Q_EMIT exportRequested(transfer);
}

/*!
 * \reimp
 */
void QmlImportExportHandler::handle_share(com::ubuntu::content::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO;
    Q_EMIT shareRequested(transfer);
}

/*!
 * \reimp
 */
void QmlImportExportHandler::handle_copy(com::ubuntu::content::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

/*!
 * \reimp
 */
void QmlImportExportHandler::handle_paste(com::ubuntu::content::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}
