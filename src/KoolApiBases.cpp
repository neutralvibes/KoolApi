#include "KoolApiBases.h"

void ApiRequest::send(int code)
{
  if (_dispatched) return;
  (code < 400) ? _dispatch(code) : _error(code, true);
  _dispatched = true;
}

void ApiRequest::_error(int code, bool complete)
{
  outdoc.clear();
  const char *msg = "message";

  if (code)
  {
    const char *txt = _statusMap.codeToText(code);

    if (_id)
      outdoc["id"] = _id;

    outdoc["error"] = code;
    outdoc[msg] = txt ? txt : "Unspecified condition.";
  }

  if (complete)
    _dispatch(code);
}
