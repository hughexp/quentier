#include "ToDoCheckboxUndoCommand.h"
#include "../NoteEditor_p.h"
#include <qute_note/logging/QuteNoteLogger.h>

namespace qute_note {

ToDoCheckboxUndoCommand::ToDoCheckboxUndoCommand(const quint64 enToDoIdNumber, NoteEditorPrivate & noteEditorPrivate,
                                                 QUndoCommand * parent) :
    INoteEditorUndoCommand(noteEditorPrivate, parent),
    m_enToDoIdNumber(enToDoIdNumber)
{
    init();
}

ToDoCheckboxUndoCommand::ToDoCheckboxUndoCommand(const quint64 enToDoIdNumber, NoteEditorPrivate & noteEditorPrivate,
                                                 const QString & text, QUndoCommand * parent) :
    INoteEditorUndoCommand(noteEditorPrivate, text, parent),
    m_enToDoIdNumber(enToDoIdNumber)
{
    init();
}

ToDoCheckboxUndoCommand::~ToDoCheckboxUndoCommand()
{}

void ToDoCheckboxUndoCommand::redoImpl()
{
    QNDEBUG("ToDoCheckboxUndoCommand::redoImpl");
    m_noteEditorPrivate.flipEnToDoCheckboxState(m_enToDoIdNumber);
}

void ToDoCheckboxUndoCommand::undoImpl()
{
    QNDEBUG("ToDoCheckboxUndoCommand::undoImpl");
    m_noteEditorPrivate.flipEnToDoCheckboxState(m_enToDoIdNumber);
}

void ToDoCheckboxUndoCommand::init()
{
    QUndoCommand::setText(QObject::tr("Change ToDo state"));
}

} // namespace qute_note
