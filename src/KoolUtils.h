#ifndef __KOOLUTILS_H__
#define __KOOLUTILS_H__


#include <stddef.h>
namespace koolutils
{
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

};

#endif // __KOOLUTILS_H__
