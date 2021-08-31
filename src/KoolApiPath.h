#ifndef __KOOLAPIPATH_H__
#define __KOOLAPIPATH_H__

#include "KoolApiBases.h"
#include "KoolApi.h"

/**
 * @brief Class to be inherited by endpoints
 * 
 */
class KoolApiPath
{
public:
  /**
     * @brief Request info.
     * Hold the json and params for a request. 
     */
  ApiRequest *request;

  virtual void get(ApiRequest *request, JsonObject out) { request->send(NOT_ALLOWED); };
  virtual void post(ApiRequest *request, JsonObject out) { request->send(NOT_ALLOWED); };
  virtual void put(ApiRequest *request, JsonObject out) { request->send(NOT_ALLOWED); };
  virtual void patch(ApiRequest *request, JsonObject out) { request->send(NOT_ALLOWED); };
  virtual void del(ApiRequest *request, JsonObject out) { request->send(NOT_ALLOWED); };

  /**
   * @brief Bitwise for describing methods used
   * 
   * @return int 
   */
  virtual int options() { return API_METHOD_UNKNOWN; };

protected:
  enum resp_code_t
  {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    BAD_REQUEST = 400,
    UNAUTHROIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    NOT_ALLOWED = 405
  };

  /**
   * @brief Construct a new Api Path object
   * 
   */
  KoolApiPath(){};

  /**
   * @brief Construct a new Api Path object
   * 
   * @param path Path to trigger
   */
  KoolApiPath(const char *path);

private:
  friend class KoolApi;

  /**
   * @brief Enpoint path
   * 
   */
  const char *_path;

  /**
   * @brief Root object for json output
   * 
   */
  JsonObject _rootJout;

  /**
   * @brief Contains data for handlers
   * 
   */
  struct handle_t
  {
    api_method_t method;
    ApiRequest *request;
    const char *uriKey;
  };

  /**
   * @brief Passes request to handler with approriate data available
   * 
   * @param h Handler information
   */
  void _handle(const handle_t h);

  /**
   * @brief Creates & option element in supplied json object
   * 
   * @param jo 
   * @param bool includeOptions Add OPTIONS to returned list. 
   * @return uint8_t Number of options found.
   */
  uint8_t _createOptions(JsonObject jo, bool includeOptions = true);
};

KoolApiPath::KoolApiPath(const char *path) : _path(path)
{
}

void KoolApiPath::_handle(const handle_t h)
{
  request = h.request;
  _rootJout = h.request->outdoc.to<JsonObject>();

  // If uriKey specified create sub key `data` for response
  if (h.uriKey || request->_id)
  {
    if (h.uriKey)
      _rootJout[h.uriKey] = _path;
    if (request->_id)
      _rootJout["id"] = request->_id;
    request->_out = _rootJout.createNestedObject("data");
  }
  else
  {
    request->_out = _rootJout;
  }

  switch (h.method)
  {
  case API_METHOD_GET:
    get(request, request->_out);
    break;
  case API_METHOD_POST:
    post(request, request->_out);
    break;
  case API_METHOD_PUT:
    put(request, request->_out);
    break;
  case API_METHOD_PATCH:
    patch(request, request->_out);
    break;
  case API_METHOD_DELETE:
    del(request, request->_out);
    break;
  case API_METHOD_OPTIONS:
  {
    request->outdoc.clear();
    auto jo = request->outdoc.to<JsonObject>();

    if (_createOptions(jo))
    {
      request->_sendOptions();
    }
    else
    {
      request->_dispatch(405);
    }
  }
  break;
  default:
    break;
  }
}

uint8_t KoolApiPath::_createOptions(JsonObject jo, bool includeOptions)
{
  auto opts = options();
  int len = __builtin_popcount(opts); // counts bits set
  uint8_t found = 0;

  if (len)
  {
    auto jArr = jo.createNestedArray("options");

    for (uint8_t x = 0; x < koolApiMethodMap.length(); ++x)
    {
      if (koolApiMethodMap.code[x] & opts)
      {
        jArr.add(koolApiMethodMap.text[x]);
        ++found;
      }
    }

    if (includeOptions && found)
      jArr.add(koolApiMethodMap.codeToText(API_METHOD_OPTIONS));
  }
  return found;
}

#endif // __KOOLAPIPATH_H__
