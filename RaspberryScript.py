# MQTT Client demo
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'

import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import requests

#Initializing variables for MQTT connection with Thingspeak
username = "Pi Reading"             #Username for Thingspeak, any name is fine
password = "UDJN8388MOOFBNEG"       #MQTT API key from Thingspeak
channelID = "1078314"               #Channel ID
read_APIKey = "04QZ7FFQAZAYOFHK"    #Read API key of the channel

#Url for IFTTT WebHook requests
url = "https://maker.ifttt.com/trigger/item_running_low/with/key/NFHSLdUvhtga4S5tnBeh-"

# The callback or when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() - if we lose the connection and
    # reconnect then subscriptions will be renewed.
    
    client.subscribe("channels/"+channelID"/subscribe/fields/+/"read_APIKey)
 
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global sent_notification
    sent = sent_notification
    result = msg.payload
    #print(result)
    result = result.decode("utf-8")
    result = int(result)
    print(result)
    
    
    if(result <= 100 and result >= 0 and sent == False):
        sent_notification = True
        response = requests.post(url)
        GPIO.output(redLed, GPIO.HIGH)
        print("Send notification")
    elif(result > 100 and sent == True):
        sent_notification = False
        GPIO.output(redLed, GPIO.LOW)
        print("Reset notification")
    else:
        print("Pass")
        
    if(result == -1):
        GPIO.output(yellowLed, GPIO.HIGH)
    
    if(result == -2):
        GPIO.output(yellowLed, GPIO.LOW)
        
    
    
    
#Ensure only 1 notification is sent
global sent_notification
sent_notification = False

#Setting pin number
redLed = 11
yellowLed = 13

#Silence warning
#GPIO.setwarnings(False)
#Setting GPIO number standard
GPIO.setmode(GPIO.BOARD)

#Setting pin mode for the connected pin
GPIO.setup(redLed, GPIO.OUT)
GPIO.setup(yellowLed, GPIO.OUT)

# Create an MQTT client and attach our routines to it.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(username,password)
client.connect("mqtt.thingspeak.com", 1883, 60)
 
client.loop_forever()