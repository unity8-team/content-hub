/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef MIR_HELPER_H
#define MIR_HELPER_H

#include <QObject>
#include <QSharedPointer>

class PromptSessionPrivate;
class MirHelperPrivate;

class PromptSession: public QObject
{
    Q_OBJECT

public:
    ~PromptSession();

    QString requestSocket();

Q_SIGNALS:
    void finished();

private:
    explicit PromptSession(PromptSessionPrivate *priv);

private:
    friend class MirHelperPrivate;
    PromptSessionPrivate *d_ptr;
    Q_DECLARE_PRIVATE(PromptSession)
};

typedef QSharedPointer<PromptSession> PromptSessionP;

class MirHelper: public QObject
{
    Q_OBJECT

public:
    static MirHelper *instance();

    PromptSessionP createPromptSession(pid_t initiatorPid);

private:
    explicit MirHelper(QObject *parent = 0);
    ~MirHelper();

private:
    friend class PromptSession;
    MirHelperPrivate *d_ptr;
    Q_DECLARE_PRIVATE(MirHelper)
};

#endif // MIR_HELPER_H
