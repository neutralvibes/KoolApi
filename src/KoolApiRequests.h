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


#endif // __KOOLAPIREQUESTS_H__
