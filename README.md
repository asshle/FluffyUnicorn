 
# SCDF X IBM Lifesavers Hackathon 

## Project Description
The following solutions consists of prototype implementation of the Residential Smart Fire Alarm , the project 
consist of various different component ranging from feild IOT sensors to cloud technology. 

Totaling at 1168 counts, fires in residential areas make up 40.8% of the total number of fire calls in 2019. From unattended cooking, 
hastily discarded items in common areas to mishaps from charging of E-scooters/PMA, fire causes property damage as well as threat to 
residents living in the area.However, most fires start without warning, and are not responded to until people see smoke and fire -
especially when people are asleep or out of the house. In instances as such, early indication and intervention is crucial in reducing 
damage and casualties.So how do we protect ourselves and our neighborhood?

With our Digital Fire Alarm Device (DFAD), we could detect and respond to the fire within early stages within our vicinity, we will be 
able cut down the extent of damage to properties and casualties. In the case of minor fires, we would be able to activate Community First Responders 
(CFR) to assist in putting out the fire immediately, and if the situation is beyond control, will help to evacuate residents in the immediate region. 
This will set aside valuable SCDF resources/personnels to attend to incidents with a higher priority. 

Our solution to this problem will include the pre-existing Home Fire Alarm Device (HFAD), coupled with field IoT sensors to IBM cloud technology. 
The IoT device, built within the hardware, will act as a field controller for field sensors to detect smoke/fire within the residence, and act as an
communication module between the sensors and the cloud network. Furthermore, these IoT devices will be added into a mesh network of other IoT devices - 
and in the event of a fire, create a inter-module communication to localize the fire. According to the severity of the fire, messages will be dispatched 
through the myResponders app to request CFRs in the vicinity for assistance, meanwhile, notifying residents of the area via emergency alert/notifications 
to evacuate. In the event that the sensors detect higher levels of fire/smoke, the SCDF will be notified to intervene. 
    
## Team Composition

Members of Fluffy Unicorn come from different background and have experience in different feilds. This team'
have worked together in various different project primarily on industrial 4.0 projects. Our team comprises of
people with background in areonautical engineering , computer engineering, control/industrial engineering and 
Biomedical Science


Team Members includes
- Lim Pin
- Kabeta Motoki
- Lerene Tong 
- Lucas Tay

## Problem statement
   Integrating with a smart enviroment

## Pitch Video
‘FluffyUnicorn:Residential Smart Fire Alarm_SCDFXIBM’
<!-- TODO: Insert youtube pitch link in here-->

