#ifndef __KOOLAPIREQUESTS_H__
#define __KOOLAPIREQUESTS_H__

#include "KoolApiBases.h"
#include "KoolApiRequestsAsyncWebserver.h"

/**
 * @brief KoolApi input from a char array source
 *
 */
class ApiCharRequest : public ApiRequest
{
public:

  /**
   * @brief Whether or not to simplify keys "uri", "method" and "body"
   *
   * Translates to: {"U": method+"|"+uri, "B": body, "P": params}
   *
   */
  bool useShortKeys = false;

  /**
   * @brief Process char request without output. Uses less memory
   *
   * @param jsonIn
   */
  ApiCharRequest(char *jsonIn)
      : _jsonIn(jsonIn),
        _maxLength(0),
        _isConst(false)
  {
  }

  /**
   * @brief Process const char request without output. Uses more memory for const char
   *
   * @param jsonIn
   */
  ApiCharRequest(const char *jsonIn)
      : _jsonInConst(jsonIn),
        _maxLength(0),
        _isConst(true)
  {
  }

  /**
   * @brief Process const char request without output
   *
   * @param jsonIn
   * @param maxInLength Length of jsonIn
   */
  ApiCharRequest(const char *jsonIn, size_t maxInLength)
      : _jsonInConst(jsonIn),
        _maxLength(0),
        _maxInLength(maxInLength),
        _isConst(true)
  {
  }


  /**
   * @brief Process char request placing any output in `output`
   *
   * @param jsonIn json
   * @param output output char[]
   * @param maxLength size of output char[]
   */
  ApiCharRequest(char *jsonIn, char *output, size_t maxLength)
      : _jsonIn(jsonIn),
        _output(output),
        _maxLength(maxLength),
        _isConst(false)
  {
  }


  /**
   * @brief Process char request placing any output in `output`
   *
   * @param jsonIn json
   * @param output output char[]
   * @param maxInLength length of jsonIn
   * @param maxLength size of output char[]
   */
  ApiCharRequest(char *jsonIn, char *output, size_t maxInLength, size_t maxLength)
      : _jsonIn(jsonIn),
        _output(output),
        _maxLength(maxLength),
        _maxInLength(maxInLength),
        _isConst(false)
  {
  }


  /**
   * @brief Process const char request placing any output in `output`
   *
   * @param jsonIn json
   * @param output output char[]
   * @param maxLength size of output char[]
   */
  ApiCharRequest(const char *jsonIn, char *output, size_t maxLength)
      : _jsonInConst(jsonIn),
        _output(output),
        _maxLength(maxLength),
        _isConst(true)
  {
  }

  virtual ~ApiCharRequest(){};

protected:
  void _dispatch(int code) const override;
  virtual int parse(const char *urlBase, const char *requestKey) override;

private:
  friend class KoolApi;

  const char *_jsonInConst;
  char *_jsonIn;
  char *_output;
  size_t _maxLength = 0;
  int _maxInLength = 0;
  bool _isConst = false;
};

#endif // __KOOLAPIREQUESTS_H__
