#include "commandmanager.h"


CommandManager::CommandManager(QObject *parent): QObject(parent) {}

CommandManager::~CommandManager()
{
    clear();
}

void CommandManager::execute(Command *command)
{
    command->execute();
    m_undoStack.push(command);

    // On command execution
    for (Command *cmd : m_redoStack) {
        delete cmd;
    }
    m_redoStack.clear();

    emit undoRedoStateChanged();
}

void CommandManager::undo()
{
    if (m_undoStack.isEmpty()) return;

    Command *command = m_undoStack.pop();
    command->undo();
    m_redoStack.push(command);

    emit undoRedoStateChanged();
}

void CommandManager::redo()
{
    if (m_redoStack.isEmpty()) return;

    Command *command = m_redoStack.pop();
    command->redo();
    m_undoStack.push(command);

    emit undoRedoStateChanged();
}

bool CommandManager::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool CommandManager::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void CommandManager::clear()
{
    for (Command *command : m_undoStack) {
        delete command;
    }
    m_undoStack.clear();

    for (Command *command : m_redoStack) {
        delete command;
    }
    m_redoStack.clear();

    emit undoRedoStateChanged();
}
