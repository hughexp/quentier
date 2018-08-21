/*
 * Copyright 2016 Dmitry Ivanov
 *
 * This file is part of Quentier.
 *
 * Quentier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Quentier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quentier. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUENTIER_MANAGE_ACCOUNTS_DIALOG_H
#define QUENTIER_MANAGE_ACCOUNTS_DIALOG_H

#include <quentier/types/Account.h>
#include <quentier/types/ErrorString.h>
#include <QDialog>
#include <QNetworkProxy>

namespace Ui {
class ManageAccountsDialog;
}

namespace quentier {

QT_FORWARD_DECLARE_CLASS(AccountsModel)

class ManageAccountsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit ManageAccountsDialog(AccountsModel & accountsModel, const int currentAccountRow = -1, QWidget * parent = Q_NULLPTR);
    virtual ~ManageAccountsDialog();

Q_SIGNALS:
    void evernoteAccountAdditionRequested(QString evernoteServer, QNetworkProxy proxy);
    void localAccountAdditionRequested(QString name, QString fullName);
    void revokeAuthentication(qevercloud::UserID id);

private Q_SLOTS:
    void onAddAccountButtonPressed();
    void onRevokeAuthenticationButtonPressed();
    void onBadAccountDisplayNameEntered(ErrorString errorDescription, int row);

private:
    Ui::ManageAccountsDialog *  m_pUi;
    AccountsModel &             m_accountsModel;
};

} // namespace quentier

#endif // QUENTIER_MANAGE_ACCOUNTS_DIALOG_H
