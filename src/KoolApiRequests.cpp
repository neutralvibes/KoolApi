
#include "KoolApiRequests.h"

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

ApiCharRequest::~ApiCharRequest() {}

void ApiCharRequest::_dispatch(int code) const
{
  if (_maxLength && !outdoc.isNull()) serializeJson(outdoc, _output, _maxLength);
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

  // Get requests should have no json body
  if (_method != API_METHOD_GET)
    this->json = jParse["body"];

  this->_out = outdoc.to<JsonObject>();

  this->params = new ApiJsonParams(jParse["params"].as<JsonObject>());

  return 0;
}
