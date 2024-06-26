# KoolApi (ESP8266/ESP32)

Version to hard code number of endpoints without using a linked list.
Testing only

## What is it?


A relatively easy to use json API library utilising `ArduinoJson` for handling JSON  requests from EspAsyncWebserver and other sources for the ESP8266 & ESP32 devices.


It is recommended you take a few minutes to read through this information before using it.

> In order to use this library knowledge of working with [ArduinoJson v6](https://arduinojson.org/) is required.

### Create one REST like API usable from multiple sources

* AsyncWebServer
* AsyncWebSocket
* Any char array data provider

### Why did I bother?

I wanted separation of website design and API, with support for usage via other request sources  such as Mqtt.

#### A bit much?

Perhaps this is a case of over engineering, I don't know, but it helps me to rapidly develop both simple and complex json based API interfaces for the ESP8266 & ESP32 devices, maybe it will help you too.

### Dependencies

* [ArduinoJson v6](https://arduinojson.org/v6/doc/installation/) - For encoding/decoding json data
* [EspAsyncWebserver](https://github.com/me-no-dev/ESPAsyncWebServer) - Only if Webserver/Sockets are used.

## About endpoint Handlers

* Each is grouped via a class inheriting from `KoolApiPath`
* Can restrict char array available methods
* Can automatically return request uri in response

* Automatically generated `405 Method Not Allowed` for methods not defined in endpoint classes or excluded for a char array data sources

* Supplied with:-
  * Url params in `request->params`
  * Parsed JSON body in `request->json`
  * A `JsonObject` output to return data

### Processing

Input data is automatically parsed via `ArduinoJson` and passed to request handlers. Incorrectly formed data is responded to with a `400 Bad Request` error for you.

When a request is processed it is passed to the relevant endpoint class handler. The handler is passed an `ApiRequest` containing the JSON body and any url params. A `JsonObject` output object is additionally supplied to the handler, which can be optionally used to return data to the client.

### Methods available

* GET    - void get(...)
* PUT    - void put(...)
* PATCH  - void patch(...)
* POST   - void post(...)
* DELETE - void del(...)

### Response status codes

Response codes should be sent before you leave a handler. The response codes available are below.

```c++
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  BAD_REQUEST = 400,
  UNAUTHROIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  NOT_ALLOWED = 405
```

Response code example

```c++
class HelloApiPath : public KoolApiPath
{
  void post(ApiRequest *request, JsonObject out)
  {
    // create something...

    // send respose code before leaving
    request->send(OK);
  }
};
```

### Response code errors

Response codes of 400 and over are returned to the client in json format eg `{"error": 400, "message": "Bad Request"}`

### Output

The `send` method of the `ApiRequest` is used to dispatch the response code as well as any optional result data, and should always be called.

All output data is placed in a JSON object as there is no reason not to supply even a basic descriptive key even if just returning an array IMO.

For example

```json
// Returned data as array example
{"people": ["John", "Peter", "Karen"]}
```

#### Adding uri in all returned data

If you want the uri returned in every request.

```c++
koolApi.setUriKey("_uri");
```

This will add the uri of the response into key `_uri` and place output data in the `data` key, as shown below

```json
{
  "_uri": "sys",
  "data": {
    "hostname": "MYHOST",
    "uptime": 32847
  }
}
```

Without that setting the output would be

```json
{ "hostname": "MYHOST", "uptime": 32847 }
```

## JsonDocument sizes

The library by default uses `StaticJsonDocument` to hold JSON data

* `StaticJsonDocument<1096>` for output
* `StaticJsonDocument<800>` for input

These values affect all JSON parsing. To override them define `KOOLAPI_MAX_OUT_SIZE` and/or `KOOLAPI_MAX_IN_SIZE` before including the library.

```c++
// Change default JsonDocument sizes example
#define KOOLAPI_MAX_OUT_SIZE 1248
#define KOOLAPI_MAX_IN_SIZE 400

#include "KoolApi"
```

## Usage

### Create an instance

If using the webserver requests starting with `/api` will be sent from the server to KoolApi

```c++
KoolApi koolApi("/api"); // /api is root of api on webserver
```

### Create endpoint handler(s)

```c++

class HelloApiPath : public KoolApiPath
{
  // Handle GET request
  void get(ApiRequest *request, JsonObject out)
  {
    out["info"] = "Hello a GET response";
    request->send(OK);
  }

  // Handle PUT request
  void put(ApiRequest *request, JsonObject out)
  {
    // any parsed data is in request->json
    auto json = request->json;

    String name = json["name"];
    int age = json["age"];

    // Do something with name and age...

    out["info"] = "Hello a PUT response";

    request->send(OK);
  }

  /**
   * Undefined methods POST,PATCH, DELETE will return '405 not allowed'
   */
};

HelloApiPath helloApiPath;
```

### Add endpoint(s) to KoolApi

```c++
koolApi.on("hello", helloApiPath); // Webserver reachable via '/api/hello'
```

### Register with AsyncWebServer instance if using

```c++
koolApi.registerWith(server);
```

### ESPAsyncWebServer example

```c++
#include "ESPAsyncWebServer.h"
#include "KoolApi.h"

const char *hostname = "ESPKOOLAPI";
const char* ssid = "yourssid";
const char* passw = "sidpassword";

AsyncWebServer server(80);

// Create the API and use "/api" as its base url
KoolApi koolApi("/api");

void setupKoolapi();

class HelloApiPath : public KoolApiPath
{
  // Get requests
  void get(ApiRequest *request, JsonObject out)
  {
    out["info"] = "Hello a GET response";
    request->send(OK);
  }
  // put requests
  void put(ApiRequest *request, JsonObject out)
  {
    auto json = request->json;
    const char * name = json["name"];

    if (name) {
      Serial.printf("Name: %s\n", name);
    }

    out["info"] = "Hello a PUT response";
    request->send(OK);
  }
  // post requests
  void post(ApiRequest *request, JsonObject out)
  {
    out["info"] = "Hello a POST response";
    request->send(OK);
  }
  // delete requests
  void del(ApiRequest *request, JsonObject out)
  {
    out["info"] = "Hello a DELETE response";
    request->send(OK);
  }
};

class AnotherApiPath : public KoolApiPath
{
  // You only need to create the methods you need
  void get(ApiRequest *request, JsonObject out)
  {
    out["info"] = "Another GET response";
    request->send(OK);
  }
};

// Create endpoint instances.
HelloApiPath helloApiPath;
AnotherApiPath anotherApiPath;

void setup()
{
  Serial.begin(115200);

#if defined(ESP8266)
  WiFi.hostname(hostname);
#else
  WiFi.setHostname(hostname);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  setupKoolapi();
}

void loop() {}

void setupKoolapi() {
  koolApi.on("hello", helloApiPath);     // '/api/hello'
  koolApi.on("another", anotherApiPath); // '/api/another'
  koolApi.registerWith(server);
  server.begin();
}
```

## Sources other than AsyncWebserver

To use the API from other sources you use a `ApiCharRequest`. The JSON to parse should contain the following keys:-

* requestKey - points to the uri handler, defaults to `$_uri`
* method to call in uppercase - `GET`, `PUT` etc...
* `body` - the JSON data to act ON

The data example *below* will send the request to the registered endpoint `"testuri/id"`, and call the `put` method in the handler class.

```JSON
{ "$_uri": "testuri/id", "method": "PUT", "body": {"id": 5, "name": "Jack Jones"} }
```

### Shorten Keys

To simplify these requests a little, `ApiCharRequest` has a variable `useShortKeys` that if you set to true, will shorten some keys required.

```c++
ApiCharRequest apiCharRequest(jsonIn);
apiCharRequest.useShortKeys = true // turns on short key mode
```

The two requests below accomplish the same task; one with and one without `useShortKeys` enabled. When you switch, only those root keys for requests are valid for that transaction.

```JSON
// Example 1) Normal key mode (default)
{"method":"PATCH","$_uri":"lamp", "body":{"state": 1}}

// Example 2) useShortKeys mode.
{"U":"PATCH|lamp","B":{"state": 1}}
```

As you see in the short key example above, short key mode combines the method & uri keys, with the values separated by a `|`, under a single key called `U`,
while the `body` key has been shortened to `B`. `P` is used for extra parameters.

### Examples

#### Process a request from a char array

```c++
  // jsonIn is your data from Serial/MQTT etc
  ApiCharRequest apiCharRequest(jsonIn);
  koolApi.process(apiCharRequest);
```

#### Process a request from a char array with output

```c++
  // jsonIn is your data from Serial/MQTT etc
  int outMaxLength = 500;
  char output[outMaxLength]; // Make large enough!

  ApiCharRequest apiCharRequest(jsonIn, output, outMaxLength);
  apiCharRequest.useShortKeys = true
  koolApi.process(apiCharRequest);
```

#### Basic Stream/Serial processing

This basic example shows reading a LF terminated string from Serial and returning the response.

```c++
  if (Serial.available())
  {
    const int outMaxLength = 500;
    char output[outMaxLength];

    String input = Serial.readStringUntil('\n');
    {
      ApiCharRequest apiCharRequest(input.c_str(), output, outMaxLength);
      apiCharRequest.useShortKeys = true
      koolApi.process(apiCharRequest);
      Serial.println(output);
    }
  }
```

## Using KoolApi?

If you use KoolApi in your project feel free to let me know.
