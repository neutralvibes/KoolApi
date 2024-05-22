
#include "KoolApiRequests.h"


void ApiCharRequest::_dispatch(int code) const
{
  if (_maxLength && !outdoc.isNull()) serializeJson(outdoc, _output, _maxLength);
}

int ApiCharRequest::parse(const char *urlBase, const char *requestKey)
{
  const char * methodTxt = "method";
  const char * bodyTxt = "body";
  const char * paramsTxt = "params";

  if (_isConst) {
    _deserializationError = deserializeJson(doc, _jsonInConst);
  }
  else
  {
    _deserializationError = (_maxInLength) ? deserializeJson(doc, _jsonIn, _maxInLength) :
                                        deserializeJson(doc, _jsonIn);
  }

  if (_deserializationError || doc.isNull() || !doc.is<JsonObject>())
  {
    return 400;
  }

  JsonObject jParse = doc.as<JsonObject>();

  this->_id = jParse["id"];

  // Use brief json keys if in that mode
  if (this->useShortKeys) {
    bodyTxt = "B";
    paramsTxt = "P";
    const char* surl = jParse["U"];
    // const char *methodStr = jParse[methodTxt];
    this->_method = koolApiMethodMap.textToCode(surl, API_METHOD_UNKNOWN);
    char * urlPos = strchr(surl, '|');

    if (urlPos && (int) strlen(surl) > surl - urlPos) {

      this->uri = urlPos + 1;
    }
  }
  else
  {
    this->uri = jParse[requestKey];
    const char *methodStr = jParse[methodTxt];
    this->_method = koolApiMethodMap.textToCode(methodStr, API_METHOD_UNKNOWN);
  }


  // Get requests should have no json body
  if (_method != API_METHOD_GET)
    this->json = jParse[bodyTxt];

  this->_out = outdoc.to<JsonObject>();

  this->params = new ApiJsonParams(jParse[paramsTxt].as<JsonObject>());

  return 0;
}

