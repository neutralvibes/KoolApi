
#include "KoolApiRequests.h"

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
