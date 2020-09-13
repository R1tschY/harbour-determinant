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
