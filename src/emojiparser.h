#ifndef DET_EMOJIPARSER_H
#define DET_EMOJIPARSER_H

#include <QObject>
#include <QRegularExpression>

namespace Det {

class EmojiParser : public QObject
{
    Q_OBJECT
public:
    explicit EmojiParser(QObject *parent = nullptr);

    Q_SCRIPTABLE QString parse(const QString& input, int emojiSize);

private:
    QRegularExpression m_re;

    QString replace(const QStringRef& match, const QString &emojiSize);
};

} // namespace Det

#endif // DET_EMOJIPARSER_H
