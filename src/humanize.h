#ifndef HUMANIZE_H
#define HUMANIZE_H

#include <QObject>

class QString;
class QDateTime;


namespace Det {

class Humanize : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE static QString formatTime(const QTime& time);
    Q_INVOKABLE static QString formatDate(const QDate& date);
    Q_INVOKABLE static QString formatDateTime(const QDateTime& time);
};

} // namespace Det

#endif // HUMANIZE_H
