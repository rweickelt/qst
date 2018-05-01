#ifndef EXEMPTION_H
#define EXEMPTION_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QVector>

#include <QtQml/private/qqmlcustomparser_p.h>

#include "qstitem.h"

class Dimension : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList testcases READ testcases WRITE setTestcases NOTIFY testcasesChanged)

    friend class DimensionParser;

signals:
    void testcasesChanged();

public:
    Dimension(QObject* parent = nullptr);
    const QVector<QVariantMap>& data() const;
    int length() const;
    QStringList testcases() const;
    void setTestcases(const QStringList& names);

protected:
    void handleParserEvent(QstItem::ParserEvent event) override;
    void setData(const QVector<QVariantMap>& data);

private:

    QStringList m_testcases;
    QVector<QVariantMap> m_data;
};

inline QStringList Dimension::testcases() const { return m_testcases; }

class DimensionParser : public QQmlCustomParser
{
public:
    void verifyBindings(const QV4::CompiledData::Unit* qmlUnit, const QList<const QV4::CompiledData::Binding*>& props) override;
    void applyBindings(QObject* object, QV4::CompiledData::CompilationUnit* compilationUnit, const QList<const QV4::CompiledData::Binding*> &bindings) override;
private:
    bool definesEmptyList(const QString& script);
};


#endif // EXEMPTION_H
