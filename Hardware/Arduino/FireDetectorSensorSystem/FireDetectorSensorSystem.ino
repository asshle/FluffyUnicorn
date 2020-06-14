/*
   TODO: write a short description of the program

*/

#include <WiFi.h>
#include <HTTPClient.h>
#include  <ArduinoJson.h>


void initialise();
void wifiInitialise();
void readFireSensor();
void cloudCommunication();
void sensorMeshReadComm();
void sensorMeshWriteComm();
void getLocationData();
void commandEvent();



struct timeScheduling
{
  const unsigned long sensorReadTimeSlice = 50;
  const unsigned long cloudCommsTimeSlice = 60000; // 1 min
  const unsigned long sensorMeshCommTimeSlice = 50;
  unsigned long sensorReadLastTime = 0;
  unsigned long cloudCommsLastTime = 0;
  unsigned long sensorMeshCommLastTime = 0;
  bool threadHalt = false;
  //TODO:Implement priority que system to allow for immediate soft interrupt
};
struct connectionData
{
  const char* networkSSID = "";
  const char* networkPassword = "";
  const String serverLink = "https://fluffyunicornred.mybluemix.net/";
  const String MacAddress = WiFi.macAddress();
};
struct systemHealth
{
  int batteryPercentage;
  int fireSensorReading;
  const int fireSensorPin = 36;
  const int batteryStatusPin = 39 ;
};

struct geoLocation
{
  const String apiToken = "a5ed7ca34461c7";
  const char* Host = "www.unwiredlabs.com";
  String endpoint = "/v2/process.php";
  String jsonString =  "{\n";
  double latitude = 0.0 ;
  double longitude  = 0.0 ;
  double accuracy = 0.0;
};

timeScheduling multiTasking;
connectionData wifiData;
systemHealth systemHealth;
geoLocation geoLocation;

void setup()
{
  // To save overhead power , please try to use minimal code here
  // for clean architecture , use interrupt driven program structures

  initialise();
}

void loop()
{
  unsigned long currentTime = millis();
  if (!multiTasking.threadHalt)
  {
    //Individual threads goes here
    if ( (unsigned long)(currentTime - multiTasking.sensorReadLastTime) >= multiTasking.sensorReadTimeSlice)
    {
      //readingSensor thread
      readSensorsVal();
      multiTasking.sensorReadLastTime = currentTime;
    }
    if ( (unsigned long)(currentTime - multiTasking.sensorMeshCommLastTime) >= multiTasking.sensorMeshCommTimeSlice)
    {
      //communication with neighbouring sensors here
      sensorMeshReadComm();
      multiTasking.sensorMeshCommLastTime = currentTime;
    }
    if ( (unsigned long)(currentTime - multiTasking.cloudCommsLastTime) >= multiTasking.cloudCommsTimeSlice)
    {
      //cloud comms thread
      //will send an update to cloud server every 1 min
      String parameters = "?";
      parameters += "Methods="; parameters += "StatusUpdate" ; parameters += "&";
      parameters += "MacAddress="; parameters += wifiData.MacAddress; parameters ; parameters += "&";
      parameters += "BatteryLevel="; parameters += systemHealth.batteryPercentage; parameters += "&";
      parameters += "FireSensorValues="; parameters += systemHealth.fireSensorReading; parameters += "&";

      cloudCommunication("StatusUpdate", parameters , currentTime);
      multiTasking.cloudCommsLastTime = currentTime;

    }
  }
  else
  {
    // When program is paused
  }
  if (Serial.available() > 0)
  {
    //read incoming commands from either bluetooth or serial command
    commandEvent();
  }

}

