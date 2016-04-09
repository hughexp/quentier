#include "NotebookModelTestHelper.h"
#include "../../models/NotebookModel.h"
#include "modeltest.h"
#include <qute_note/utility/SysInfo.h>
#include <qute_note/logging/QuteNoteLogger.h>
#include <qute_note/utility/UidGenerator.h>

#define FAIL(text) \
    QNWARNING(text); \
    emit failure(); \
    return

namespace qute_note {

NotebookModelTestHelper::NotebookModelTestHelper(LocalStorageManagerThreadWorker * pLocalStorageManagerThreadWorker,
                                                 QObject * parent) :
    QObject(parent),
    m_pLocalStorageManagerThreadWorker(pLocalStorageManagerThreadWorker)
{
    QObject::connect(pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,addNotebookFailed,Notebook,QString,QUuid),
                     this, QNSLOT(NotebookModelTestHelper,onAddNotebookFailed,Notebook,QString,QUuid));
    QObject::connect(pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,updateNotebookFailed,Notebook,QString,QUuid),
                     this, QNSLOT(NotebookModelTestHelper,onUpdateNotebookFailed,Notebook,QString,QUuid));
    QObject::connect(pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,findNotebookFailed,Notebook,QString,QUuid),
                     this, QNSLOT(NotebookModelTestHelper,onFindNotebookFailed,Notebook,QString,QUuid));
    QObject::connect(pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,listNotebooksFailed,
                                                                LocalStorageManager::ListObjectsOptions,size_t,size_t,
                                                                LocalStorageManager::ListNotebooksOrder::type,
                                                                LocalStorageManager::OrderDirection::type,
                                                                QString,QString,QUuid),
                     this, QNSLOT(NotebookModelTestHelper,onListNotebooksFailed,LocalStorageManager::ListObjectsOptions,
                                  size_t,size_t,LocalStorageManager::ListNotebooksOrder::type,
                                  LocalStorageManager::OrderDirection::type,QString,QString,QUuid));
    QObject::connect(pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,expungeNotebookFailed,Notebook,QString,QUuid),
                     this, QNSLOT(NotebookModelTestHelper,onExpungeNotebookFailed,Notebook,QString,QUuid));
}

