#ifndef MATRIX_H
#define MATRIX_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtQml/QQmlListProperty>

#include "qstitem.h"

class Dimension;

class Matrix : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList testcases READ testcases WRITE setTestcases NOTIFY testcasesChanged)

signals:
    void testcasesChanged();

public:
    enum Scope {
        ProjectScope,
        TestcaseScope
    };

    Matrix(QObject* parent = nullptr);
    QList<Dimension*> dimensions() const;
    QStringList testcases() const;
    Scope scope() const;
    void setTestcases(const QStringList& names);

protected:
    void handleParserEvent(QstItem::ParserEvent event);

private:
    Scope m_scope;
    QStringList m_testcases;
};

inline Matrix::Scope Matrix::scope() const { return m_scope; }
inline QStringList Matrix::testcases() const { return m_testcases; }

#endif // MATRIX_H
