weerlicht (weather lights)
==========================

What does it do.?
-----------------

Use a Spark Core to control Adafruit Neopixels/strip WS2812 RGB LEDs via OpenWeatherMap Data.

Uses Telize.com for (Gateway IP) location.

The script 

	weer.php 

provides lat/long coords of Spark Core to OpenWeatherMap API.

JSON data we get back is parsed for weather condition code.

Weather condition code determines 6-digit Hex colour code (e.g. ffffff) sent to Spark Core (via HTTP POST).

Exposed Spark function parses incoming Hex string, converts to RGB value, drives the LED strip.

Usage notes
-----------

You will need to supply 

	config.php 

with the following

	Your OpenWeatherMap API ID 
		(apply for yours at OpenWeatherMap.com)
	
	Your unique Spark Core device ID 
		(interrogate through the Spark Build Web IDE)

	Your unique Spark access token 
		(as above)

Future expansion
----------------

Original project 'enclosure' is a clear PVC dome umbrella.

Add tilt sensor or accelerometer to determine when umbrella is in 'deployed' mode and switch from weather forecast to animation mode.

Wrote Spark function to accept six digit hex string as allows future expansion to receiving hex codes over Twitter whilst in 'deployed' mode.
