#ifndef SECRETSSERVICE_H
#define SECRETSSERVICE_H

#include <QObject>

namespace Sailfish {
namespace Secrets {
    class SecretManager;
} // namespace Secret
} // namespace Sailfish

namespace Det {

class SecretsService : public QObject {
    Q_OBJECT
public:
    explicit SecretsService(QObject* parent = nullptr);

signals:

public slots:

private:
    Sailfish::Secrets::SecretManager* m_service;

    void onInitialized();
};

} // namespace Det

#endif // SECRETSSERVICE_H
