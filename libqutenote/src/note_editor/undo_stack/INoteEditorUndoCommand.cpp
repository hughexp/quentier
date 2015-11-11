#include "INoteEditorUndoCommand.h"
#include "../NoteEditor_p.h"

namespace qute_note {

INoteEditorUndoCommand::INoteEditorUndoCommand(NoteEditorPrivate & noteEditorPrivate, QUndoCommand * parent) :
    QUndoCommand(parent),
    m_noteEditorPrivate(noteEditorPrivate)
{}

INoteEditorUndoCommand::INoteEditorUndoCommand(NoteEditorPrivate & noteEditorPrivate, const QString & text, QUndoCommand * parent) :
    QUndoCommand(text, parent),
    m_noteEditorPrivate(noteEditorPrivate)
{}

INoteEditorUndoCommand::~INoteEditorUndoCommand()
{}

} // namespace qute_note
