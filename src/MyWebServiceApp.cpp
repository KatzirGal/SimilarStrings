#include "MyWebServiceApp.h"

#include "PermutationTrie.h"
#include <filesystem>
#include <chrono>
#include <iostream>
#include <string>
#ifndef _WIN32
#include <mutex>
#endif

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/JSON/Object.h>

MyWebServiceApp::MyWebServiceApp()
    : m_totalRequests(0), m_totalProcessingTimeNs(0)
{
}

int MyWebServiceApp::main(const std::vector<std::string>& argv)
{
    int retVal = PrepareCommandsHandler(argv);
    if (retVal) return retVal;

    return StartServer();
}

int MyWebServiceApp::StartServer()
{
    unsigned short port = 8000;
    Poco::Net::ServerSocket svs(port);
    Poco::Net::HTTPServer server(new JSONRequestHandlerFactory(*this), svs, new Poco::Net::HTTPServerParams());

    std::cout << "Starting REST API server on port " << port << std::endl;
    server.start();

    waitForTerminationRequest(); // Wait for CTRL-C or kill
    std::cout << "Stopping server..." << std::endl;
    server.stop();

    return Application::EXIT_OK;
}

int MyWebServiceApp::PrepareCommandsHandler(const Poco::Util::AbstractConfiguration::Keys& argv)
{
    size_t argc = argv.size();
    if (argc < 1)
    {
        std::cerr << "Error: not enough parameters\n";
        std::cout << "SimilarStrings.exe '<Path to the dictionary>'" << std::endl;
        return 1;
    }

    std::string filePath = argv[0];
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Error: File does not exist!" << std::endl;
        return 1;
    }

    m_commandsHandler.InitPermutationTrie(filePath);
    
    return 0;
}

bool MyWebServiceApp::HandleRequests(const Poco::URI& uri, const Poco::URI::QueryParameters& queryParameters, Poco::Net::HTTPServerResponse& response, std::ostream& out)
{
    std::unordered_map<std::string, std::string> params;
    for (const auto& queryParameter : queryParameters) {
        params[queryParameter.first] = queryParameter.second;
    }


    if (uri.getPath() == "/api/v1/similar")
    {
        if (params.count("word") == 0)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_UNPROCESSABLE_ENTITY);
            Poco::JSON::Object errorResponse;
            errorResponse.set("status", "error");
            errorResponse.set("message", "Missing Parameter");
            errorResponse.stringify(out);
            return false;
        }

        std::vector<std::string> similarWords;
        if (!m_commandsHandler.OnSimilar(params["word"], similarWords))
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            Poco::JSON::Object errorResponse;
            errorResponse.set("status", "error");
            errorResponse.set("message", "Internal Error");
            errorResponse.stringify(out);
            return false;
        }

        
        std::chrono::time_point start = std::chrono::high_resolution_clock::now();

        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        Poco::JSON::Object responseObj;
        Poco::JSON::Array similarWordsList;
        
        for (const std::string& similarWord : similarWords)
        {
            similarWordsList.add(similarWord);
        }

        responseObj.set("similar", similarWordsList);
        responseObj.stringify(out);

        std::chrono::time_point stop = std::chrono::high_resolution_clock::now();
        std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

        {
            std::unique_lock lock(m_rwMutex);
            ++m_totalRequests;
            m_totalProcessingTimeNs += duration.count();
        }

        return true;
    }
    else if (uri.getPath() == "/api/v1/stats") // stats will handle also here
    {
        Poco::JSON::Object responseObj;
        m_commandsHandler.OnStats(responseObj);

        int totalRequests = 0;
        long long avgProcessingTimeNs = 0;
        {
            std::shared_lock lock(m_rwMutex);
            totalRequests = m_totalRequests;
            avgProcessingTimeNs = m_totalRequests ? m_totalProcessingTimeNs / m_totalRequests : 0;
        }
        
        responseObj.set("totalRequests", totalRequests);
        responseObj.set("avgProcessingTimeNs", avgProcessingTimeNs);

        responseObj.stringify(out);

        return true;
    }

    return false;
}
