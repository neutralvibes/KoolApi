#ifndef __KOOAPIREQUESTSASYNCWEBSER_H__
#define __KOOAPIREQUESTSASYNCWEBSER_H__

#include "KoolApiBases.h"

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
  {}

  virtual ~ApiAsyncParams(){};

  const int length() const override { return _request->params(); }

  bool has(const char *name) const override { return _request->hasParam(name, _isPost, _isFile); }

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

class ApiAsyncWebSocket : public ApiRequest
{

public:
  ApiAsyncWebSocket(AsyncWebSocket *ws, AsyncWebSocketClient *client, uint8_t *data, size_t len);
  virtual ~ApiAsyncWebSocket() {};

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

#endif
#endif // __KOOAPIREQUESTSASYNCWEBSER_H__