//========================Initialisation Methods=========================
void initialise()
{
  //Initialise components here
  Serial.begin(9600); // initialise 9600 for now , if bluetooth is used , then switch to 115200
  wifiInitialise();// connect to metwork
  geoLocationInit();// get location infomations
  startUpResponse(); // send a signal to command center to inform this device is "alive"
}
void wifiInitialise()
{
  //TODO: Write a method to be able to change the wifi ssid and password either through serial or bluetooth
  // For prototyping/Testing phase is ok to use this method , however it is unacceptable for production phase

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Setup done");

  wifiData.networkSSID = "MBS_2.4";
  wifiData.networkPassword = "Password1234";
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(wifiData.networkSSID);
  WiFi.begin( wifiData.networkSSID,  wifiData.networkPassword);

  if (wifiData.networkSSID != "" || wifiData.networkPassword != "") //prevent error
  {
    int wifiRetryCounter = 0 ;
    WiFi.begin(wifiData.networkSSID, wifiData.networkPassword);

    while (WiFi.status() != WL_CONNECTED && wifiRetryCounter < 100)
    {
      delay(1000);
      Serial.println("Connecting to WiFi..");
      wifiRetryCounter++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Connected to the WiFi network");
    }
    else
    {
      Serial.println("Unable to the WiFi network check credentials");
    }

  }
}