void NotebookModelTestHelper::test()
{
    QNDEBUG("NotebookModelTestHelper::test");

    try {
        Notebook first;
        first.setName("First");
        first.setLocal(true);
        first.setDirty(true);

        Notebook second;
        second.setName("Second");
        second.setLocal(true);
        second.setDirty(false);
        second.setStack("Stack 1");

        Notebook third;
        third.setName("Third");
        third.setLocal(false);
        third.setGuid(UidGenerator::Generate());
        third.setDirty(true);
        third.setStack("Stack 1");

        Notebook fourth;
        fourth.setName("Fourth");
        fourth.setLocal(false);
        fourth.setGuid(UidGenerator::Generate());
        fourth.setDirty(false);
        fourth.setPublished(true);
        fourth.setStack("Stack 1");

        Notebook fifth;
        fifth.setName("Fifth");
        fifth.setLocal(false);
        fifth.setGuid(UidGenerator::Generate());
        fifth.setLinkedNotebookGuid(fifth.guid());
        fifth.setDirty(false);
        fifth.setStack("Stack 1");

        Notebook sixth;
        sixth.setName("Sixth");
        sixth.setLocal(false);
        sixth.setGuid(UidGenerator::Generate());
        sixth.setDirty(false);

        Notebook seventh;
        seventh.setName("Seventh");
        seventh.setLocal(false);
        seventh.setGuid(UidGenerator::Generate());
        seventh.setDirty(false);
        seventh.setPublished(true);

        Notebook eighth;
        eighth.setName("Eighth");
        eighth.setLocal(false);
        eighth.setGuid(UidGenerator::Generate());
        eighth.setLinkedNotebookGuid(eighth.guid());
        eighth.setDirty(false);

        Notebook nineth;
        nineth.setName("Nineth");
        nineth.setLocal(true);
        nineth.setDirty(false);
        nineth.setStack("Stack 2");

        Notebook tenth;
        tenth.setName("Tenth");
        tenth.setLocal(false);
        tenth.setGuid(UidGenerator::Generate());
        tenth.setDirty(false);
        tenth.setStack("Stack 2");

#define ADD_NOTEBOOK(notebook) \
        m_pLocalStorageManagerThreadWorker->onAddNotebookRequest(notebook, QUuid())

        ADD_NOTEBOOK(first);
        ADD_NOTEBOOK(second);
        ADD_NOTEBOOK(third);
        ADD_NOTEBOOK(fourth);
        ADD_NOTEBOOK(fifth);
        ADD_NOTEBOOK(sixth);
        ADD_NOTEBOOK(seventh);
        ADD_NOTEBOOK(eighth);
        ADD_NOTEBOOK(nineth);
        ADD_NOTEBOOK(tenth);

#undef ADD_NOTEBOOK

        NotebookModel * model = new NotebookModel(*m_pLocalStorageManagerThreadWorker, this);
        ModelTest t1(model);
        Q_UNUSED(t1)

        // Should not be able to change the dirty flag manually
        QModelIndex secondIndex = model->indexForLocalUid(second.localUid());
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for local uid");
        }

        QModelIndex secondParentIndex = model->parent(secondIndex);
        secondIndex = model->index(secondIndex.row(), NotebookModel::Columns::Dirty, secondParentIndex);
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for dirty column");
        }

        bool res = model->setData(secondIndex, QVariant(true), Qt::EditRole);
        if (res) {
            FAIL("Was able to change the dirty flag in the notebook model manually which is not intended");
        }

        QVariant data = model->data(secondIndex, Qt::EditRole);
        if (data.isNull()) {
            FAIL("Null data was returned by the notebook model while expected to get the state of dirty flag");
        }

        if (data.toBool()) {
            FAIL("The dirty state appears to have changed after setData in notebook model even though the method returned false");
        }

        // Should be able to make the non-synchronizable (local) item synchronizable (non-local)
        secondIndex = model->index(secondIndex.row(), NotebookModel::Columns::Synchronizable, secondParentIndex);
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for synchronizable column");
        }

        res = model->setData(secondIndex, QVariant(true), Qt::EditRole);
        if (!res) {
            FAIL("Can't change the synchronizable flag from false to true for notebook model item");
        }

        data = model->data(secondIndex, Qt::EditRole);
        if (data.isNull()) {
            FAIL("Null data was returned by the notebook model while expected to get the state of synchronizable flag");
        }

        if (!data.toBool()) {
            FAIL("The synchronizable state appears to have not changed after setData in notebook model even though the method returned true");
        }

        // Verify the dirty flag has changed as a result of making the item synchronizable
        secondIndex = model->index(secondIndex.row(), NotebookModel::Columns::Dirty, secondParentIndex);
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for dirty column");
        }

        data = model->data(secondIndex, Qt::EditRole);
        if (data.isNull()) {
            FAIL("Null data was returned by the notebook model while expected to get the state of dirty flag");
        }

        if (!data.toBool()) {
            FAIL("The dirty state hasn't changed after making the notebook model item synchronizable while it was expected to have changed");
        }

        // Should not be able to make the synchronizable (non-local) item non-synchronizable (local)
        secondIndex = model->index(secondIndex.row(), NotebookModel::Columns::Synchronizable, secondParentIndex);
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook item model index for synchronizable column");
        }

        res = model->setData(secondIndex, QVariant(false), Qt::EditRole);
        if (res) {
            FAIL("Was able to change the synchronizable flag in notebook model from true to false which is not intended");
        }

        data = model->data(secondIndex, Qt::EditRole);
        if (data.isNull()) {
            FAIL("Null data was returned by the notebook model while expected to get the state of synchronizable flag");
        }

        if (!data.toBool()) {
            FAIL("The synchronizable state appears to have changed after setData in notebook model even though the method returned false");
        }

        // Should be able to change name
        secondIndex = model->index(secondIndex.row(), NotebookModel::Columns::Name, secondParentIndex);
        if (!secondIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for name column");
        }

        QString newName = "Second (name modified)";
        res = model->setData(secondIndex, QVariant(newName), Qt::EditRole);
        if (!res) {
            FAIL("Can't change the name of the notebook model item");
        }

        data = model->data(secondIndex, Qt::EditRole);
        if (data.isNull()) {
            FAIL("Null data was returned by the notebook model while expected to get the name of the tag item");
        }

        if (data.toString() != newName) {
            FAIL("The name of the notebook item returned by the model does not match the name just set to this item: "
                      "received " << data.toString() << ", expected " << newName);
        }

        // Should not be able to remove the row with a synchronizable (non-local) notebook
        res = model->removeRow(secondIndex.row(), secondParentIndex);
        if (res) {
            FAIL("Was able to remove the row with a synchronizable notebook which is not intended");
        }

        QModelIndex secondIndexAfterFailedRemoval = model->indexForLocalUid(second.localUid());
        if (!secondIndexAfterFailedRemoval.isValid()) {
            FAIL("Can't get the valid notebook model item index after the failed row removal attempt");
        }

        if (secondIndexAfterFailedRemoval.row() != secondIndex.row()) {
            FAIL("Notebook model returned item index with a different row after the failed row removal attempt");
        }

        // Should be able to remove the row with a non-synchronizable (local) notebook
        QModelIndex firstIndex = model->indexForLocalUid(first.localUid());
        if (!firstIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for local uid");
        }

        QModelIndex firstParentIndex = model->parent(firstIndex);
        res = model->removeRow(firstIndex.row(), firstParentIndex);
        if (!res) {
            FAIL("Can't remove the row with a non-synchronizable notebook item from the model");
        }

        QModelIndex firstIndexAfterRemoval = model->indexForLocalUid(first.localUid());
        if (firstIndexAfterRemoval.isValid()) {
            FAIL("Was able to get the valid model index for the removed notebook item by local uid which is not intended");
        }

        // Should be able to move the non-stacked item to the existing stack
        QModelIndex sixthIndex = model->indexForLocalUid(sixth.localUid());
        if (!sixthIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index for local uid");
        }

        QModelIndex sixthIndexMoved = model->moveToStack(sixthIndex, fifth.stack());
        if (!sixthIndexMoved.isValid()) {
            FAIL("Can't get the valid notebook model item index from the method intended to move the item to the stack");
        }

        const NotebookModelItem * sixthItem = model->itemForIndex(sixthIndexMoved);
        if (!sixthItem) {
            FAIL("Can't get the notebook model item moved to the stack from its model index");
        }

        if (sixthItem->type() != NotebookModelItem::Type::Notebook) {
            FAIL("Notebook model item has wrong type after being moved to the stack: " << *sixthItem);
        }

        const NotebookItem * sixthNotebookItem = sixthItem->notebookItem();
        if (!sixthNotebookItem) {
            FAIL("Notebook model item has null pointer to the notebook item even though it's of notebook type");
        }

        if (sixthNotebookItem->stack() != fifth.stack()) {
            FAIL("Notebook item's stack is not equal to the one expected as the notebook model item was moved to this stack: "
                 << fifth.stack() << "; notebook item: " << *sixthNotebookItem);
        }

        const NotebookModelItem * sixthParentItem = sixthItem->parent();
        if (!sixthParentItem) {
            FAIL("The notebook model item has null parent after it has been moved to the existing stack");
        }

        if (sixthParentItem->type() != NotebookModelItem::Type::Stack) {
            FAIL("The notebook model item has parent of non-stack type after it has been moved to the existing stack");
        }

        const NotebookStackItem * sixthParentStackItem = sixthParentItem->notebookStackItem();
        if (!sixthParentStackItem) {
            FAIL("The notebook model item has parent of stack type but with null pointer to the stack item "
                 "after the model item has been moved to the existing stack");
        }

        if (sixthParentStackItem->name() != fifth.stack()) {
            FAIL("The notebook model item has stack parent which name doesn't correspond to the expected one "
                 "after that item has been moved to the existing stack");
        }

        // Should be able to move the non-stacked item to the new stack
        const QString newStack = "My brand new stack";

        QModelIndex seventhIndex = model->indexForLocalUid(seventh.localUid());
        if (!seventhIndex.isValid()) {
            FAIL("Can't get the valid notebook model item index from the method intended to move the item to the stack");
        }

        QModelIndex seventhIndexMoved = model->moveToStack(seventhIndex, newStack);
        if (!seventhIndexMoved.isValid()) {
            FAIL("Can't get the valid notebook model item index from the method intended to move the item to the stack");
        }

        const NotebookModelItem * seventhItem = model->itemForIndex(seventhIndexMoved);
        if (!seventhItem) {
            FAIL("Notebook model item has null pointer to the notebook item even though it's of notebook type");
        }

        if (seventhItem->type() != NotebookModelItem::Type::Notebook) {
            FAIL("Notebook model item has wrong type after being moved to the stack: " << *seventhItem);
        }

        const NotebookItem * seventhNotebookItem = seventhItem->notebookItem();
        if (!seventhNotebookItem) {
            FAIL("Notebook model item has null pointer to the notebook item even though it's of notebook type");
        }

        if (seventhNotebookItem->stack() != newStack) {
            FAIL("Notebook item's stack is not equal to the one expected as the notebook model item was moved to the new stack: "
                 << newStack << "; notebook item: " << *seventhNotebookItem);
        }

        const NotebookModelItem * seventhParentItem = seventhItem->parent();
        if (!seventhParentItem) {
            FAIL("The notebook model item has null pointer after it has been moved to the new stack");
        }

        if (seventhParentItem->type() != NotebookModelItem::Type::Stack) {
            FAIL("The notebook model item has parent of non-stack type after it has been moved to the existing stack");
        }

        const NotebookStackItem * seventhParentStackItem = seventhParentItem->notebookStackItem();
        if (!seventhParentStackItem) {
            FAIL("The notebook model item has parent of stack type but with null pointer to the stack item "
                 "after the model item has been moved to the existing stack");
        }

        if (seventhParentStackItem->name() != newStack) {
            FAIL("The notebook model item has stack parent which name doesn't correspond to the expected one "
                 "after that item has been moved to the existing stack");
        }

        // TODO: add more manual tests here

        emit success();
        return;
    }
    catch(const IQuteNoteException & exception) {
        SysInfo & sysInfo = SysInfo::GetSingleton();
        QNWARNING("Caught QuteNote exception: " + exception.errorMessage() +
                  ", what: " + QString(exception.what()) + "; stack trace: " + sysInfo.GetStackTrace());
    }
    catch(const std::exception & exception) {
        SysInfo & sysInfo = SysInfo::GetSingleton();
        QNWARNING("Caught std::exception: " + QString(exception.what()) + "; stack trace: " + sysInfo.GetStackTrace());
    }
    catch(...) {
        SysInfo & sysInfo = SysInfo::GetSingleton();
        QNWARNING("Caught some unknown exception; stack trace: " + sysInfo.GetStackTrace());
    }

    emit failure();
}

