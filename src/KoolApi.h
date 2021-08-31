#ifndef __KOOLAPI_H__
#define __KOOLAPI_H__



/*

https://restfulapi.net/resource-naming/

200 OK
    The request has succeeded. The meaning of the success depends on the HTTP method:
        GET: The resource has been fetched and is transmitted in the message body.
        HEAD: The representation headers are included in the response without any message body.
        PUT or POST: The resource describing the result of the action is transmitted in the message body.
        TRACE: The message body contains the request message as received by the server.

201 Created
    The request has succeeded and a new resource has been created as a result. This is typically the response sent after POST requests, or some PUT requests.

202 Accepted
    The request has been received but not yet acted upon. It is noncommittal, since there is no way in HTTP to later send an asynchronous response indicating the outcome of the request. It is intended for cases where another process or server handles the request, or for batch processing.


400 Bad Request
    The server could not understand the request due to invalid syntax.

401 Unauthorized
    Although the HTTP standard specifies "unauthorized", semantically this response means "unauthenticated". That is, the client must authenticate itself to get the requested response.

403 Forbidden
    The client does not have access rights to the content; that is, it is unauthorized, so the server is refusing to give the requested resource. Unlike 401, the client's identity is known to the server.

404 Not Found
    The server can not find the requested resource. In the browser, this means the URL is not recognized. In an API, this can also mean that the endpoint is valid but the resource itself does not exist. Servers may also send this response instead of 403 to hide the existence of a resource from an unauthorized client. This response code is probably the most famous one due to its frequent occurrence on the web.

405 Method Not Allowed
    The request method is known by the server but is not supported by the target resource. For example, an API may forbid DELETE-ing a resource.
*/

#include "KoolApiPath.h"
#include "KoolApiRequests.h"

/**
 * @brief Handles processing of requests
 * 
 */
class KoolApi
{
public:
  /**
   * @brief Construct a KoolApi object
   * 
   * @param urlBase uri to mount api on. Eg "/_api"
   */
  KoolApi(const char *urlBase);

  virtual ~KoolApi();

  /**
   * @brief Returns number of uri endpoints registered
   * 
   * @return const uint8_t 
   */
  const size_t uriCount() const;

  /**
   * @brief Get the Url Base set
   * 
   * @return const char* 
   */
  const char *const getUrlBase() const;

  /**
   * @brief Set the key to determine the uri path.
   *  For when the source of the request may be other than webserver, such as Mqtt, Serial, Websockets etc...
   * 
   * @param key The json key to examine. Default: "$_uri"
   * @return ApiProcessor& 
   */

  KoolApi &setRequestKey(const char *key);

  /**
   * @brief Get the key to determine the uri path.
   *  Used when the source of the request may be other than webserver, such as Mqtt, Serial, Websockets etc...
   */

  const char *const getRequestKey() const;

  /**
   * @brief Set the Uri return json key.
   * 
   * If used will add the uri to the returned object automatically for you.
   * 
   * @param key Name of json key to hold the uri path.
   * @return ApiProcessor& 
   */
  KoolApi &setUriKey(const char *key);

  /**
   * @brief Get the uri Key to be added to returned results.
   * 
   * @return const char* 
   */
  const char *const getUriKey() const;

  /**
   * @brief If set will enable the describer on the uri specified for GET requests.
   * 
   * @param uri eg `__describer__`. Default: nullptr
   * @return KoolApi& 
   */
  KoolApi &setDesriberUri(const char *uri);

  /**
   * @brief Return the uri for the Desriber. 
   * 
   * @return const char* The uri. Returns nullptr if not set.
   */
  const char *const getDesriberUri() const;

  /**
   * @brief Add a uri handler
   * 
   * @param uri uri path. Eg "/puppet"
   * @param handler uri handling class
   */
  void on(const char *uri, KoolApiPath &handler);

  /**
   * @brief Process the request.
   * 
   * If the methods `methodsAccepted` is set, anything other than those methods will respond 405 Not Allowed.
   * 
   * @param request The request object
   * @param methodsAccepted bitwise accepted methods. eg  (API_METHOD_GET | API_METHOD_PUT)
   */
  void process(ApiRequest &request, int methodsAccepted = (int)API_METHOD_ANY) const;

