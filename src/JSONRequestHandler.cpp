#include "JSONRequestHandler.h"

#include "MyWebServiceApp.h"
#include <Poco/JSON/Object.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>

JSONRequestHandler::JSONRequestHandler(MyWebServiceApp& webServiceApp)
    :m_webServiceApp(webServiceApp)
{
}

void JSONRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    response.setContentType("application/json");
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

    std::ostream& out = response.send();

    if (request.getMethod() == "GET")
    {
        std::string command = request.getURI();
        const Poco::URI uri(command);
        const Poco::URI::QueryParameters queryParams = uri.getQueryParameters();

        m_webServiceApp.HandleRequests(uri, queryParams, response, out);
        

        //Poco::JSON::Object jsonResponse;
        //jsonResponse.set("message", "Hello from REST API");
        //jsonResponse.set("status", "success");
        //jsonResponse.set("uri", uri);

        //jsonResponse.stringify(out);
    }
    else
    {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        Poco::JSON::Object errorResponse;
        errorResponse.set("status", "error");
        errorResponse.set("message", "Unsupported HTTP method");
        errorResponse.stringify(out);
    }
}

JSONRequestHandlerFactory::JSONRequestHandlerFactory(MyWebServiceApp& webServiceApp)
    :m_webServiceApp(webServiceApp)
{
}

Poco::Net::HTTPRequestHandler* JSONRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest&)
{
    return new JSONRequestHandler(m_webServiceApp);
}
