/*
 * Copyright 2016-2020 Dmitry Ivanov
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

#include "AddOrEditSavedSearchDialog.h"
#include "ui_AddOrEditSavedSearchDialog.h"

#include <lib/model/saved_search/SavedSearchModel.h>
#include <lib/libquentier/headers/quentier/local_storage/NoteSearchQuery.h>
#include <lib/libquentier/headers/quentier/logging/QuentierLogger.h>

#include <QPushButton>

namespace quentier {

AddOrEditSavedSearchDialog::AddOrEditSavedSearchDialog(
    SavedSearchModel * pSavedSearchModel, QWidget * parent,
    const QString & editedSavedSearchLocalUid) :
    QDialog(parent),
    m_pUi(new Ui::AddOrEditSavedSearchDialog),
    m_pSavedSearchModel(pSavedSearchModel), m_pSearchQuery(new NoteSearchQuery),
    m_editedSavedSearchLocalUid(editedSavedSearchLocalUid)
{
    m_pUi->setupUi(this);
    m_pUi->statusBar->setHidden(true);
    m_pUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    setupEditedSavedSearchItem();

    createConnections();

    if (!m_editedSavedSearchLocalUid.isEmpty()) {
        m_pUi->searchQueryPlainTextEdit->setFocus();
    }
    else {
        m_pUi->savedSearchNameLineEdit->setFocus();
    }
}

AddOrEditSavedSearchDialog::~AddOrEditSavedSearchDialog()
{
    delete m_pUi;
}

QString AddOrEditSavedSearchDialog::query() const
{
    return m_pUi->searchQueryPlainTextEdit->toPlainText();
}

void AddOrEditSavedSearchDialog::setQuery(const QString & query)
{
    QNDEBUG("dialog", "AddOrEditSavedSearchDialog::setQuery: " << query);
    m_pUi->searchQueryPlainTextEdit->setPlainText(query);
}

void AddOrEditSavedSearchDialog::accept()
{
    QString savedSearchName = m_pUi->savedSearchNameLineEdit->text().trimmed();
    m_stringUtils.removeNewlines(savedSearchName);

    QString savedSearchQuery = m_pSearchQuery->queryString();
    bool queryIsEmpty = m_pSearchQuery->isEmpty();

    QNDEBUG(
        "dialog",
        "AddOrEditSavedSearchDialog::accept: name = "
            << savedSearchName << ", query = " << savedSearchQuery
            << ", query is empty = " << (queryIsEmpty ? "true" : "false"));

#define REPORT_ERROR(error)                                                    \
    m_pUi->statusBar->setText(tr(error));                                      \
    QNWARNING("dialog", error);                                                \
    m_pUi->statusBar->setHidden(false)

    if (Q_UNLIKELY(m_pSavedSearchModel.isNull())) {
        REPORT_ERROR(
            QT_TR_NOOP("Can't accept new saved search or edit "
                       "the existing one: saved search model is gone"));
        return;
    }

    if (m_editedSavedSearchLocalUid.isEmpty()) {
        QNDEBUG(
            "dialog",
            "Edited saved search local uid is empty, adding "
                << "new saved search to the model");

        ErrorString errorDescription;

        auto index = m_pSavedSearchModel->createSavedSearch(
            savedSearchName, savedSearchQuery, errorDescription);

        if (!index.isValid()) {
            m_pUi->statusBar->setText(errorDescription.localizedString());
            QNWARNING("dialog", errorDescription);
            m_pUi->statusBar->setHidden(false);
            return;
        }
    }
    else {
        QNDEBUG(
            "dialog",
            "Edited saved search local uid is not empty, "
                << "editing the existing saved search within the model");

        QModelIndex index =
            m_pSavedSearchModel->indexForLocalUid(m_editedSavedSearchLocalUid);

        const auto * pItem = m_pSavedSearchModel->itemForIndex(index);
        if (Q_UNLIKELY(!pItem)) {
            REPORT_ERROR(
                QT_TR_NOOP("Can't edit saved search: saved search was "
                           "not found in the model"));
            return;
        }

        const auto * pSavedSearchItem = pItem->cast<SavedSearchItem>();
        if (Q_UNLIKELY(!pSavedSearchItem)) {
            REPORT_ERROR(
                QT_TR_NOOP("Can't edit saved search: can't cast model item to "
                           "saved search one"));
            return;
        }

        auto queryIndex = m_pSavedSearchModel->index(
            index.row(), static_cast<int>(SavedSearchModel::Column::Query),
            index.parent());

        if (pSavedSearchItem->query() != savedSearchQuery) {
            bool res =
                m_pSavedSearchModel->setData(queryIndex, savedSearchQuery);
            if (Q_UNLIKELY(!res)) {
                REPORT_ERROR(
                    QT_TR_NOOP("Failed to set the saved search query "
                               "to the model"));
            }
        }

        // If needed, update the saved search name
        auto nameIndex = m_pSavedSearchModel->index(
            index.row(), static_cast<int>(SavedSearchModel::Column::Name),
            index.parent());

        if (pSavedSearchItem->nameUpper() != savedSearchName.toUpper()) {
            bool res = m_pSavedSearchModel->setData(nameIndex, savedSearchName);
            if (Q_UNLIKELY(!res)) {
                // Probably the new name collides with some existing
                // saved search's name
                auto existingItemIndex =
                    m_pSavedSearchModel->indexForSavedSearchName(
                        savedSearchName);

                if (existingItemIndex.isValid() &&
                    ((existingItemIndex.row() != nameIndex.row()) ||
                     (existingItemIndex.parent() != nameIndex.parent())))
                {
                    // The new name collides with some existing saved search and
                    // now with the currently edited one
                    REPORT_ERROR(
                        QT_TR_NOOP("The saved search name must be "
                                   "unique in case insensitive manner"));
                }
                else {
                    // Don't really know what happened...
                    REPORT_ERROR(
                        QT_TR_NOOP("Can't set this name for the saved "
                                   "search"));
                }
            }
        }
    }

    QDialog::accept();
}

void AddOrEditSavedSearchDialog::onSavedSearchNameEdited(
    const QString & savedSearchName)
{
    QNDEBUG(
        "dialog",
        "AddOrEditSavedSearchDialog::onSavedSearchNameEdited: "
            << savedSearchName);

    if (Q_UNLIKELY(m_pSavedSearchModel.isNull())) {
        QNTRACE("dialog", "No saved search model");
        return;
    }

    QModelIndex index =
        m_pSavedSearchModel->indexForSavedSearchName(savedSearchName);
    if (index.isValid()) {
        m_pUi->statusBar->setText(
            tr("The saved search name must be unique in "
               "case insensitive manner"));

        m_pUi->statusBar->setHidden(false);
        m_pUi->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    }
    else {
        m_pUi->statusBar->clear();
        m_pUi->statusBar->setHidden(true);
        m_pUi->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
    }
}

void AddOrEditSavedSearchDialog::onSearchQueryEdited()
{
    QString searchQuery = m_pUi->searchQueryPlainTextEdit->toPlainText();

    QNDEBUG(
        "dialog",
        "AddOrEditSavedSearchDialog::onSearchQueryEdited: " << searchQuery);

    ErrorString parseError;
    bool res = m_pSearchQuery->setQueryString(searchQuery, parseError);
    if (!res) {
        ErrorString error(QT_TR_NOOP("Search query string is invalid"));
        error.appendBase(parseError.base());
        error.appendBase(parseError.additionalBases());
        error.details() = parseError.details();
        QNDEBUG("dialog", error);

        // NOTE: only show the parsing error to user if the query was good
        // before the last edit
        if (m_pUi->buttonBox->button(QDialogButtonBox::Ok)->isEnabled()) {
            m_pUi->statusBar->setText(error.localizedString());
            m_pUi->statusBar->setHidden(false);
            m_pUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        }

        m_pSearchQuery->clear();
        return;
    }

    m_pUi->statusBar->clear();
    m_pUi->statusBar->setHidden(true);
    m_pUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

    QNTRACE(
        "dialog",
        "Successfully parsed the note search query: " << *m_pSearchQuery);
}

void AddOrEditSavedSearchDialog::createConnections()
{
    QObject::connect(
        m_pUi->savedSearchNameLineEdit, &QLineEdit::textEdited, this,
        &AddOrEditSavedSearchDialog::onSavedSearchNameEdited);

    QObject::connect(
        m_pUi->searchQueryPlainTextEdit, &QPlainTextEdit::textChanged, this,
        &AddOrEditSavedSearchDialog::onSearchQueryEdited);
}

void AddOrEditSavedSearchDialog::setupEditedSavedSearchItem()
{
    QNDEBUG("dialog", "AddOrEditSavedSearchDialog::setupEditedSavedSearchItem");

    if (m_editedSavedSearchLocalUid.isEmpty()) {
        QNDEBUG("dialog", "Edited saved search's local uid is empty");
        return;
    }

    if (Q_UNLIKELY(m_pSavedSearchModel.isNull())) {
        QNDEBUG("dialog", "Saved search model is null");
        return;
    }

    auto editedSavedSearchIndex =
        m_pSavedSearchModel->indexForLocalUid(m_editedSavedSearchLocalUid);

    const auto * pItem =
        m_pSavedSearchModel->itemForIndex(editedSavedSearchIndex);

    const SavedSearchItem * pSavedSearchItem =
        (pItem ? pItem->cast<SavedSearchItem>() : nullptr);

    if (Q_UNLIKELY(!pSavedSearchItem)) {
        m_pUi->statusBar->setText(
            tr("Can't find the edited saved search within the model"));
        m_pUi->statusBar->setHidden(false);
        return;
    }

    m_pUi->savedSearchNameLineEdit->setText(pSavedSearchItem->name());
    m_pUi->searchQueryPlainTextEdit->setPlainText(pSavedSearchItem->query());
}

} // namespace quentier
