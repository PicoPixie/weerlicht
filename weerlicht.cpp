/*
 * weerlicht/weerlicht.cpp
 * =======================
 * controlling ws2812 LED over https 
 * with Spark Core, geolocation and OpenWeatherMap.
 *
 * Firmware script for Spark Core device.
 *
 */


//Spark libs
#include "HttpClient/HttpClient.h"
#include "neopixel/neopixel.h"

//name the pins that ctrl the strip(s)
#define PIN A5 

HttpClient http;
http_request_t geoRequest;
http_response_t geoResponse;


http_header_t geoHeaders[] = {
      { "Content-Type", "application/x-www-form-urlencoded" },
      { NULL, NULL } //NOTE: Always terminate headers with NULL
};


//the varietal of neopixel(30,60,14* density chain) and where to locate
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN);

//how many Romans.?
int n = strip.numPixels(); 

//our governer so we can ease off on hammering the server
unsigned long lastTime = 0L; 

void setup() {
    //expose Spark function here
    Spark.function("led", ledControl);
    //configure the pin to be output
    pinMode(PIN, OUTPUT);
    //initialize the strip LEDs to be OFF
    strip.begin();
    //flush it clear to start
    strip.show(); 
    //location of weer.php
    geoRequest.hostname = ""; //e.g yourserver.net
    geoRequest.path = ""; //e.g /~you/weer.php
    geoRequest.port = 80; //the webserver port
    //uncomment the following line for debugging via screen(1)
    //Serial.begin(9600);
}

void loop() {
    unsigned long now = millis();
    //every 60 secs
    if (now-lastTime>60000UL) {
        lastTime = now;
        //hit up weer.php
        http.get(geoRequest, geoResponse, geoHeaders);
        //uncomment for debugging
        //Serial.print("Application>\tResponse status: ");
        //Serial.println(geoResponse.status);
        //Serial.print("Application>\tHTTP Response Body: ");
        //Serial.println(geoResponse.body);
    }
}

/*
 * takes a single hex element (0-9 || A-F) 
 * maps this element to corresponding integer,
 * 0-9 returned as they are, A->10..F->15
 */
int getMultiplier(String littleBit) {
    
    int mult = 0; // init. R,G,B val will be 0 SAFE
    
    if(littleBit.toInt() == 0) { // FAILED *possibly* bc A-F present
        //try to standardise it 
        littleBit.toUpperCase();
        //which is it.?
        if(littleBit.compareTo("A") == 0) mult = 10; 
        else if(littleBit.compareTo("B") == 0) mult = 11;
        else if(littleBit.compareTo("C") == 0) mult = 12;
        else if(littleBit.compareTo("D") == 0) mult = 13;
        else if(littleBit.compareTo("E") == 0) mult = 14;
        else if(littleBit.compareTo("F") == 0) mult = 15;
        //got something strange, fail safe, 
        //returning 0 will not switch LED on 
        else return 0; 
    } else {
	//got an integral, just use as is
        mult = littleBit.toInt(); 
    }
    
    return mult;
}

/*
 * takes a 2 digit channel chunk of the hex command string
 * maps to int value 0-255
 */
int hex2rgb(String chanHex) {
    
    int tally, multiplier = 0;
    //take the 2char chanHex string
    //address the [0]th element
    String first = chanHex.substring(0,1);
    multiplier = getMultiplier(first);
    //keep tally
    tally = (multiplier * 16); // m * 16^1
    //now address the [1]st element
    String second = chanHex.substring(1);
    multiplier = getMultiplier(second);
    //update tally
    tally += multiplier; // m * 16^0
    
    //sanitise to range 0-255, this will max out the lights, 
    //look into lights with remaining good eye 
    if(tally > 255) tally = 255;
    
    return tally;
}

/*
 * our exposed function maps to here
 * expects a 6 digit hex string e.g 6600cc
 * POST'd from weer.php / the wider ternet - so sanitise
 */
int ledControl(String command) {
    //perform essential sanity checks on input string HERE!!
    //don't trust foreign-input coming over http
    if(command.length() != 6) return 1; //invalid command length rec'd
    //chunk remainder of command string into 3 pairs, 1 each for R,G,B
    String redChanHex = command.substring(0,2);
    String greenChanHex = command.substring(2,4);
    String blueChanHex = command.substring(4,6);
    
    //attempt to convert HEX => RGB
    int redChan = hex2rgb(redChanHex);
    int greenChan = hex2rgb(greenChanHex);
    int blueChan = hex2rgb(blueChanHex);
    //set strip to 1/4 brightness
    strip.setBrightness(64); 
    int setcolour = strip.Color(redChan, greenChan, blueChan);
    int setoff = strip.Color(0,0,0);
    //command each LED on strip in turn
    for(int i=0; i < n; i++) {
        strip.setPixelColor(i-1,(setoff));
        strip.setPixelColor(i,(setcolour));
        strip.show();
        delay(1000);
    }
    //will the last one to leave turn off the lights
    //switch last LED in chain off. 
    strip.setPixelColor(strip.numPixels()-1,(setoff));
    //flush it out
    strip.show();
    //give Spark clown API happy finish
    return 0;
}
