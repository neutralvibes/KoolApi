#ifndef __KOOLAPIPATH_H__
#define __KOOLAPIPATH_H__

#include "KoolApiBases.h"

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
    UNAUTHORIZED = 401,
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
   * @brief Destroy the Kool Api Path object
   *
   */
  virtual ~KoolApiPath(){};

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

#endif // __KOOLAPIPATH_H__
