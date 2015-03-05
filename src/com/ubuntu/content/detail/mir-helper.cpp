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

#include "debug.h"
#include "mir-helper.h"

#include <mir_toolkit/mir_client_library.h>
#include <mir_toolkit/mir_prompt_session.h>

#include <QHash>
#include <QStandardPaths>
#include <QWeakPointer>

static MirHelper *m_instance = 0;

class PromptSessionPrivate
{
public:
    inline PromptSessionPrivate(MirPromptSession *session, pid_t initiatorPid);
    inline ~PromptSessionPrivate();

    void emitFinished() { Q_EMIT q_ptr->finished(); }

    MirPromptSession *m_mirSession;
    pid_t m_initiatorPid;
    QList<int> m_fds;
    mutable PromptSession *q_ptr;
};

class MirHelperPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(MirHelper)

public:
    inline MirHelperPrivate(MirHelper *helper);
    inline ~MirHelperPrivate();

    PromptSession *createPromptSession(pid_t initiatorPid);
    void onSessionStopped(MirPromptSession *mirSession);

private:
    friend class PromptSession;
    MirConnection *m_connection;
    QHash<pid_t,QWeakPointer<PromptSession> > m_sessions;
    mutable MirHelper *q_ptr;
};

PromptSessionPrivate::PromptSessionPrivate(MirPromptSession *session,
                                           pid_t initiatorPid):
    m_mirSession(session),
    m_initiatorPid(initiatorPid)
{
    TRACE() << Q_FUNC_INFO;
}

PromptSessionPrivate::~PromptSessionPrivate()
{
    mir_prompt_session_release_sync(m_mirSession);
    m_mirSession = 0;
}

PromptSession::PromptSession(PromptSessionPrivate *priv):
    d_ptr(priv)
{
    TRACE() << Q_FUNC_INFO;
    priv->q_ptr = this;
}

PromptSession::~PromptSession()
{
    TRACE() << Q_FUNC_INFO;
    Q_D(PromptSession);
    MirHelperPrivate *helperPrivate = MirHelper::instance()->d_ptr;
    helperPrivate->m_sessions.remove(d->m_initiatorPid);
    delete d_ptr;
}

static void client_fd_callback(MirPromptSession *, size_t count,
                               int const *fds, void *context)
{
    TRACE() << Q_FUNC_INFO;
    PromptSessionPrivate *priv = (PromptSessionPrivate *)context;
    for (size_t i = 0; i < count; i++) {
        priv->m_fds.append(fds[i]);
    }
}

QString PromptSession::requestSocket()
{
    TRACE() << Q_FUNC_INFO;
    Q_D(PromptSession);

    d->m_fds.clear();
    mir_wait_for(mir_prompt_session_new_fds_for_prompt_providers(
        d->m_mirSession, 1, client_fd_callback, d));
    if (!d->m_fds.isEmpty()) {
        return QString("%1").arg(d->m_fds[0]);
    } else {
        return QString();
    }
}

MirHelperPrivate::MirHelperPrivate(MirHelper *helper):
    QObject(helper),
    q_ptr(helper)
{
    TRACE() << Q_FUNC_INFO;
    QString mirSocket =
        QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) +
        "/mir_socket_trusted";
    m_connection = mir_connect_sync(mirSocket.toUtf8().constData(),
                                    "content-hub-service");
    if (Q_UNLIKELY(!mir_connection_is_valid(m_connection))) {
        qWarning() << "Invalid Mir connection:" <<
            mir_connection_get_error_message(m_connection);
        return;
    }
}

MirHelperPrivate::~MirHelperPrivate()
{
    TRACE() << Q_FUNC_INFO;
    if (m_connection) {
        mir_connection_release(m_connection);
        m_connection = 0;
    }
}

static void session_event_callback(MirPromptSession *mirSession,
                                   MirPromptSessionState state,
                                   void *self)
{
    TRACE() << Q_FUNC_INFO;
    MirHelperPrivate *helper = reinterpret_cast<MirHelperPrivate*>(self);
    TRACE() << "Prompt Session state updated to" << state;
    if (state == mir_prompt_session_state_stopped) {
        helper->onSessionStopped(mirSession);
    }
}

void MirHelperPrivate::onSessionStopped(MirPromptSession *mirSession)
{
    TRACE() << Q_FUNC_INFO;
    Q_FOREACH(PromptSessionP session, m_sessions) {
        if (mirSession == session->d_ptr->m_mirSession) {
            session->d_ptr->emitFinished();
        }
    }
}

PromptSession *MirHelperPrivate::createPromptSession(pid_t initiatorPid)
{
    TRACE() << Q_FUNC_INFO << initiatorPid;
    if (Q_UNLIKELY(!m_connection)) return 0;
    if (Q_UNLIKELY(!mir_connection_is_valid(m_connection))) return 0; 

    MirPromptSession *mirSession =
        mir_connection_create_prompt_session_sync(m_connection,
                                                  initiatorPid,
                                                  session_event_callback,
                                                  this);
    if (!mirSession) return 0;

    if (Q_UNLIKELY(!mir_prompt_session_is_valid(mirSession))) {
        qWarning() << "Invalid prompt session:" <<
            mir_prompt_session_error_message(mirSession);
        return 0;
    }

    return new PromptSession(new PromptSessionPrivate(mirSession, initiatorPid));
}

MirHelper::MirHelper(QObject *parent):
    QObject(parent),
    d_ptr(new MirHelperPrivate(this))
{
    TRACE() << Q_FUNC_INFO;
}

MirHelper::~MirHelper()
{
    TRACE() << Q_FUNC_INFO;
    m_instance = 0;
}

MirHelper *MirHelper::instance()
{
    TRACE() << Q_FUNC_INFO;
    if (!m_instance) {
        m_instance = new MirHelper;
    }
    return m_instance;
}

PromptSessionP MirHelper::createPromptSession(pid_t initiatorPid)
{
    Q_D(MirHelper);
    TRACE() << Q_FUNC_INFO << initiatorPid;
    PromptSessionP session = d->m_sessions.value(initiatorPid);
    if (session.isNull()) {
        TRACE() << Q_FUNC_INFO << "Session is NULL";
        PromptSession *s = d->createPromptSession(initiatorPid);
        if (s) {
            TRACE() << Q_FUNC_INFO << "Creating new session";
            session = PromptSessionP(s);
            d->m_sessions.insert(initiatorPid, session);
        }
    }
    return session;
}

#include "mir-helper.moc"