  /**
   * @brief Checks if the url supplied starts with the base url
   * 
   * @param url 
   * @return true 
   * @return false 
   */
  inline bool startsWithUriKey(const char *url) const;

#ifdef _ESPAsyncWebServer_H_

  /**
   * @brief Api filter for Async Webserver
   * 
   * @param r request
   * @return true 
   * @return false 
   */
  bool apiFilter(const AsyncWebServerRequest *r);

  /**
   * @brief Register api with AsyncWebserver instance
   * 
   * @param server 
   * @param logfunc 
   */
  void registerWith(AsyncWebServer &server, void (*logfunc)(AsyncWebServerRequest *request, const char *msg) = nullptr);

#endif

protected:
  /**
   * @brief request json key name to map to api path
   * 
   */
  const char *_requestKey = "$_uri";

  /** 
   * Url base for webserver requests
   */
  const char *_urlBase = nullptr;

  /**
   * @brief Key name to populate with api path that responded to request.
   * 
   * If not specified will not be created.
   * 
   */
  const char *_uriKey = nullptr;

  /**
   * Handler class list
   */
  koolutils::LinkedList<KoolApiPath *> _handlerList;

  /**
   * @brief If set will enable the describer on the uri specified
   * 
   */
  const char *_describerUri = nullptr;

  /**
   * @brief Returns the handler for the path specified
   * 
   * @param path Path to search for
   * @return ApiPath* || nullptr if not found
   */
  KoolApiPath *_findHandler(const char *path) const;

  /**
   * @brief Adds options for describing the api
   * 
   * @param out 
   */
  void _describeApi(JsonObject &out) const;

private:
};


KoolApi::KoolApi(const char *urlBase) : _urlBase(urlBase), _handlerList(koolutils::LinkedList<KoolApiPath *>([](KoolApiPath *p) {}))
{
}

KoolApi::~KoolApi() 
{ 
}

const size_t KoolApi::uriCount() const
{
  return _handlerList.length();
}

const char *const KoolApi::getUrlBase() const
{
  return _urlBase;
}

KoolApi &KoolApi::setRequestKey(const char *key)
{
  _requestKey = key;
  return *this;
}

const char *const KoolApi::getRequestKey() const
{
  return _requestKey;
}

const char *const KoolApi::getUriKey() const
{
  return _uriKey;
}

KoolApi &KoolApi::setDesriberUri(const char *uri)
{
  _describerUri = uri;
  return *this;
}

const char *const KoolApi::getDesriberUri() const
{
  return _describerUri;
}

void KoolApi::on(const char *uri, KoolApiPath &handler)
{
  handler._path = uri;
  _handlerList.add(&handler);
}

void KoolApi::process(ApiRequest &request, int methodsAccepted) const
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

KoolApiPath *KoolApi::_findHandler(const char *path) const
{
  if (path)
  {

    for (auto handler : _handlerList)
    {
      if (strncmp(handler->_path, path, strlen(path)) == 0)
      {
        return handler;
      }
    }
  }

  return nullptr;
}

void KoolApi::_describeApi(JsonObject &out) const
{
  auto h = out.createNestedArray("handlers");

  for (auto handler : _handlerList)
  {
    JsonObject p = h.createNestedObject();
    p["path"] = handler->_path;
    handler->_createOptions(p, false);
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
              process(apiRequest);
            })
      .setFilter(std::bind(&KoolApi::apiFilter, this, std::placeholders::_1));

  server.on(
            _urlBase,
            HTTP_ANY, [](AsyncWebServerRequest *request)
            {
              if (request->method() == HTTP_OPTIONS)
              {
                auto resp = request->beginResponse(200);
                resp->addHeader("Access-Control-Allow-Methods", "*");
                request->send(resp);
              }
            },
            NULL, [this, logfunc](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
              if (logfunc)
                logfunc(request, "");

              ApiAsyncWebRequest apiRequest(request, data, len);
              process(apiRequest);
            })
      .setFilter(std::bind(&KoolApi::apiFilter, this, std::placeholders::_1));
}

#endif

#endif // __KOOLAPI_H__
