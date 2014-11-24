<?php
//params for geo location service
$geoHost = "http://www.telize.com";
$geoPath = "/geoip";

//params for weather service
$weerHost = "http://api.openweathermap.org";
$weerPath = "/data/2.5/weather";
$APIID = "Insert_Unique_OpenWeatherMap_API_ID";

//params for Spark Cloud API - the service
$sparkCloudHost = "https://api.spark.io";
$sparkCloudPath = "/v1/devices/";
//our exposed firmware fn. that takes weather data as hex colour code
$sparkFn = "/led";

//params for Spark Core - the device
$sparkDevice = "Insert_Unique_Spark_Device_ID";
$sparkToken = "Insert_Unique_Access_Token";
?>
