#ifndef __QUTE_NOTE__CORE__CLIENT__SYNCHRONIZATION__SYNCHRONIZATION_MANAGER_PRIVATE_H
#define __QUTE_NOTE__CORE__CLIENT__SYNCHRONIZATION__SYNCHRONIZATION_MANAGER_PRIVATE_H

#include <tools/qt4helper.h>
#include <QEverCloud.h>
#include <oauth.h>
#include <QObject>

namespace qute_note {

QT_FORWARD_DECLARE_CLASS(LocalStorageManagerThread)

class SynchronizationManagerPrivate: public QObject
{
    Q_OBJECT
public:
    SynchronizationManagerPrivate(LocalStorageManagerThread & localStorageManagerThread);
    virtual ~SynchronizationManagerPrivate();

    void synchronize();

Q_SIGNALS:
    void notifyError(QString errorDescription);

private Q_SLOTS:
    void onOAuthSuccess();
    void onOAuthFailure();
    void onOAuthResult(bool result);

private:
    SynchronizationManagerPrivate() Q_DECL_DELETE;
    SynchronizationManagerPrivate(const SynchronizationManagerPrivate & other) Q_DECL_DELETE;
    SynchronizationManagerPrivate & operator=(const SynchronizationManagerPrivate & other) Q_DECL_DELETE;

    void connect(LocalStorageManagerThread & localStorageManagerThread);
    void authenticate();

    void launchOAuth();

private:
    QScopedPointer<qevercloud::SyncState>   m_pLastSyncState;
    QScopedPointer<qevercloud::EvernoteOAuthWebView>    m_pOAuthWebView;
    QScopedPointer<qevercloud::EvernoteOAuthWebView::OAuthResult>   m_pOAuthResult;
};

} // namespace qute_note

#endif // __QUTE_NOTE__CORE__CLIENT__SYNCHRONIZATION__SYNCHRONIZATION_MANAGER_PRIVATE_H
