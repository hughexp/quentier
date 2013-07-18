#ifndef __QUTE_NOTE__TODO_CHECKBOX_TEXT_OBJECT_H
#define __QUTE_NOTE__TODO_CHECKBOX_TEXT_OBJECT_H

#include <QTextObjectInterface>

QT_FORWARD_DECLARE_CLASS(QTextDocument)
QT_FORWARD_DECLARE_CLASS(QTextFormat)
QT_FORWARD_DECLARE_CLASS(QPainter)
QT_FORWARD_DECLARE_CLASS(QRectF)
QT_FORWARD_DECLARE_CLASS(QSizeF)

// NOTE: I have to declare two classes instead of a template one as Q_OBJECT does not support template classes

class ToDoCheckboxTextObjectUnchecked: public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit ToDoCheckboxTextObjectUnchecked() {}
    virtual ~ToDoCheckboxTextObjectUnchecked() override {}

public:
    // QTextObjectInterface
    virtual void drawObject(QPainter * pPainter, const QRectF & rect,
                            QTextDocument * pDoc, int positionInDocument,
                            const QTextFormat & format) final override;
    virtual QSizeF intrinsicSize(QTextDocument * pDoc, int positionInDocument,
                                 const QTextFormat & format) final override;
};

class ToDoCheckboxTextObjectChecked: public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit ToDoCheckboxTextObjectChecked() {}
    virtual ~ToDoCheckboxTextObjectChecked() override {}

public:
    // QTextObjectInterface
    virtual void drawObject(QPainter * pPainter, const QRectF & rect,
                            QTextDocument * pDoc, int positionInDocument,
                            const QTextFormat & format) final override;
    virtual QSizeF intrinsicSize(QTextDocument * pDoc, int positionInDocument,
                                 const QTextFormat & format) final override;
};

#endif // __QUTE_NOTE__TODO_CHECKBOX_TEXT_OBJECT_H