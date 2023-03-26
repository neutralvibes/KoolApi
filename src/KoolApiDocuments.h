#ifndef __KOOLAPIDOCUMENTS_H__
#define __KOOLAPIDOCUMENTS_H__

#ifdef _WIN32
#include "ArduinoJson-v6.18.0.h"
#else
#include <Arduino.h>
#include "ArduinoJson.h"
#endif

#ifndef KOOLAPI_MAX_OUT_SIZE
#define KOOLAPI_MAX_OUT_SIZE 1096 // Size of json output documents
#endif

#ifndef KOOLAPI_MAX_IN_SIZE
#define KOOLAPI_MAX_IN_SIZE 800 // Size of json input documents
#endif

#ifndef KOOLAPI_create_IN_doc
#define KOOLAPI_create_IN_doc StaticJsonDocument<KOOLAPI_MAX_IN_SIZE> doc;
#endif

#ifndef KOOLAPI_create_OUT_outdoc
#define KOOLAPI_create_OUT_outdoc StaticJsonDocument<KOOLAPI_MAX_OUT_SIZE> outdoc;
#endif

#endif // __KOOLAPIDOCUMENTS_H__
