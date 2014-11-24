<?php
//holds all HOST + PATH + KEYs, data for qry building
include "config.php";

//where is the spark core - gets ip of calling gateway
$sparkGW = $_SERVER['REMOTE_ADDR'];
if(isset($_SERVER['HTTP_X_FORWARDED_FOR'])) //they used a correctly-set proxy
	$sparkGW = $_SERVER['HTTP_X_FORWARDED_FOR']; //so get the real caller
//else may just get weather for where proxy server is located 

//try to get lat/long coords for calling ip
$getGeo = $geoHost . $geoPath . "/" . $sparkGW;
if($json = file_get_contents($getGeo)) {

	//pull data to assoc array $geo[]
	$geo = json_decode($json, true);

	//now do the weather lookup for those coords 
	//write the teem toon
	$getWeather = $weerHost . $weerPath; 
	$getWeather .= "?lat=".$geo['latitude']."&lon=".$geo['longitude'];
	$getWeather .= "&units=metric";
	$getWeather .= "&lang=en";
	$getWeather .= "&APIID=".$APIID;

	//sing the teem toon
	if($json = file_get_contents($getWeather)) {

		//pull it out to assoc array
		$weather = json_decode($json, true); 

		//just switch on first digit of 3-digit weather condition code 
		$weatherGroup = str_split($weather['weather'][0]['id']);
		switch($weatherGroup[0]) {
			case '2': $hex = 'ff3300'; break; //200 series:thunder:orange
			case '3': $hex = '009999'; break; //300 series:drizzle:teal
			case '5': $hex = '003399'; break; //500 series:rain:blue
			case '6': $hex = 'ffffff'; break; //600 series:snow:white
			case '7': $hex = 'ffff66'; break; //700 series:atmos:yellow
			case '8': $hex = '00cc00'; break; //800 series:cloud:green
			default: $hex = '6600cc'; //got something unexpected send default:purple
		}

		//build the spark update POST
		$sparkURL = $sparkCloudHost . $sparkCloudPath . $sparkDevice . $sparkFn; 
		$sparkData = array(
			'params' => $hex, //the colour we want
			'access_token' => $sparkToken,
		);
		$sparkResource = array(
			'http' => array(
				'header' => "Content-type: application/x-www-form-urlencoded\r\n",
				'method' => 'POST',
				'content' => http_build_query($sparkData),
			),
		);

		//try to update the LEDs colour
		$context = stream_context_create($sparkResource);
		if($fp = fopen($sparkURL,'r',false,$context)) {
			fpassthru($fp);
			fclose($fp);
		} else
			//failed to POST to exposed Spark fn.
			return;
	} else
		//Something Went Wrong getting weather for those coords
		return;
} else 
	//Something Went Wrong with coords lookup
	return; 
?>
