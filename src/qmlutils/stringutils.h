#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QObject>

namespace Det {

class StringUtils : public QObject
{
    Q_OBJECT
public:
    explicit StringUtils(QObject *parent = nullptr);

    static void registerType();

    Q_SCRIPTABLE int getEmoijChainLength(const QString& input) const;
    Q_SCRIPTABLE bool isSymbol(const QString& input) const;
};

} // namespace Det

#endif // STRINGUTILS_H
