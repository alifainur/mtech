#include <WiFi.h>
#include <AsyncTCP.h>
 
//******Enter your network credentials************
const char* ssid = "Your Network SSID";
const char* password = "Your network Password";
//************************************************
 
//Define Status
const char* PARAM_INPUT_1 = "state";
const char* PARAM_INPUT_2 = "state2";
 
//**********Pin Assignments***********
const int relay1 = 23;
const int switch1 = 4;
const int relay2 = 25;
const int switch2 = 26;
const int Build_in_LED = 2;
//************************************
 
//*********************Variables declaration**********************
int relay_1_status = LOW; // the current status of relay1
int switch_1_status; // the current status of switch1
int last_switch_1_status = LOW; // Last status of switch1
int relay_2_status = LOW; // the current status of relay2
int switch_2_status; // the current status of switch2
int last_switch_2_status = LOW; // Last status of switch2
//****************************************************************
 
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers
 
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
 
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
<title>NodeMCU based Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html {font-family: Arial; display: inline-block; text-align: center;}
h2 {font-size: 3.0rem;}
p {font-size: 3.0rem;}
body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
.switch {position: relative; display: inline-block; width: 120px; height: 68px}
.switch input {display: none}
.slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #F63E36; border-radius: 34px}
.slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
input:checked+.slider {background-color: #3CC33C}
input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
</style>
</head>
<body>
<h2>ESP32 based Web Server</h2>
%BUTTONPLACEHOLDER%
%BUTTONPLACEHOLDER2%
<script>
function toggleCheckbox(element)
{
var xhr = new XMLHttpRequest();
if(element.checked)
{
xhr.open("GET", "/update?state=1", true);
}
else
{
xhr.open("GET", "/update?state=0", true);
}
xhr.send();
}
 
function toggleCheckbox2(element)
{
var xhr2 = new XMLHttpRequest();
if(element.checked)
{
xhr2.open("GET", "/update?state2=1", true);
}
else
{
xhr2.open("GET", "/update?state2=0", true);
}
xhr2.send();
}
 
setInterval(function ( )
{
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function()
{
if (this.readyState == 4 && this.status == 200)
{
var inputChecked;
var outputStateM;
 
if( this.responseText == 1)
{
inputChecked = false;
outputStateM = "OFF";
}
else
{
inputChecked = true;
outputStateM = "ON";
}
document.getElementById("output").checked = inputChecked;
document.getElementById("outputState").innerHTML = outputStateM;
}
}
xhttp.open("GET", "/state", true);
xhttp.send();
 
var xhttp2 = new XMLHttpRequest();
xhttp2.onreadystatechange = function()
{
if (this.readyState == 4 && this.status == 200)
{
var inputChecked2;
var outputStateM2;
 
if( this.responseText == 1)
{
inputChecked2 = false;
outputStateM2 = "OFF";
}
else
{
inputChecked2 = true;
outputStateM2 = "ON";
}
document.getElementById("output2").checked = inputChecked2;
document.getElementById("outputState2").innerHTML = outputStateM2;
}
};
xhttp2.open("GET", "/state2", true);
xhttp2.send();
 
}, 1000 ) ;
</script>
</body>
</html>
)rawliteral";
 
// Replaces placeholder with button section in your web page
String processor(const String& var)
{
//Serial.println(var);
if(var == "BUTTONPLACEHOLDER")
{
String buttons1 ="";
String outputStateValue = outputState();
buttons1+= "<h4>Device 1 - Status <span id=\"outputState\"><span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
return buttons1;
}
 
if(var == "BUTTONPLACEHOLDER2")
{
String buttons2 ="";
String outputStateValue2 = outputState2();
buttons2+= "<h4>Device 2 - Status <span id=\"outputState2\"><span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox2(this)\" id=\"output2\" " + outputStateValue2 + "><span class=\"slider\"></span></label>";
return buttons2;
}
return String();
}
 
String outputState()
{
if(digitalRead(relay1))
{
return "checked";
}
else
{
return "";
}
return "";
}
String outputState2()
{
if(digitalRead(relay2))
{
return "checked";
}
else
{
return "";
}
return "";
}
 
void setup()
{
// Serial port for debugging purposes
Serial.begin(115200);
 
pinMode(relay1, OUTPUT);
digitalWrite(relay1, HIGH);
pinMode(switch1, INPUT);
 
pinMode(relay2, OUTPUT);
digitalWrite(relay2, HIGH);
pinMode(switch2, INPUT);

pinMode(Build_in_LED, OUTPUT);
digitalWrite(Build_in_LED, HIGH);
 
// Connect to Wi-Fi
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(1000);
Serial.println("Connecting to WiFi..");
}
digitalWrite(Build_in_LED,LOW); // Indicator connected to WiFi LED

// Print ESP Local IP Address
Serial.println(WiFi.localIP());
 
// Route for root / web page
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
{
request->send_P(200, "text/html", index_html, processor);
});
 
// Send a GET request
server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request)
{
String inputMessage;
String inputParam;
 
// GET input1 value on <ESP_IP>/update?state=<inputMessage>
if (request->hasParam(PARAM_INPUT_1))
{
inputMessage = request->getParam(PARAM_INPUT_1)->value();
inputParam = PARAM_INPUT_1;
digitalWrite(relay1, inputMessage.toInt());
relay_1_status = !relay_1_status;
}
else
{
inputMessage = "No message sent";
inputParam = "none";
}
Serial.println(inputMessage);
request->send(200, "text/plain", "OK");
 
String inputMessage2;
String inputParam2;
 
if (request->hasParam(PARAM_INPUT_2))
{
inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
inputParam2 = PARAM_INPUT_2;
digitalWrite(relay2, inputMessage2.toInt());
relay_2_status = !relay_2_status;
}
else
{
inputMessage2 = "No message sent";
inputParam2 = "none";
}
Serial.println(inputMessage2);
request->send(200, "text/plain", "OK");
});
 
// Send a GET request to <ESP_IP>/state
server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request)
{
request->send(200, "text/plain", String(digitalRead(relay1)).c_str());
});
 
server.on("/state2", HTTP_GET, [] (AsyncWebServerRequest *request)
{
request->send(200, "text/plain", String(digitalRead(relay2)).c_str());
});
// Start server
server.begin();
}
 
void loop()
{
int reading1 = digitalRead(switch1);
if (reading1 != last_switch_1_status)
{
lastDebounceTime = millis(); // reset the debouncing timer
}
 
if ((millis() - lastDebounceTime) > debounceDelay) 
{ 
if (reading1 != switch_1_status)
{
switch_1_status = reading1;
if (switch_1_status == HIGH)
{
relay_1_status = !relay_1_status;
}
}
}
 
int reading2 = digitalRead(switch2);
if (reading2 != last_switch_2_status)
{
lastDebounceTime = millis();
}
 
if ((millis() - lastDebounceTime) > debounceDelay)
{
if (reading2 != switch_2_status)
{
switch_2_status = reading2;
if (switch_2_status == HIGH)
{
relay_2_status = !relay_2_status;
}
}
}
// set the LED:
digitalWrite(relay1, relay_1_status);
digitalWrite(relay2, relay_2_status);
 
// save the reading. Next time through the loop, it'll be the lastButtonState:
last_switch_1_status = reading1;
last_switch_2_status = reading2;
}