void NotebookModelTestHelper::onAddNotebookFailed(Notebook notebook, QString errorDescription, QUuid requestId)
{
    QNDEBUG("NotebookModelTestHelper::onAddNotebookFailed: notebook = " << notebook << "\nError description = "
            << errorDescription << ", request id = " << requestId);

    emit failure();
}

void NotebookModelTestHelper::onUpdateNotebookFailed(Notebook notebook, QString errorDescription, QUuid requestId)
{
    QNDEBUG("NotebookModelTestHelper::onUpdateNotebookFailed: notebook = " << notebook << "\nError description = "
            << errorDescription << ", request id = " << requestId);

    emit failure();
}

void NotebookModelTestHelper::onFindNotebookFailed(Notebook notebook, QString errorDescription, QUuid requestId)
{
    QNDEBUG("NotebookModelTestHelper::onFindNotebookFailed: notebook = " << notebook << "\nError description = "
            << errorDescription << ", request id = " << requestId);

    emit failure();
}

void NotebookModelTestHelper::onListNotebooksFailed(LocalStorageManager::ListObjectsOptions flag, size_t limit, size_t offset,
                                                    LocalStorageManager::ListNotebooksOrder::type order,
                                                    LocalStorageManager::OrderDirection::type orderDirection,
                                                    QString linkedNotebookGuid, QString errorDescription, QUuid requestId)
{
    QNDEBUG("NotebookModelTestHelper::onListNotebooksFailed: flag = " << flag << ", limit = " << limit
            << ", offset = " << offset << ", order = " << order << ", direction = " << orderDirection
            << ", linked notebook guid = " << (linkedNotebookGuid.isNull() ? QString("<null>") : linkedNotebookGuid)
            << ", error description = " << errorDescription << ", request id = " << requestId);

    emit failure();
}

void NotebookModelTestHelper::onExpungeNotebookFailed(Notebook notebook, QString errorDescription, QUuid requestId)
{
    QNDEBUG("NotebookModelTestHelper::onExpungeNotebookFailed: notebook = " << notebook << "\nError description = "
            << errorDescription << ", request id = " << requestId);

    emit failure();
}

} // namespace qute_note
