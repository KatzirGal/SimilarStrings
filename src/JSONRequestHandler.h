#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class MyWebServiceApp;

class JSONRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    JSONRequestHandler(MyWebServiceApp& webServiceApp);

public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

private:
    MyWebServiceApp& m_webServiceApp;
};

class JSONRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public: 
    JSONRequestHandlerFactory(MyWebServiceApp& webServiceApp);

public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest&) override;

private:
    MyWebServiceApp& m_webServiceApp;
};