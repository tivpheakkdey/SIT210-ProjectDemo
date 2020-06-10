// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

char mqttUserName[] = "TSPhotonMQTTDemo";    // Use any name.
char mqttPass[] = "UDJN8388MOOFBNEG";        // Change this to your MQTT API key from Account > MyProfile.
char writeAPIKey[] = "RVA07EO5CVNXQP98";     // Change this to your channel write API key.
long channelID = 1078314;                    // Change this to your channel number.
char server[] = "mqtt.thingspeak.com";       // Define the ThingSpeak MQTT broker.
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                // This variable is used for random generation of the client ID.

// Define a callback function to initialize the MQTT client.
void callback(char* topic, byte* payload, unsigned int length) {
 }

MQTT client(server, 1883, callback);       // Initialize the MQTT client.


unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 20L * 1000L; // Post data every 20 seconds.

String weight = "1000";                          // Initialize initial weight to 1000g
String order = "-2";                              // Initialize initial weight to 1000g

void setup() {
    //Register the cloud function
    Particle.function("currentWeight", currentWeight);
    Particle.function("orderIn", orderIn);
    Particle.function("orderOut", orderOut);
}

void loop() {
    // If MQTT client is not connected then reconnect.
    if (!client.isConnected())
    {
      reconnect();
    }
    
    client.loop();  // Call the loop continuously to establish connection to the server.
    
    if (millis() - lastConnectionTime > postingInterval)
    {
        mqttpublish();
    }
    
}

void reconnect(){
    
     char clientID[9];
     
     Serial.println("Attempting MQTT connection");
        // Generate ClientID
        for (int i = 0; i < 8; i++) {
            clientID[i] = alphanum[random(51)];
        }
        clientID[8]='\0';
        
        // Connect to the ThingSpeak MQTT broker.
        if (client.connect(clientID,mqttUserName,mqttPass))  {
            Particle.publish("Conn:"+ String(server) + " cl: " + String(clientID)+ " Uname:" + String(mqttUserName));
        } else
        {
            Particle.publish("Failed to connect. Trying to reconnect in 5 seconds");
            delay(5000);
        } 
}

void mqttpublish() {
    
    // Create a data and topic string to send data to ThingSpeak.
    String data = String("field1=" + String(weight) + "&field2=" + String(order));
    String topic = String("channels/"+String(channelID)+ "/publish/"+String(writeAPIKey));
    
    client.publish(topic,data);
    
    lastConnectionTime = millis();
}

int currentWeight(String input) 
{
    weight = input;
    
    return 1;
}

int orderIn(String input) 
{
    order = "-1";
    
    return 1;
}

int orderOut(String input) 
{
    order = "-2";
    
    return 1;
}