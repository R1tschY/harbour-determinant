#include "secretsservice.h"

#include <QLoggingCategory>

#include <Secrets/collectionnamesrequest.h>
#include <Secrets/secretmanager.h>

namespace Det {

using namespace Sailfish::Secrets;

static Q_LOGGING_CATEGORY(logger, "determinant.secretsservice");

SecretsService::SecretsService(QObject* parent)
    : QObject(parent)
    , m_service(new SecretManager())
{
    connect(m_service, &SecretManager::isInitializedChanged,
        this, &SecretsService::onInitialized);
}

void SecretsService::onInitialized()
{
    if (!m_service->isInitialized())
        return;

    qCInfo(logger) << "SecretManager initialized";

    auto* namesRequest = new CollectionNamesRequest(this);
    namesRequest->setManager(m_service);
    connect(namesRequest, &CollectionNamesRequest::resultChanged,
        this, [namesRequest]() {
            qCDebug(logger) << "Secret: Collections" << namesRequest->collectionNames();
            qCDebug(logger) << "Secret: Storage" << namesRequest->storagePluginName();
        });
    namesRequest->startRequest();
}

} // namespace Det
