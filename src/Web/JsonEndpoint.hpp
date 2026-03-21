#ifndef JSONENDPOINT_HPP
#define JSONENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

struct JsonBodyContext
{
  String body;
};

struct Response
{
    String body;
    String contentType = "application/json";
    int statusCode = 200;
    Response(String body, String contentType = "application/json", int statusCode = 200)
        : body(std::move(body)), contentType(std::move(contentType)), statusCode(statusCode) {}
};

using JsonRouteHandler = std::function<Response(
    AsyncWebServerRequest *request,
    JsonDocument &doc)>;

class JsonEndpoint
{
protected:
    void addJsonHandler(AsyncWebServer &server, WebRequestMethodComposite method, const char *uri, JsonRouteHandler handler);
};

#endif // JSONENDPOINT_HPP