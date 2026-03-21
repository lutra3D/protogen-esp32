#include "JsonEndpoint.hpp"

void JsonEndpoint::addJsonHandler(AsyncWebServer &server, WebRequestMethodComposite method, const char *uri, JsonRouteHandler handler)
{
  server.on(
      uri,
      method,
      [](AsyncWebServerRequest *request)
      {
        // Response is sent in body callback
      },
      nullptr,
      [handler](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        // First chunk: validate content type and allocate buffer
        if (index == 0)
        {
          if (!request->hasHeader("Content-Type"))
          {
            request->send(400, "text/plain", "error: Missing Content-Type header");
            return;
          }

          auto header = request->getHeader("Content-Type");
          if (header->value().indexOf("application/json") == -1)
          {
            request->send(415, "text/plain", "error: Content-Type must be application/json");
            return;
          }

          JsonBodyContext *ctx = new JsonBodyContext();
          ctx->body.reserve(total);
          request->_tempObject = ctx;
        }

        if (!request->_tempObject)
        {
          return;
        }

        JsonBodyContext *ctx = reinterpret_cast<JsonBodyContext *>(request->_tempObject);

        for (size_t i = 0; i < len; i++)
        {
          ctx->body += static_cast<char>(data[i]);
        }

        // Last chunk: deserialize and dispatch
        if (index + len == total)
        {
          JsonDocument doc;
          DeserializationError err = deserializeJson(doc, ctx->body);

          if (err)
          {
            delete ctx;
            request->_tempObject = nullptr;

            request->send(400, "text/plain", "error: Invalid JSON");
            return;
          }

          auto response = handler(request, doc);

          delete ctx;
          request->_tempObject = nullptr;

          request->send(response.statusCode, response.contentType, response.body);
        }
      });
}