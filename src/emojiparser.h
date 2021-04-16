#ifndef DET_EMOJIPARSER_H
#define DET_EMOJIPARSER_H

#include <QObject>
#include <QRegularExpression>

namespace Det {

class EmojiParser : public QObject
{
    Q_OBJECT
public:
    explicit EmojiParser(const QString& twemojiPrefix, QObject *parent = nullptr);

    Q_SCRIPTABLE QString parse(const QString& input, int emojiSize);
    Q_SCRIPTABLE QString parseText(const QString& input, int emojiSize);

    static void registerType();
private:
    QRegularExpression m_re;
    QString m_twemojiPrefix;

    QString replace(const QStringRef& match, const QString &emojiSize);
};

} // namespace Det

#endif // DET_EMOJIPARSER_H
