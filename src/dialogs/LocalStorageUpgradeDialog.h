/*
 * Copyright 2018 Dmitry Ivanov
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

#ifndef QUENTIER_DIALOGS_LOCAL_STORAGE_UPGRADE_DIALOG_H
#define QUENTIER_DIALOGS_LOCAL_STORAGE_UPGRADE_DIALOG_H

#include <quentier/utility/Macros.h>
#include <quentier/types/Account.h>
#include <QDialog>
#include <QVector>
#include <QFlags>

namespace Ui {
class LocalStorageUpgradeDialog;
}

QT_FORWARD_DECLARE_CLASS(QItemSelection)

namespace quentier {

QT_FORWARD_DECLARE_CLASS(ErrorString)
QT_FORWARD_DECLARE_CLASS(AccountModel)
QT_FORWARD_DECLARE_CLASS(AccountFilterModel)
QT_FORWARD_DECLARE_CLASS(ILocalStoragePatch)

class LocalStorageUpgradeDialog : public QDialog
{
    Q_OBJECT
public:
    struct Option
    {
        enum type
        {
            AddAccount = 1 << 1,
            SwitchToAnotherAccount = 1 << 2
        };
    };
    Q_DECLARE_FLAGS(Options, Option::type)

    explicit LocalStorageUpgradeDialog(const Account & currentAccount,
                                       AccountModel & accountModel,
                                       const QVector<ILocalStoragePatch*> & patches,
                                       const Options options,
                                       QWidget * parent = Q_NULLPTR);
    virtual ~LocalStorageUpgradeDialog();

Q_SIGNALS:
    void shouldSwitchToAccount(Account account);
    void shouldCreateNewAccount();
    void shouldQuitApp();

private Q_SLOTS:
    void onSwitchToAccountPushButtonPressed();
    void onCreateNewAccountButtonPressed();
    void onQuitAppButtonPressed();
    void onApplyPatchButtonPressed();

    void onApplyPatchProgressUpdate(double progress);

    void onAccountViewSelectionChanged(const QItemSelection & selected,
                                       const QItemSelection & deselected);

private:
    void createConnections();
    void setPatchInfoLabel();
    void setErrorToStatusBar(const ErrorString & error);

private:
    void showHideDialogPartsAccordingToOptions();

private:
    virtual void reject() Q_DECL_OVERRIDE;

private:
    Ui::LocalStorageUpgradeDialog * m_pUi;
    QVector<ILocalStoragePatch*>    m_patches;
    AccountFilterModel *    m_pAccountFilterModel;
    Options     m_options;
    int         m_currentPatchIndex;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LocalStorageUpgradeDialog::Options)

} // namespace quentier

#endif // QUENTIER_DIALOGS_LOCAL_STORAGE_UPGRADE_DIALOG_H
