#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include "script.h"
#include "style.h"
#define NUM_LEDS 120
#define D5 14
CRGB leds[NUM_LEDS];
byte favorites[100][3];
int ind;
byte red = 255;
byte green = 255;
byte blue = 255;
int brightness = 16;
bool isConnected;
ESP8266WiFiMulti wifiMulti;
bool runRainbow = false;

ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)

void setup() {
  ind = 0;

  FastLED.addLeds<WS2811, D5, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);

  Serial.begin(115200);  
  
  wifiMulti.addAP("Buddy", "0889537309");

  while (wifiMulti.run() != WL_CONNECTED) { //Wait for connection
    delay(500);
    
    Serial.print("Waiting to connect… ");
    Serial.println((millis() / 1000));
    
    if ((millis() / 1000) >= 30) {
      isConnected = false;
      break;
    }
    else {
      isConnected = true;
    }
  }

  if( isConnected ) {
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP to access the server
  }
  else {
    Serial.println("Starting AP mode...");
    startAP();
  }

  server.on("/", handleRoot);
  server.on("/addToFavorite", handleFavoriteColor);
  server.on("/changeBrightness", handleBrightness);
  server.on("/changeColor", handleQuery);
  server.on("/rainbow", handleRainbow);

  server.begin();                                       //Start the server
  Serial.println("Server listening");
  setAll(red, green, blue);
  setBrightness(brightness);

}

void loop() {
  server.handleClient();    //Handling of incoming requests
  if (runRainbow) {
    rainbow();
  }
}

void handleRoot() {

  String page_content = generateContent();

  server.send(200, "text/html", page_content);
}

void handleQuery() {
  red = server.arg("red").toInt();
  green = server.arg("green").toInt();
  blue = server.arg("blue").toInt();

  //setAll(red, green, blue);
  //fadeInFadeOut();
  setEach();
  runRainbow = false;

  server.send(200, "text/plain", "");
}

void handleBrightness() {
  graduallyChangeBrightness();
  runRainbow = false;

  server.send(200, "text/plain", "");
}

void handleFavoriteColor() {
  red = server.arg("red").toInt();
  green = server.arg("green").toInt();
  blue = server.arg("blue").toInt();
  runRainbow = false;

  addFavoriteColor(red, green, blue);

  server.send(200, "text/plain", "");
}

void handleRainbow() {
  server.send(200, "text/plain", "");
  rainbow();

}

void setPixel(int index, byte red, byte green, byte blue) {
  leds[index].r = red;
  leds[index].g = green;
  leds[index].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
  }
  FastLED.show();
}

void fadeInFadeOut() {
  int b = brightness;

  for (int i = 0; i <= b - 10; i++) {
    brightness -= 1;
    setBrightness(brightness);
  }

  setAll(red, green, blue);

  for (int i = 0; i <= b - 10; i++) {
    brightness += 1;
    setBrightness(brightness);
  }
}

void setEach(){
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    FastLED.delay(5);
    FastLED.show();
  }
}

void setBrightness(int brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(10);
}

void graduallyChangeBrightness() {
  int br = brightness;
  int newBrightness = server.arg("brightness").toInt() / 4; 

  Serial.print(br);
  Serial.print(" - ");
  Serial.print(newBrightness);

  if (newBrightness == 0) {
    brightness = 0;
    setBrightness(brightness);  
  }
  
  else if (newBrightness > br) {
    for (int i = 0; brightness <= newBrightness; i++) {
      brightness += 1;
      FastLED.delay(1);
      setBrightness(brightness); 
    }  
  }
  else {
    for (int i = 0; brightness >= newBrightness; i--) {
      brightness -= 1;
      FastLED.delay(1);
      setBrightness(brightness); 
    }
  }
  server.send(200, "text/plain", "");
}

String generateContent() {
  String page_content = "<!DOCTYPE html><html><head><title>Change strip color</title><style type='text/css'>";
  page_content += STYLE;
  page_content += "</style><script src='https://code.jquery.com/jquery-3.4.1.min.js' integrity='sha256-CSXorXvZcTkaix6Yvo6HppcZGetbYMGWSFlBw8HfCJo=' crossorigin='anonymous'></script><script type='text/javascript'>";
  page_content += SCRIPT;
  page_content += "</script></head><body><div class='container'><form method='POST' name='rgb_values'>";
  page_content += createField("range", "red", "color", red);
  page_content += createField("range", "green", "color", green);
  page_content += createField("range", "blue", "color", blue);
  page_content += createField("range", "brightness", "brightness", brightness);
  page_content += "</form><span id='r' class='values'>R: </span><span id='g' class='values'>G: </span><span id='b' class='values'>B: </span><span id='brightness' class='values'>Brightness : </span><br><div id='color'></div><div><button class='random'>Random</button> <button class='addFave'>Add to favorite</button></div></div>";
  page_content += "<div class='favorites'><p>Favorite colors:</p>";
  
    for (int i = 1; i <= ind; i++)
    {
      int red = favorites[i][0];
      int green = favorites[i][1];
      int blue = favorites[i][2];

      String button = generateButton(i, red, green, blue);

      page_content += button;
    }

  page_content += "</div></body></html>";
  return page_content;

}

String createField(String type, String name, String myClass, int value) {
  if ( type == "range") {
    String output = "<input type='range' name="; output += name; output += " class="; output += myClass ; output += " min='0' max='255' value="; output += value; output += ">"; return output;
  }
}

void startAP() {
  IPAddress local_ip(192,168,0,111);
  IPAddress gateway(192,168,0,111);
  IPAddress subnet(255,255,255,0);
  WiFi.softAP("LedController", "samolevski");
  WiFi.softAPConfig(local_ip, gateway, subnet);
}

void addFavoriteColor(byte red, byte green, byte blue) {

  favorites[ind + 1][0] = red;
  favorites[ind + 1][1] = green;
  favorites[ind + 1][2] = blue;

  ind++;
}

String generateButton(int index, int red, int green, int blue) {
  String output = "<button class='favoriteColor' id='";
  output += index;
  output += "' style='background-color: rgb(";
  output += red;
  output += ", ";
  output += green;
  output += ", ";
  output += blue;
  output += ");'></button>";

  return output;
}

void rainbow() {
  runRainbow = true;
    for (int i = 0; i < 100; i+=1) {
      if(!runRainbow) {
        break; 
      }
      uint8_t hue = beatsin8(i, 0, 255);
      fill_rainbow(leds, NUM_LEDS, hue, 5);
      FastLED.show();
      FastLED.delay(100);
      server.handleClient();
  }
  Serial.println("Finished rainbow effect.");
}