#pragma once

#include "KoolApiBases.h"

class ApiJsonParams : public ApiParamBase
{
  friend class KoolApi;

  JsonObject _params;

public:
  ApiJsonParams(JsonObject params) : _params(params)
  {
  }

  virtual ~ApiJsonParams(){}

  const int length() const override
  {
    return _params.size();
  }

  bool has(const char *name) const override
  {
    return _params.containsKey(name);
  }

  String get(const char *name) const override
  {
    return _params[name].as<String>();
  }
};

/**
 * @brief KoolApi input from a char array source
 * 
 */
class ApiCharRequest : public ApiRequest
{
public:
  /**
   * @brief Process char request without output. Uses less memory
   * 
   * @param jsonIn 
   */
  ApiCharRequest(char *jsonIn);

  /**
   * @brief Process const char request without output. Uses more memory for const char
   * 
   * @param jsonIn 
   */
  ApiCharRequest(const char *jsonIn);

  /**
   * @brief Process char request placing any output in `output`
   * 
   * @param jsonIn json
   * @param output output char[]
   * @param maxLength size of output char[]
   */
  ApiCharRequest(char *jsonIn, char *output, size_t maxLength);

  /**
   * @brief Process const char request placing any output in `output`
   * 
   * @param jsonIn json
   * @param output output char[]
   * @param maxLength size of output char[]
   */
  ApiCharRequest(const char *jsonIn, char *output, size_t maxLength);

  virtual ~ApiCharRequest();

protected:
  void _dispatch(int code) const override;
  virtual int parse(const char *urlBase, const char *requestKey) override;

private:
  friend class KoolApi;

  const char *_jsonInConst;
  char *_jsonIn;
  char *_output;
  size_t _maxLength = 0;
  bool _isConst = false;
};

ApiCharRequest::ApiCharRequest(char *jsonIn) : _jsonIn(jsonIn),
                                               _maxLength(0),
                                               _isConst(false)

{
}

ApiCharRequest::ApiCharRequest(const char *jsonIn) : _jsonInConst(jsonIn),
                                                     _maxLength(0),
                                                     _isConst(true)

{
}

ApiCharRequest::ApiCharRequest(char *jsonIn, char *output, size_t maxLength) : _jsonIn(jsonIn),
                                                                               _output(output),
                                                                               _maxLength(maxLength),
                                                                               _isConst(false)

{
}

ApiCharRequest::ApiCharRequest(const char *jsonIn, char *output, size_t maxLength) : _jsonInConst(jsonIn),
                                                                                     _output(output),
                                                                                     _maxLength(maxLength),
                                                                                     _isConst(true)
{
}

ApiCharRequest::~ApiCharRequest()
{
}

void ApiCharRequest::_dispatch(int code) const
{
  if (_maxLength && !outdoc.isNull())
  {
    serializeJson(outdoc, _output, _maxLength);
  }
}

int ApiCharRequest::parse(const char *urlBase, const char *requestKey)
{
  _deserializationError = (_isConst) ? deserializeJson(doc, _jsonInConst) : deserializeJson(doc, _jsonIn);

  if (_deserializationError || doc.isNull() || !doc.is<JsonObject>())
  {
    return 400;
  }

  JsonObject jParse = doc.as<JsonObject>();

  this->_id = jParse["id"];

  this->uri = jParse[requestKey];

  const char *methodStr = jParse["method"];

  this->_method = koolApiMethodMap.textToCode(methodStr, API_METHOD_UNKNOWN);

  if (_method != API_METHOD_GET)
  {
    // Get requests should have no json body
    this->json = jParse["body"];
  }

  this->_out = outdoc.to<JsonObject>();

  this->params = new ApiJsonParams(jParse["params"].as<JsonObject>());

  return 0;
}

#ifdef _ESPAsyncWebServer_H_

class ApiAsyncParams : public ApiParamBase
{
  AsyncWebServerRequest *_request;
  bool _isPost = false;
  bool _isFile = false;

