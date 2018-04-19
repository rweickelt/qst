#ifndef MATRIX_H
#define MATRIX_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtQml/QQmlListProperty>

#include "parsereventhandler.h"

class Dimension;

class Matrix : public QObject, public ParserEventHandler
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "children")
    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)

    Q_PROPERTY(QStringList testcases READ testcases WRITE setTestcases NOTIFY testcasesChanged)

signals:
    void testcasesChanged();

public:
    enum Scope {
        ProjectScope,
        TestcaseScope
    };

    Matrix(QObject* parent = nullptr);
    QQmlListProperty<QObject> children();
    QList<Dimension*> dimensions() const;
    QStringList testcases() const;
    Scope scope() const;
    void setTestcases(const QStringList& names);

protected:
    void handleParserEvent(ParserEventHandler::ParserEvent event);

private:
    QList<QObject *> m_children;
    Scope m_scope;
    QStringList m_testcases;
};

inline QQmlListProperty<QObject> Matrix::children() { return QQmlListProperty<QObject>(this, m_children); }
inline Matrix::Scope Matrix::scope() const { return m_scope; }
inline QStringList Matrix::testcases() const { return m_testcases; }

#endif // MATRIX_H
