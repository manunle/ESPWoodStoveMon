//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>
<tr>
  <td align="right">OTA Password</td>
  <td><input type="text" id="OTApwd" name="OTApwd" value=""></td>
</tr>
<tr>
  <td align="right">MQTT Server</td>
  <td><input type="text" id="MQTTServer" name="MQTTServer" value=""></td>
</tr>
<tr>
  <td align="right">MQTT Port</td>
  <td><input type="text" id="MQTTPort" name="MQTTPort" value=""></td>
</tr>
<tr>
  <td align="right">Toggle Topic</td>
  <td><input type="text" id="ToggleTopic" name="ToggleTopic" value=""></td>
</tr>
<tr>
  <td align="right">Toggle Message</td>
  <td><input type="text" id="ToggleMessage" name="ToggleMessage" value=""></td>
</tr>
<tr>
  <td align="right">Heartbeat Every</td>
  <td><input type="text" id="HeartbeatEvery" name="HeartbeatEvery" value=""></td>
</tr>
<tr>
  <td align="right">Temperature delay</td>
  <td><input type="text" id="TemperatureDelay" name="TemperatureDelay" value=""></td>
</tr>
<tr>
  <td align="right">On Temperature</td>
  <td><input type="text" id="OnTemperature" name="OnTemperature" value=""></td>
</tr>
<tr>
  <td align="right">Off Temperature</td>
  <td><input type="text" id="OffTemperature" name="OffTemperature" value=""></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
  values += "OTApwd|" + (String) config.OTApwd + "|div\n";
  values += "MQTTServer|" + (String) config.MQTTServer + "|div\n";
  values += "MQTTPort|" + (String) config.MQTTPort + "|div\n";
  values += "HeartbeatEvery|" + (String) config.HeartbeatEvery + "|div\n";
  values += "TemperatureDelay|" + (String) config.TemperatureDelay + "|div\n";
  values += "OnTemperature|" + (String) config.OnTemperature + "|div\n";
  values += "OffTemperature|" + (String) config.OffTemperature + "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html()
{
	
	if (server.args() > 0 )  // Save Settings
	{	
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
      if (server.argName(i) == "OTApwd") config.OTApwd = urldecode(server.arg(i));
      if (server.argName(i) == "MQTTServer") config.MQTTServer = urldecode(server.arg(i));
      if (server.argName(i) == "MQTTPort") config.MQTTPort = server.arg(i).toInt();
      if (server.argName(i) == "HeartbeatEvery") config.HeartbeatEvery = server.arg(i).toInt();
      if (server.argName(i) == "TemperatureDelay") config.TemperatureDelay = server.arg(i).toInt();
      if (server.argName(i) == "OnTemperature") config.OnTemperature = server.arg(i).toInt();
      if (server.argName(i) == "OffTemperature") config.OffTemperature = server.arg(i).toInt();
		}
		WriteConfig();
		firstStart = true;
	}
	server.send_P ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
	
	
}

void send_general_configuration_values_html()
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
  values += "OTApwd|" +  (String)  config.OTApwd +  "|input\n";
  values += "MQTTServer|" +  (String)  config.MQTTServer +  "|input\n";
  values += "MQTTPort|" +  (String)  config.MQTTPort +  "|input\n";
  values += "HeartbeatEvery|" + (String) config.HeartbeatEvery + "|input\n";
  values += "TemperatureDelay|" + (String) config.TemperatureDelay + "|input\n";
  values += "OnTemperature|" + (String) config.OnTemperature + "|input\n";
  values += "OffTemperature|" + (String) config.OffTemperature + "|input\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}
