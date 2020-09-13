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
