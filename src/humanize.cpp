/*
 * Copyright 2020 Richard Liebscher <richard.liebscher@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "humanize.h"

#include <array>
#include <cmath>
#include <algorithm>

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

static std::array<QString, 5> bytesUnits{
    QStringLiteral(" B"),
    QStringLiteral(" kB"),
    QStringLiteral(" MB"),
    QStringLiteral(" GB"),
    QStringLiteral(" TB")
};

QString Humanize::humanizeBytes(qint64 bytes) {
    std::size_t dim;
    if (bytes != 0) {
        dim = std::log(std::abs(bytes)) / std::log(1024);
        dim = std::min(dim, bytesUnits.size() - 1);
    } else {
        dim = 0;
    }

    if (dim != 0) {
        return QString::number(bytes / pow(1024, dim), 'f', 2) + bytesUnits[dim];
    } else {
        return QString::number(bytes) + bytesUnits[0];
    }
}

} // namespace Det
