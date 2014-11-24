/*
 * weerlicht/weerlicht.cpp
 * =======================
 * controlling ws2812 RGB LEDs over HTTP(S)
 * with Spark Core, geolocation and OpenWeatherMap.
 *
 * Firmware script for Spark Core device.
 */

// This #include statement was automatically added by the Spark IDE.
#include "HttpClient/HttpClient.h"

// This #include statement was automatically added by the Spark IDE.
#include "neopixel/neopixel.h"

//name the pins that ctrl the LED chains
#define PIN0 D0 
#define PIN1 D1
#define PIN2 D2
#define PIN3 D3
#define PIN4 D4
#define PIN5 D5
#define PIN6 D6
#define PIN7 D7

HttpClient http;
http_request_t geoRequest;
http_response_t geoResponse;


http_header_t geoHeaders[] = {
       { "Content-Type", "application/x-www-form-urlencoded" },
       { NULL, NULL } // NOTE: Always terminate headers with NULL
};

//each separate LED chain, defines LED count on strip, and pin it's attached to
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(14, PIN0);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(14, PIN1);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(14, PIN2);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(14, PIN3);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(14, PIN4);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(14, PIN5);
Adafruit_NeoPixel strip6 = Adafruit_NeoPixel(14, PIN6);
Adafruit_NeoPixel strip7 = Adafruit_NeoPixel(14, PIN7);

//How Many Romans.?
//a loop counter for setting same colour to whole chain
int n = strip0.numPixels(); 

//default to purple on boot, until we've rx'd a 'weather colour'
int lastColour = strip0.Color(80,11,191);

//our rainbow colours for alternate display between weather displays
int setRed = strip0.Color(246,20,20);
int setOrange = strip1.Color(252,76,0);
int setYellow = strip2.Color(249,255,36);
int setGreen = strip3.Color(50,215,8);
int setBlue = strip4.Color(18,110,144);
int setPurple = strip5.Color(68,18,144);
int setMagenta = strip6.Color(215,39,180);
int setPink = strip7.Color(255,20,114);

//time governer to ease off on hammering the weather script
//currently gets fresh weather report every 15mins (4 lookups in 60min incl. initial)
unsigned long lastTime = 0;  

void setup() {

	//expose Spark function called by weather script
	Spark.function("led", ledControl);
	
	//configure the pins to be output
	pinMode(PIN0, OUTPUT);
	pinMode(PIN1, OUTPUT);
	pinMode(PIN2, OUTPUT);
	pinMode(PIN3, OUTPUT);
	pinMode(PIN4, OUTPUT);
	pinMode(PIN5, OUTPUT);
	pinMode(PIN6, OUTPUT);
	pinMode(PIN7, OUTPUT);

        //initialize the chains OFF
	strip0.begin();
	strip1.begin();
	strip2.begin();
	strip3.begin();
	strip4.begin();
	strip5.begin();
	strip6.begin();
	strip7.begin();

	//set chains to 1/8th bright, reduce current draw 
	strip0.setBrightness(32); 
	strip1.setBrightness(32);
	strip2.setBrightness(32);
	strip3.setBrightness(32);
	strip4.setBrightness(32);
	strip5.setBrightness(32);
	strip6.setBrightness(32);
	strip7.setBrightness(32);

	//flush 'em clear to start
	strip0.show();
	strip1.show();
	strip2.show();
	strip3.show();
	strip4.show();
	strip5.show();
	strip6.show();
	strip7.show();

	//this is where we POST greetings
	geoRequest.hostname = "yourserver.net";
	geoRequest.path = "/~you/weerlicht/weer.php";
	geoRequest.port = 80;
	//Serial.begin(9600);
}

