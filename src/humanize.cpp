#include "humanize.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>

namespace Det {

QString Humanize::formatTime(const QTime& time)
{
    if (m_timeFormat == TimeFormat12Hours) {
        return time.toString(QStringLiteral("hh:mm A"));
    } else {
        return time.toString(QStringLiteral("hh:mm"));
    }
}

QString Humanize::formatDate(const QDate& date)
{
    QDate now = QDate::currentDate();
    qint64 days = date.daysTo(now);
    if (days == 0) {
        return tr("today");
    } else if (days == 1) {
        return tr("yesterday");
    } else if (days < 6) {
        return date.toString(QStringLiteral("dddd"));
    } else if (days < 350) {
        return date.toString(QStringLiteral("d MMM"));
    }

    return date.toString(Qt::SystemLocaleShortDate);
}

QString Humanize::formatDateTime(const QDateTime& dateTime)
{
    QDate now = QDate::currentDate();
    qint64 days = dateTime.date().daysTo(now);
    if (days == 0) {
        return formatTime(dateTime.time());
    } else if (days == 1) {
        return tr("yesterday");
    } else if (days < 6) {
        return dateTime.date().toString(QStringLiteral("dddd"));
    } else if (days < 350) {
        return dateTime.date().toString(QStringLiteral("d MMM"));
    }

    return dateTime.date().toString(Qt::SystemLocaleShortDate);
}

void Humanize::setTimeFormat(TimeFormat timeFormat)
{
    if (timeFormat == m_timeFormat)
        return;

    m_timeFormat = timeFormat;
    emit timeFormatChanged();
}

// TODO: move to customized Room/User
qreal Humanize::stringToHue(const QString& str)
{
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(),
        QCryptographicHash::Sha1);
    quint16 hashValue = quint16(hash[0] | hash[1] << 8);
    return qreal(hashValue) / std::numeric_limits<quint16>::max();
}

} // namespace Det