void geoLocationInit()
{
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  // WiFi.scanNetworks will return the number of networks found
  if (n == 0 ) {
    Serial.println("No networks available");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
  }
  // now build the jsonString...
  geoLocation.jsonString = "{\n";
  geoLocation.jsonString += "\"token\" : \"";
  geoLocation.jsonString += geoLocation.apiToken;
  geoLocation.jsonString += "\",\n";
  geoLocation.jsonString += "\"id\" : \"saikirandevice01\",\n";
  geoLocation.jsonString += "\"wifi\": [\n";
  for (int j = 0; j < n; ++j)
  {
    geoLocation.jsonString += "{\n";
    geoLocation.jsonString += "\"bssid\" : \"";
    geoLocation.jsonString += (WiFi.BSSIDstr(j));
    geoLocation.jsonString += "\",\n";
    geoLocation.jsonString += "\"signal\": ";
    geoLocation.jsonString += WiFi.RSSI(j);
    geoLocation.jsonString += "\n";
    if (j < n - 1)
    {
      geoLocation.jsonString += "},\n";
    } else
    {
      geoLocation.jsonString += "}\n";
    }
  }
  geoLocation.jsonString += ("]\n");
  geoLocation.jsonString += ("}\n");
  //Serial.println(geoLocation.jsonString);

  WiFiClientSecure client;
  //Connect to the client and make the api call
  Serial.println("Requesting URL: https://" + (String)geoLocation.Host +  geoLocation.endpoint);
  if (client.connect(geoLocation.Host, 443))
  {
    Serial.println("Connected");
    client.println("POST " + geoLocation.endpoint + " HTTP/1.1");
    client.println("Host: " + (String)geoLocation.Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println( geoLocation.jsonString.length());
    client.println();
    client.print( geoLocation.jsonString);
    delay(500);
  }

  //recieving the location informations
  Serial.print("Waiting for response from location server");
  while (geoLocation.latitude == 0.0 && geoLocation.longitude == 0.0)
  {
    String line = client.readStringUntil('\r');
    JsonObject& root = jsonBuffer.parseObject(line);
    Serial.print(".");
    if (root.success())
    {
      geoLocation.latitude = root["lat"];
      geoLocation.longitude = root["lon"];
      geoLocation.accuracy = root["accuracy"];

      Serial.println();
      Serial.print("Latitude = ");
      Serial.println(geoLocation.latitude, 6);
      Serial.print("Longitude = ");
      Serial.println(geoLocation.longitude, 6);
      Serial.print("Accuracy = ");
      Serial.println(geoLocation.accuracy);
    }
  }

  Serial.println("closing connection"); Serial.println();
  client.stop();
}

void startUpResponse()
{
  readSensorsVal();//get sensor reading for the fist time
  
  String parameters = "?";
  parameters += "Methods="; parameters += "DeviceOnline" ; parameters += "&";
  parameters += "MacAddress="; parameters += wifiData.MacAddress; parameters ; parameters += "&";
  parameters += "BatteryLevel="; parameters += systemHealth.batteryPercentage; parameters += "&";
  parameters += "FireSensorValues="; parameters += systemHealth.fireSensorReading; parameters += "&";
  parameters += "DeviceLongitude="; parameters +=  geoLocation.latitude; parameters += "&";
  parameters += "DeviceLatitude="; parameters +=geoLocation.longitude ;
  
  cloudCommunication("PowerOnInit", parameters , millis());
}

//========================Hardware/Board Methods=========================
void readSensorsVal()
{
  //read fire sensor value here

  //======Actual System Implementation=========================
  //systemHealth.fireSensorReading = analogRead(systemHealth.fireSensorPin);
  //systemHealth.batteryPercentage = analogRead(systemHealth.batteryStatusPin);

  //======Prototype/Testing of sensor readings=================
  systemHealth.fireSensorReading = 2048;//random(0, 4095); // since actual reading of analog values will be from 0 - 4095
  systemHealth.batteryPercentage = 1000;//random(0, 1023); // maximum reading of battery

  //systemHealth.batteryPercentage / 204.6 ;//for a 5V battery source , 1023/5 =204.6 (converting adc input to actual voltage)

  //for purpose of the prototype , since we do not have any actual fire detector , we will generate random values
  systemHealth.fireSensorReading = map( systemHealth.fireSensorReading , 0, 4095, 0, 100); //TODO: change map value to 0-100
  systemHealth.batteryPercentage = map( systemHealth.batteryPercentage , 0, 1023, 0, 100); //TODO: change map value to 0-100

  if (systemHealth.fireSensorReading > 75) //TODO: switch this to software interrupt for more responsive respond
  {
    //trigger response for fire department and update neighbours here

    String parameters = "?";
    parameters += "Methods="; parameters += "FireAlert" ; parameters += "&";
    parameters += "MacAddress="; parameters += wifiData.MacAddress; parameters ; parameters += "&";
    parameters += "FireLocatedLat="; parameters += geoLocation.latitude; parameters += "&";
    parameters += "FireLocatedLong="; parameters +=  geoLocation.longitude; parameters += "&";
    parameters += "FireSensorValues="; parameters += systemHealth.fireSensorReading;

    cloudCommunication("NotifyFire", parameters, millis());
  }
  if (systemHealth.batteryPercentage < 40) //TODO: switch this to software interrupt for more responsive respond
  {
    //trigger response to user to inform them the battery level is low

    String parameters = "?";
    parameters += "Methods="; parameters += "LowBatteryNotice" ; parameters += "&";
    parameters += "MacAddress="; parameters += wifiData.MacAddress; parameters ; parameters += "&";
    parameters += "LowBatteryLevelNotice="; parameters += "LowBattery" ; parameters += "&";
    parameters += "BatteryLevel="; parameters += systemHealth.batteryPercentage;

    cloudCommunication("LowBatteryLevelNotice", parameters, millis());
  }
}


//========================Communications Methods=========================
void commandEvent()
{
  //Serial.println(commands);
  String command = Serial.readString();
  command.trim();
  Serial.println(command);
  if (command == "test" )
  {
    String parameters = "?";
    parameters += "Method="; parameters += "Test" ; parameters += "&";
    parameters += "MacAddress="; parameters += wifiData.MacAddress; parameters ; parameters += "&";
    parameters += "BatteryLevel="; parameters += systemHealth.batteryPercentage; parameters += "&";
    parameters += "FireSensorValues="; parameters += systemHealth.fireSensorReading; parameters += "&";

    cloudCommunication("StatusUpdate", parameters , millis());
  }
  else
  {
    Serial.println("invalid command");
  }

}
void cloudCommunication(String apiLink , String parameters , unsigned long timeStamp)
{
  //establish communications with Node red command center
  /*
     msg.payload = {};
     msg.payload.name = "CentralLX";
     msg.payload.lat = 1.290270;
     msg.payload.lon = 103.851959;
     msg.payload.layer = "SensorData";
     msg.payload.UVLevel = getUVLevel() ;
     msg.payload.Temperature = getTemp();
  */
  String getReq = wifiData.serverLink;
  getReq += apiLink;
  getReq += parameters;
  Serial.print(getReq); Serial.print(" millis at : "); Serial.println(timeStamp);
  if (WiFi.status() == WL_CONNECTED)
  {
    //Check the current connection status
    HTTPClient http;
    http.begin(getReq); //node red get req
    int httpCode = http.GET(); //Make the request

    if (httpCode > 0)
    {
      //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }

    else
    {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }
}
void sensorMeshReadComm()
{
  // Listen for other sensor readings

  // TODO: make use of ESP32 node access point to communicate with other 
}
void sensorMeshWriteComm()
{
  // in the events of "emergency" sent status to other devices
}
