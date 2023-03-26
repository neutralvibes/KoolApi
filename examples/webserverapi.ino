/**
 * This example uses the same API for access
 * via Serial & Webserver at the same time
*/

#include "ESPAsyncWebServer.h"
#include "KoolApi.h"

const char *hostname = "ESPKOOLAPI";
const char *ssid = "YOURSID";
const char *passw = "SSIDPASSWORD";

AsyncWebServer server(80);

// Set the api to use "/api" as its base url
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
    auto json = request->json; // data is here

    const char *name = json["name"];

    if (name)
    {
      Serial.printf("Name: %s\n", name);
    }

    out["info"] = "Hello a PUT response";
    request->send(OK);
  }
  // post requests
  void post(ApiRequest *request, JsonObject out)
  {
    auto json = request->json; // data is here

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

// Create path instances.
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

const int outMaxLength = 500;
char output[outMaxLength];

void loop()
{
  if (Serial.available())
  {
    const int outMaxLength = 500;
    char output[outMaxLength];

    String input = Serial.readStringUntil('\n');
    {
      ApiCharRequest charRequest(input.c_str(), output, outMaxLength);
      koolApi.process(charRequest);
      Serial.println(output);
    }
    
  }
}

void setupKoolapi()
{
  koolApi.setUriKey("_uri");
  koolApi.on("hello", helloApiPath);     // '/api/hello'
  koolApi.on("another", anotherApiPath); // '/api/another'
  koolApi.registerWith(server);
  server.begin();
}
