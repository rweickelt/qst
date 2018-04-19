#ifndef EXEMPTION_H
#define EXEMPTION_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QVector>
#include <QtQml/QQmlListProperty>

#include "parsereventhandler.h"

class Dimension : public QObject, public ParserEventHandler
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "children")
    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)

    Q_PROPERTY(QStringList testcases READ testcases WRITE setTestcases NOTIFY testcasesChanged)

signals:
    void testcasesChanged();

public:
    Dimension(QObject* parent = nullptr);
    QQmlListProperty<QObject> children();
    const QVector<QVariantMap>& data() const;
    int length() const;
    QStringList testcases() const;
    void setTestcases(const QStringList& names);

protected:
    void handleParserEvent(ParserEventHandler::ParserEvent event);

private:
    QList<QObject *> m_children;
    QStringList m_testcases;
    QVector<QVariantMap> m_data;
};

inline QQmlListProperty<QObject> Dimension::children() { return QQmlListProperty<QObject>(this, m_children); }
inline QStringList Dimension::testcases() const { return m_testcases; }

#endif // EXEMPTION_H
