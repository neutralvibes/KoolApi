
#include "KoolApi.h"

KoolApi::KoolApi(const char *urlBase) : _urlBase(urlBase)
{
}

KoolApi::~KoolApi()
{
  for (int i = 0; i < _handlersLength; i++)
    delete _handlerList[i];
}

const size_t KoolApi::uriCount() const { return _handlersLength; }

const char *const KoolApi::getUrlBase() const { return _urlBase; }

KoolApi &KoolApi::setRequestKey(const char *key)
{
  _requestKey = key;
  return *this;
}

const char *const KoolApi::getRequestKey() const { return _requestKey; }

const char *const KoolApi::getUriKey() const { return _uriKey; }

KoolApi &KoolApi::setDesriberUri(const char *uri)
{
  _describerUri = uri;
  return *this;
}

const char *const KoolApi::getDesriberUri() const { return _describerUri; }

KoolApi &KoolApi::on(const char *uri, KoolApiPath &handler)
{
  handler._path = uri;
  KoolApiPath **replaceArr = new KoolApiPath *[_handlersLength + 1];

  if (!_handlersLength)
  {
    replaceArr[0] = &handler;
  }
  else
  {
    for (int i = 0; i < _handlersLength; i++)
    {
      replaceArr[i] = _handlerList[i];
    }
    replaceArr[_handlersLength] = &handler;
  }

  _handlersLength++;
  _handlerList = replaceArr;

  return *this;
}

void KoolApi::process(ApiRequest &request, int methodsAccepted)
{
  auto errParseCode = request.parse(_urlBase, _requestKey);

  if (errParseCode)
  {
    request._error(errParseCode);

    if (request._deserializationError)
    {
      Serial.printf("Json Error: %s\n", request._deserializationError.c_str());
    }

    return;
  }

  if (!request.uri)
  {
    request._error(400);
    return;
  }

  auto handler = _findHandler(request.uri);

  if (!handler && _describerUri && request._method == API_METHOD_GET && strncmp(request.uri, _describerUri, strlen(_describerUri)) == 0)
  {
    _describeApi(request._out);
    request._dispatch(200);
    return;
  }

  int errMethod = (!handler) ? 404 : (request._method == API_METHOD_UNKNOWN) ? 400
                                 : (!(methodsAccepted & request._method))    ? 405
                                                                             : 0;

  if (errMethod)
  {
    request._error(errMethod);
    return;
  }

  KoolApiPath::handle_t h = {
      .method = (api_method_t)request._method,
      .request = &request,
      .uriKey = _uriKey};

  handler->_handle(h);
}

bool KoolApi::startsWithUriKey(const char *url) const
{
  return strncmp(url, _urlBase, strlen(_urlBase)) == 0;
}

KoolApiPath *KoolApi::_findHandler(const char *path)
{
  if (path)
  {

    for (uint8_t i = 0; i < _handlersLength; ++i)
    {

      if (strcmp(_handlerList[i]->_path, path) == 0)
      {
        return _handlerList[i];
      }
    }
  }

  return nullptr;
}

void KoolApi::_describeApi(JsonObject &out)
{
  auto h = out.createNestedArray("handlers");

  for (uint8_t i = 0; i < _handlersLength; ++i)
  {

    JsonObject p = h.createNestedObject();
    p["path"] = _handlerList[i]->_path;
    _handlerList[i]->_createOptions(p, false);
  }
}

KoolApi &KoolApi::setUriKey(const char *key)
{
  _uriKey = key;
  return *this;
}

#ifdef _ESPAsyncWebServer_H_

bool KoolApi::apiFilter(const AsyncWebServerRequest *r)
{
  auto rs = r->url().c_str();
  return startsWithUriKey(rs);
}

void KoolApi::registerWith(AsyncWebServer &server, void (*logfunc)(AsyncWebServerRequest *request, const char *msg))
{
  server.on(
            _urlBase, HTTP_GET, [this, logfunc](AsyncWebServerRequest *request)
            {
              if (logfunc)
                logfunc(request, "");

              ApiAsyncWebRequest apiRequest(request);
              process(apiRequest); })
      .setFilter(std::bind(&KoolApi::apiFilter, this, std::placeholders::_1));

  server.on(
            _urlBase,
            HTTP_ANY, [this, logfunc](AsyncWebServerRequest *request)
            {
              if (logfunc)
                logfunc(request, "");

              if (request->method() == HTTP_OPTIONS)
              {
                ApiAsyncWebRequest apiRequest(request);
                process(apiRequest);
              } },
            NULL, [this, logfunc](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
              if (logfunc)
                logfunc(request, "");

              ApiAsyncWebRequest apiRequest(request, data, len);
              process(apiRequest); })
      .setFilter(std::bind(&KoolApi::apiFilter, this, std::placeholders::_1));
}

#endif
