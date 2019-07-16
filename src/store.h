#ifndef STORE_H
#define STORE_H

#include <QObject>

namespace Determinant {

class Store : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName
               READ userName
               WRITE setUserName
               NOTIFY userNameChanged)

    Q_PROPERTY(QString accessToken
               READ accessToken
               WRITE setAccessToken
               NOTIFY accessTokenChanged)

    Q_PROPERTY(QString homeServer
               READ homeServer
               WRITE setHomeServer
               NOTIFY homeServerChanged)
public:
    explicit Store(QObject *parent = nullptr);

    QString userName() const;
    void setUserName(const QString &userName);

    QString accessToken() const;
    void setAccessToken(const QString &accessToken);

    QString homeServer() const;
    void setHomeServer(const QString &homeServer);

signals:
    void userNameChanged();
    void accessTokenChanged();
    void homeServerChanged();

public slots:

private:
    QString m_userName;
    QString m_accessToken;
    QString m_homeServer;
};

} // namespace Determinant

#endif // STORE_H
