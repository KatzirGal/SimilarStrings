#pragma once

#include "JSONRequestHandler.h"

#include "CommandsHandler.h"
#include <shared_mutex>
#include <vector>

#include <Poco/URI.h>
#include <Poco/Util/ServerApplication.h>

class MyWebServiceApp : public Poco::Util::ServerApplication
{
public:
    MyWebServiceApp();

protected:
    int main(const std::vector<std::string>&) override;

private:
    int StartServer();

    int PrepareCommandsHandler(const Poco::Util::AbstractConfiguration::Keys& argv);

public:
    bool HandleRequests(const Poco::URI& uri, const Poco::URI::QueryParameters& params, Poco::Net::HTTPServerResponse& response, std::ostream& out);

private:
    CommandsHandler m_commandsHandler;

    std::shared_mutex m_rwMutex;
    int m_totalRequests;
    long long m_totalProcessingTimeNs;
};

