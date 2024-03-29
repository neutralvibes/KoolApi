
#include "KoolApiRequestsAsyncWebserver.h"
#ifdef _ESPAsyncWebServer_H_


void ApiAsyncWebRequest::_dispatch(int code) const
{
  auto len = measureJson(outdoc);

  AsyncResponseStream *response = _request->beginResponseStream("application/json", len);
  response->addHeader("Cache-Control", "no-store");
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Headers", "*");
  response->addHeader("Access-Control-Allow-Credentials", "true");

  response->setCode(code);
  serializeJson(outdoc, *response);
  _request->send(response);
}

void ApiAsyncWebRequest::_sendOptions() const
{
  uint8_t optsLen = outdoc["options"].size();
  uint8_t pos = 0;

  auto len = measureJson(outdoc);
  AsyncResponseStream *resp = _request->beginResponseStream("application/json", len);

  resp->addHeader("Access-Control-Allow-Origin", "*");
  resp->addHeader("Access-Control-Allow-Headers", "*");

  if (optsLen)
  {
    char optsBuff[optsLen * 10];
    optsBuff[0] = 0;

    for (uint8_t i = 0; i < optsLen; ++i)
    {
      strcat(optsBuff, outdoc["options"][i]);
      
      ++pos;

      if (pos < optsLen)
        strcat(optsBuff, ", ");
    }

    resp->addHeader("Access-Control-Allow-Methods", optsBuff);
  }

  resp->setCode(200);
  serializeJson(outdoc, *resp);
  _request->send(resp);
}

int ApiAsyncWebRequest::parse(const char *urlBase, const char *requestKey)
{
  this->_method = koolApiMethodMap.isValid((api_method_t)_request->method(), API_METHOD_UNKNOWN);

  if (this->_method == API_METHOD_UNKNOWN)
  {
    return 405;
  }

  // Only read json if a body request
  if (_isBody)
  {
    _deserializationError = deserializeJson(doc, _data, _len);

    if (_deserializationError || doc.isNull() || !doc.is<JsonObject>())
    {
      return 400;
    }
  }

  int bl = strlen(urlBase);
  auto url = _request->url().c_str();

  this->uri = url + bl + 1;

  // Get requests should have no json body
  if (this->_method != API_METHOD_GET)
  {
    this->json = doc.as<JsonObject>();
  }

  this->_out = outdoc.to<JsonObject>();
  this->params = new ApiAsyncParams(_request);

  return 0;
};

void ApiAsyncWebSocket::_dispatch(int code) const
{
  auto len = measureJson(outdoc);

  if (len)
  {
    AsyncWebSocketMessageBuffer *buffer = _ws->makeBuffer(len);
    serializeJson(outdoc, buffer->get(), len + 1);
    _client->text(buffer);
  }
}

int ApiAsyncWebSocket::parse(const char *urlBase, const char *requestKey)
{
  _deserializationError = deserializeJson(doc, _data, _len);

  if (_deserializationError || doc.isNull() || !doc.is<JsonObject>())
  {
    return 400;
  }

  auto jParse = doc.as<JsonObject>();

  this->_id = jParse["id"];

  this->uri = jParse[requestKey];

  const char *methodStr = jParse["method"];

  this->_method = koolApiMethodMap.textToCode(methodStr, API_METHOD_UNKNOWN);

  // Get requests should have no json body
  if (_method != API_METHOD_GET)
  {
    this->json = jParse["body"];
  }

  this->_out = outdoc.to<JsonObject>();

  this->params = new ApiJsonParams(jParse["params"].as<JsonObject>());

  return 0;
};

#endif
