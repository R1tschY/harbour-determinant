#ifndef HUMANIZE_H
#define HUMANIZE_H

#include <QObject>

class QString;
class QDateTime;

namespace Det {

class Humanize : public QObject {
    Q_OBJECT
    Q_PROPERTY(TimeFormat timeFormat READ timeFormat WRITE setTimeFormat
            NOTIFY timeFormatChanged)
public:
    enum TimeFormat : bool {
        TimeFormat12Hours,
        TimeFormat24Hours
    };
    Q_ENUM(TimeFormat)

    Q_INVOKABLE QString formatTime(const QTime& time);
    Q_INVOKABLE QString formatDate(const QDate& date);
    Q_INVOKABLE QString formatDateTime(const QDateTime& time);

    TimeFormat timeFormat() const { return m_timeFormat; }
    void setTimeFormat(TimeFormat timeFormat);

    Q_INVOKABLE qreal stringToHue(const QString& str);

signals:
    void timeFormatChanged();

private:
    TimeFormat m_timeFormat;
};

} // namespace Det

#endif // HUMANIZE_H
