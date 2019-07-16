#include "store.h"

namespace Determinant {

Store::Store(QObject *parent) : QObject(parent)
{

}

QString Store::userName() const
{
    return m_userName;
}

void Store::setUserName(const QString &userName)
{
    m_userName = userName;
}

QString Store::accessToken() const
{
    return m_accessToken;
}

void Store::setAccessToken(const QString &accessToken)
{
    m_accessToken = accessToken;
}

QString Store::homeServer() const
{
    return m_homeServer;
}

void Store::setHomeServer(const QString &homeServer)
{
    m_homeServer = homeServer;
}

} // namespace Determinant