void loop() {

	//when we're not displaying the weather, default to rainbow
	//one colour per chain
	for(int i=0; i < n; i++) 
		strip0.setPixelColor(i,(setRed)); 
	for(int i=0; i < n; i++) 
		strip1.setPixelColor(i,(setOrange)); 
	for(int i=0; i < n; i++) 
		strip2.setPixelColor(i,(setYellow)); 
	for(int i=0; i < n; i++) 
		strip3.setPixelColor(i,(setGreen)); 
	for(int i=0; i < n; i++) 
		strip4.setPixelColor(i,(setBlue)); 
	for(int i=0; i < n; i++) 
		strip5.setPixelColor(i,(setPurple)); 
	for(int i=0; i < n; i++) 
		strip6.setPixelColor(i,(setMagenta)); 
	for(int i=0; i < n; i++) 
		strip7.setPixelColor(i,(setPink));  

	strip0.show();
	strip1.show();
	strip2.show();
	strip3.show();
	strip4.show();
	strip5.show();
	strip6.show();
	strip7.show();
	
	//time since chip booted
	unsigned long now = millis();

	//every 15mins go get fresh weather report
	//if first time we need to lookup weather
	//i.e if Spark hasn't been running 15mins yet, do initial lookup
	if (((now - lastTime) > 900000UL) || (lastTime == 0)) {
		//say hello to the server weather script
		http.get(geoRequest, geoResponse, geoHeaders);
		//if we didn't rx a valid server response, force a subsequent lookup
		if (geoResponse.status == 200) lastTime = now;
		//Serial.print("Application>\tResponse status: ");
		//Serial.println(geoResponse.status);
		//Serial.print("Application>\tHTTP Response Body: ");
		//Serial.println(geoResponse.body);
	}

	delay(800);

	for(int i=0; i < n; i++) {
		//creates a one pixel colour 'drip'
		strip0.setPixelColor(i,(lastColour));
		strip1.setPixelColor(i,(lastColour));
		strip2.setPixelColor(i,(lastColour));
		strip3.setPixelColor(i,(lastColour));
		strip4.setPixelColor(i,(lastColour));
		strip5.setPixelColor(i,(lastColour));
		strip6.setPixelColor(i,(lastColour));
		strip7.setPixelColor(i,(lastColour));
		//flush it out
		strip0.show();
		strip1.show();
		strip2.show();
		strip3.show();
		strip4.show();
		strip5.show();
		strip6.show();
		strip7.show();

		delay(800);
	}
}

/* maps a single base 16 element to base 10
 */
int getMultiplier(String chunk) {

	int mult = 0; //if no change, R|G|B val will be 0 SAFE

	if(chunk.toInt() == 0) { // FAILED *possibly* bc A-F present
		// try to standardise it 
		chunk.toUpperCase();
		// which is it.?
		if(chunk.compareTo("A") == 0) 
			mult = 10; 
		else if(chunk.compareTo("B") == 0) 
			mult = 11;
		else if(chunk.compareTo("C") == 0) 
			mult = 12;
		else if(chunk.compareTo("D") == 0) 
			mult = 13;
		else if(chunk.compareTo("E") == 0) 
			mult = 14;
		else if(chunk.compareTo("F") == 0) 
			mult = 15;
		// got something strange, so just break here, returning 0 will not switch LED on 
		else 
			return 0; 
	} else {
		mult = chunk.toInt(); // got an integral, just use asis
	}

	return mult;
}

/* takes 2 digit chunk of hex command rx'd
 * maps to int val 0-255 for channel (R|G|B)
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

	//sanitise to range 0-255 
	if(tally > 255) 
		tally = 255;

	return tally;
}

/* our exposed fn. maps to here
 * expects a 6 digit hex string e.g 6600cc
 * POST'd from weer.php / the wider 'ternet - so sanitise
 */
int ledControl(String command) {

	//bloody foreign-input coming over http, claiming fn.params, dont trust 'em!
	if(command.length() != 6) 
		return 1; // invalid command rx'd
	// chunk remainder of command string into 3 pairs, 1 each for R,G,B
	String redChanHex = command.substring(0,2);
	String greenChanHex = command.substring(2,4);
	String blueChanHex = command.substring(4,6);
	// Attempt to convert HEX => RGB
	int redChan = hex2rgb(redChanHex);
	int greenChan = hex2rgb(greenChanHex);
	int blueChan = hex2rgb(blueChanHex);
	
	//overwrite our default colour
	int setColour = strip0.Color(redChan, greenChan, blueChan);
	lastColour = setColour;
	
	// spark clown api is expecting an int happy ending
	return 0;
}
