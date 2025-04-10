#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include "command.h"

#include <QObject>
#include <QStack>


class CommandManager: public QObject {
    Q_OBJECT

public:
    explicit CommandManager(QObject *parent = nullptr);
    ~CommandManager();

    void execute(Command *command);
    void undo();
    void redo();

    bool canUndo() const;
    bool canRedo() const;

    void clear();

signals:
    void undoRedoStateChanged();

private:
    QStack<Command*> m_undoStack;
    QStack<Command*> m_redoStack;
};

#endif // COMMANDMANAGER_H