  friend class ApiProcessor;

public:
  ApiAsyncParams(AsyncWebServerRequest *request, bool isPost = false, bool isFile = false)
      : _request(request), _isPost(isPost)
  {
  }

  inline const int length() const override
  {
    return _request->params();
  }

  bool has(const char *name) const override
  {
    return _request->hasParam(name, _isPost, _isFile);
  }

  String get(const char *name) const override
  {
    AsyncWebParameter *p = _request->getParam(name, _isPost);
    return (p != nullptr) ? p->value() : String();
  }
};

class ApiAsyncWebRequest : public ApiRequest
{
  
public:
  virtual ~ApiAsyncWebRequest();
  ApiAsyncWebRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len);

  ApiAsyncWebRequest(AsyncWebServerRequest *request);

protected:
  void _dispatch(int code) const override;
  virtual void _sendOptions() const override;
  virtual int parse(const char *urlBase, const char *requestKey) override;

private:
  friend class KoolApi;

  /**
     * @brief Pointer to async request
     * 
     */
  AsyncWebServerRequest *_request;

  /**
     * @brief Data supplied
     * 
     */
  uint8_t *_data = nullptr;

  /**
     * @brief Length of data supplied
     * 
     */
  int _len = 0;

  /**
     * @brief Is a body request
     * 
     */
  bool _isBody = false;
};

ApiAsyncWebRequest::ApiAsyncWebRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len)
    : _request(request),
      _data(data),
      _len(len),
      _isBody(true)
{
}

ApiAsyncWebRequest::ApiAsyncWebRequest(AsyncWebServerRequest *request) : _request(request),
                                                                         _isBody(false)
{
}

ApiAsyncWebRequest::~ApiAsyncWebRequest()
{
}

void ApiAsyncWebRequest::_dispatch(int code) const
{
  auto len = measureJson(outdoc);

  AsyncResponseStream *response = _request->beginResponseStream("application/json", len);
  // response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "no-store");
  response->setCode(code);
  serializeJson(outdoc, *response);
  _request->send(response);
}

void ApiAsyncWebRequest::_sendOptions() const
{
  bool _cors = true;
  uint8_t optsLen = outdoc["options"].size();
  uint8_t pos = 0;

  auto len = measureJson(outdoc);
  AsyncResponseStream *response = _request->beginResponseStream("application/json", len);

  if (_cors)
  {
    response->addHeader("Access-Control-Allow-Origin", "*");
  }

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

    const char *acceptHeader = (_cors) ? "Access-Control-Allow-Methods" : "Accept";

    response->addHeader(acceptHeader, optsBuff);
  }

  response->addHeader("Cache-Control", "no-store");

  response->setCode(200);
  serializeJson(outdoc, *response);
  _request->send(response);
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

class ApiAsyncWebSocket : public ApiRequest
{

public:
  ApiAsyncWebSocket(AsyncWebSocket *ws, AsyncWebSocketClient *client, uint8_t *data, size_t len);
  virtual ~ApiAsyncWebSocket();

protected:
  void _dispatch(int code) const override;
  virtual int parse(const char *urlBase, const char *requestKey) override;

private:
  friend class KoolApi;

  AsyncWebSocket *_ws;
  AsyncWebSocketClient *_client;

  /**
     * @brief Data supplied
     * 
     */
  uint8_t *_data = nullptr;

  /**
     * @brief Length of data supplied
     * 
     */
  size_t _len = 0;
};

ApiAsyncWebSocket::~ApiAsyncWebSocket(){};

ApiAsyncWebSocket::ApiAsyncWebSocket(AsyncWebSocket *ws, AsyncWebSocketClient *client, uint8_t *data, size_t len)
    : _ws(ws),
      _client(client),
      _data(data),
      _len(len)
{
}

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

  // std::unique_ptr<ApiJsonParams> p (new ApiJsonParams(jParse["params"].as<JsonObject>()));
  return 0;
};

#endif
