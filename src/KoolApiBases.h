#ifndef __KOOLAPIBASES_H__
#define __KOOLAPIBASES_H__

#include "KoolApiDocuments.h"
#include "KoolUtils.h"

class KoolApi;
class KoolApiPath;

typedef enum
{
  API_METHOD_GET = 0b00000001,
  API_METHOD_POST = 0b00000010,
  API_METHOD_DELETE = 0b00000100,
  API_METHOD_PUT = 0b00001000,
  API_METHOD_PATCH = 0b00010000,
  API_METHOD_OPTIONS = 0b01000000,
  API_METHOD_ANY = 0b01111111,

  API_METHOD_UNKNOWN = -1
} api_method_t;

template <class T, uint8_t S>
class KoolApiTextMapper
{

public:
  T code[S];
  const char *text[S];

  const uint8_t length() const { return S; }

  /**
   * @brief Get the code that matches the supplied char*
   *
   * @param findText text to match
   * @param failCode Code to return on failure.
   * @return T The code matching suppiled findText. failCode if not found.
   */
  T textToCode(const char *findText, const T failCode) const
  {
    if (findText)
    {
      for (uint8_t i = 0; i < S; ++i)
      {
        if (strncmp(findText, this->text[i], strlen(this->text[i])) == 0)
          return code[i];
      }
    }

    return failCode;
  }

  const char *codeToText(int code) const
  {
    for (uint8_t i = 0; i < S; ++i)
    {
      if (this->code[i] == code)
        return text[i];
    }

    return nullptr;
  }

  /**
   * @brief Searchs code for first with bit set
   *
   * @param bit
   * @return const char*
   */
  const char *bitToText(int bit) const
  {
    for (uint8_t i = 0; i < S; ++i)
    {
      if (this->code[i] & bit)
        return text[i];
    }
    return nullptr;
  }

  /**
   * @brief Returns same code if valid. failCode if not.
   *
   * @param code
   * @param failCode
   * @return T
   */
  T isValid(T code, T failCode) const
  {
    for (uint8_t i = 0; i < S; ++i)
    {
      if (this->code[i] == code)
        return code;
    }
    return failCode;
  }
};

// Method map
const KoolApiTextMapper<api_method_t, 6> koolApiMethodMap = {
    {API_METHOD_GET,
     API_METHOD_PUT,
     API_METHOD_POST,
     API_METHOD_PATCH,
     API_METHOD_DELETE,
     API_METHOD_OPTIONS},
    {"GET",
     "PUT",
     "POST",
     "PATCH",
     "DELETE",
     "OPTIONS"}};

// Error status map
const KoolApiTextMapper<int, 6> _statusMap = {
    {400,
     401,
     403,
     404,
     405,
     406},
    {"Bad Request",
     "Unauthorized",
     "Forbidden",
     "Not Found",
     "Method Not Allowed",
     "Not Acceptable"}};
/**
 * @brief Base class for api params
 *
 */
class ApiParamBase
{
public:
  virtual ~ApiParamBase() {}

  virtual const int length() const = 0;
  virtual bool has(const char *name) const = 0;
  virtual String get(const char *name) const = 0;

  friend class KoolApi;
};

/**
 * @brief Base class for api requests
 *
 */
class ApiRequest
{
public:
  api_method_t _method = API_METHOD_UNKNOWN;

  /**
   * @brief The uri of the request
   *
   */
  const char *uri;

  /**
   * @brief The request input JsonObject.
   *
   */
  JsonObject json;

  /**
   * @brief The request params populated by api handlers.
   *
   */
  ApiParamBase *params = nullptr;

  // std::unique_ptr<ApiParamBase> params; - Couldnt get make_un

  /**
   * @brief Destroy the Api Request object
   *
   */
  virtual ~ApiRequest() { if (params) delete params; }

  /**
	 * @brief Send code and response data to request client
	 * If the code is greater than 400 an error response is created and dispatched.
	 *
	 * Use the correct codes whether or not output is to webserver client
	 * @param code HTTP Response code.
	 */
  void send(int code);

protected:
  KOOLAPI_create_IN_doc;
  KOOLAPI_create_OUT_outdoc;

  DeserializationError _deserializationError;
  /**
   * @brief Called by the processor to parse the request
   *
   * @param requestKey
   * @return int error code if any
   */
  virtual int parse(const char *urlBase, const char *requestKey) = 0;

  /**
   * @brief Whether the request has been dispatched
   *
   */
  bool _dispatched = false;

  /**
   * @brief Identifier for the message
   *
   * Maximum is 4294967295
   *
   */
  uint32_t _id = 0;

  /**
   * @brief The request output JsonObject to be populated by api handler.
   *
   */
  JsonObject _out;

  /**
   * @brief Decendants send request to destination
   *
   * @param code
   */
  virtual void _dispatch(int code) const = 0;

  /**
   * @brief Decendants send OPTIONS to destination if supported
   *
   * @param code
   */
  virtual void _sendOptions() const {};

  /**
   * @brief Clears any output add sends an error message with code.
   *
   *
   * @param code
   * @param complete Whether the response should be immediately dispatched. Default true
   */
  void _error(int code, bool complete = true);

  private:
  friend class KoolApiPath;
  friend class KoolApi;
};


class ApiJsonParams : public ApiParamBase
{
  friend class KoolApi;

  JsonObject _params;

public:
  ApiJsonParams(JsonObject params) : _params(params) {}

  virtual ~ApiJsonParams(){}

  const int length() const override { return _params.size(); }

  bool has(const char *name) const override { return _params.containsKey(name); }

  String get(const char *name) const override { return _params[name].as<String>(); }
};

#endif // __KOOLAPIBASES_H__
