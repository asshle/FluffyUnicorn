/*
   TODO: write a short description of the program

*/

#include <WiFi.h>
#include <HTTPClient.h>


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
  const String serverLink = "https://fluffyunicorn.mybluemix.net/";
  const String MacAddress = WiFi.macAddress();
};
struct systemHealth
{
  int batteryPercentage;
  int fireSensorReading;
  const int fireSensorPin = 36;
  const int batteryStatusPin = 39 ;
  long latituide , longituide;

};

timeScheduling multiTasking;
connectionData wifiData;
systemHealth systemHealth;

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
      String parameters = "?sensorValue=262&";
      parameters += wifiData.MacAddress; 
      cloudCommunication("StatusUpdate",parameters , currentTime);
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
  wifiInitialise();
  //TODO: initialise incoming pin for "simulated" sensor
  //TODO: Get lat long data here

}
void wifiInitialise()
{
  //TODO: Write a method to be able to change the wifi ssid and password either through serial or bluetooth
  // For prototyping/Testing phase is ok to use this method , however it is unacceptable for production phase
  wifiData.networkSSID = "MBS_2.4";
  wifiData.networkPassword = "Password1234";
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

//========================Hardware/Board Methods=========================


/*

   struct systemHealth
  {
     int batteryPercentage;
     int fireSensorReading;
     long latituide , longituide;
     int fireSensorPin = A0;
     int batteryStatusPin = A1;
  }

*/
void getLocationData()
{
  //get current location data

}
void readSensorsVal()
{
  //read fire sensor value here

  //======Actual System Implementation=========================
  //systemHealth.fireSensorReading = analogRead(systemHealth.fireSensorPin);
  //systemHealth.batteryPercentage = analogRead(systemHealth.batteryStatusPin);

  //======Prototype/Testing of sensor readings=================
  systemHealth.fireSensorReading = random(0, 4095); // since actual reading of analog values will be from 0 - 4095
  systemHealth.batteryPercentage = random(0, 1023); // maximum reading of battery

  //for purpose of the prototype , since we do not have any actual fire detector , we will generate random values
  systemHealth.fireSensorReading = map( systemHealth.fireSensorReading , 0, 4095, 0, 100);
  systemHealth.batteryPercentage = systemHealth.batteryPercentage / 204.6 ;//for a 5V battery source , 1023/5 =204.6

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
    cloudCommunication("test", "?testData = test", millis());
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
}
void sensorMeshWriteComm()
{
  // in the events of "emergency" sent status to other devices
}