## Architecture of proposed system
![Residential Smart Fire Alarm System Architecture](https://github.com/asshle/FluffyUnicorn/blob/master/Assets%20Image/SystemArchitecture.PNG?raw=true)

## Getting started
Hardware setup :
```
For prototyping we used an ESP32 dev kit c wroom.
The software was written in arduino C++ 
to allow the code to be able to run on your computer , you will need to have arduino IDE installed on your computer. [Arduino IDE Download](https://www.arduino.cc/en/main/software)

On first install , you will require to add a board library as well as some service library to make the source code usuable on your computer

To add the board:
-On the top left hand panel click on **File > Preferences**
-Under Additional Board Manager URLs add this url link *https://dl.espressif.com/dl/package_esp32_index.json*
-Once done , go to the top left hand corner and select **Tools> Board > Board Managers **  
-In there , search for "**ESP32**" select the one by esprieff systems
-After adding the board libraries , select "**Esp32 Dev Module**"

After that you are ready to use the ESP32 on your IDE for more detailed description click [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

For this project we used 1 other external library 

To add new library in arduino IDE: 
-On the top left hand corner select **Tools > Manage Libraries**
-An window should appear search for "**ArduinoJson**"
-Select the library from "Benoit Blanchon"
-Ensure that the version for the ArduinoJson is 5.0.0 if not the code will not work

After following the above instruction , you should be able to run the program without an issue

```    
Cloud setup :
```
Step 1: https://developer.ibm.com/components/node-red/tutorials/how-to-create-a-node-red-starter-application/
Step 2: Once Node Red is up, stop node red through the application details
Step 3: Go to Git repository and add these few dependencies under package.json
Step 4: Build and Depoly once again on Delivery Pipeline
Step 5: Open Node Red to start working on it!

```

## Description of solution

Hardware Implementations:
```    
-Implementation build
    In this prototype we used ESP32 dev kit c Wroom as our IOT device , the software for the ESP32 was written in C++ Arduino. The embedded system
    code consist of various different function like reading of sensors value , communicating with node red command center , as well as communicating with 
    other IOT device in the vicinity. 

    Within the arduino code , the archietecture for the embded system follows the principle commonly found in operating systems to multitask between different 
    processes. Within the board we managed to program a feature update the the node red command center every minute. This will update the command center on the
    system health and other parameter such as (battery level ,Fire Sensor Value , Timestamp, Device location)
 
- Tests conducted on the system
    Due to the short nature of the hackathon we were unable to conduct any real unit tests on this system , but through out development , we test the functionility
    the code. All the code in the source code works however we were unable to implement the mesh communication in time due to the complexity and the time given of 
    for the hackathon. However given more time , we will utilise technology used in apps like Trace Together where we will use Bluetooth Low Energy (BLE) to communicate
    with nodes in the mesh system. 

- Limitation of the system
    As mentioned in the above statement , we were unable to implement a mesh technology in time due to the complexity of mesh network. Besides the lack of implementation
    of the mesh technology , the system will track device location , with PDPA laws recently being implemented in singapore , tracking device location might become 
    more complicated since we are unable isolate where is the fire occuring. Another limitation include the sensitivity of the sensors , since our team is not introducing a new 
    techology for the fire/smoke sensor , reliability and sensitivity of the fire detection will rely on how accurate and precise current smoke/fire sensor are today.

    Security is also another limitation , since all of our code is in development phase , we did not consider the security the system. So to move forward we may need to 
    intensive vulnerability testing on the system to ensure the reliable and secure use of the system 

- Scalability of system
    Since the hardware uses industry standard IOT device (NodeMCU ESP32) the hardware will be compatible with a scaling system. Since future iterations of this project will 
    include a mesh communications network , device will be able to join and leave the network without impacting the system, thus make it easy to scale. The biggest challenge 
    for scaling the system is that since our device communicates to node red via an API , due to high volume of the data coming into the command center , we would require more 
    computation power to handle the processing of the infomations 

```
Cloud Implementations
```
-Implementation build
    In this project, the wiring tool for IoT devices used is Node Red. Node Red is written in JavaScript. This was an ideal tool as it  is easily able to get readings from ESP32 through various methods, and the dashboard function on Node Red is good in showing visual analytics. 
    In this project, Node Red is used to channel the data from the ESP32 board to the Dashboard, which shows the parameters as stated as above in the hardware implementations. Those data which are considered to be a threat / cause of fire will then be plotted on the mini map on the dashboard, where the user is able to observe.
- Tests conducted on the system
    No real tests were conducted on the system, but throughout the development of the project we were able to justify our ideas behind this project.The Mapping of the Data, as well as constructing of the Table of important parameters were working well with dummy data. If there was more time,
    we could implement databases such as Firebase, to be able to give a further thought analysis about the data we can retrieve from the sensors.
- Limitation of the system
    This is practical for small network prototypes involving to about 20 sensors, but it is not ideal for a large network as Node Red uses visual drag and drop interface and it can become complex and messy structure. However, Node Red is made for rapid IoT prototyping and is suitable for events such as a hackathon. 
- Scalability of system
    As mentioned in the limitations, Node Red is not an ideal solutions for engaging numerous customers which will lead to large data. Node.js as a backend service will be able to cover for the areas of Node Red, but it may take some time to  do a full system for this.
- Live Demo
    Receive Data from ESP32
    Node Red Channels the Data to the respective Dashboard
    On the Dashboard, we are able to visualize the data obtained
    Possible fire threats are then shown on the Map
    SCDF is able to monitor this

```
   
## Project Roadmap / Project Timeline
 <!-- TODO: insert Project Roadmap and Project timeline write up here-->





