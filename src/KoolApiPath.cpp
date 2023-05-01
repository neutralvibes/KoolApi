#include "KoolApiPath.h"

KoolApiPath::KoolApiPath(const char *path) : _path(path){}

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
