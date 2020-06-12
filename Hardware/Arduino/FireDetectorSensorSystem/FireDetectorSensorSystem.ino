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
struct ConnectionData
{
  const char* networkSSID = "";
  const char* networkPassword = "";
  const String serverLink ="https://fluffyunicorn.mybluemix.net/";
};

timeScheduling multiTasking;
ConnectionData wifiData;

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
      readFireSensor();
    }
    if ( (unsigned long)(currentTime - multiTasking.sensorMeshCommLastTime) >= multiTasking.sensorMeshCommTimeSlice)
    {
      //communication with neighbouring sensors here
      sensorMeshReadComm();
    }
     if ( (unsigned long)(currentTime - multiTasking.cloudCommsLastTime) >= multiTasking.cloudCommsTimeSlice)
    {
      //cloud comms thread  
      //TODO: Check why time wait is not working 
      cloudCommunication("StatusUpdate","?sensorValue=262");
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
void getLocationData()
{
  //get current location data
}
void readFireSensor()
{
  //read fire sensor value here
  //for purpose of the prototype , since we do not have any actual fire detector , we will generate random values
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
    cloudCommunication("StatusUpdate","?sensorValue=262");
  }
  else
  {
    Serial.println("invalid command");
  }

}
void cloudCommunication(String apiLink , String parameters)
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
  getReq+=apiLink;
  getReq+=parameters;
  Serial.println(getReq);
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
