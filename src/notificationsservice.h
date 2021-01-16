#ifndef DET_NOTIFICATIONSSERVICE_H
#define DET_NOTIFICATIONSSERVICE_H

#include <QObject>

namespace Det {

class NotificationsService : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsService(QObject *parent = nullptr);

signals:

};

} // namespace Det

#endif // DET_NOTIFICATIONSSERVICE_H
