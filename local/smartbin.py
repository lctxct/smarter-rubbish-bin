import os
import json
import datetime
from dotenv import load_dotenv
from time import sleep
# MQTT 
import paho.mqtt.client as mqtt
# Firebase 
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
# Supabase 
import supabase
from supabase import create_client, Client

# Topics
topic_recyclables = "fill_level/recyclable"
topic_trash = "fill_level/trash"
topic_class = "classification"

DB_TABLE_NAME = "bintable"
# Database columns
# id          int8
# created_at  timestamptz
# photo_path  text
# class       int8
# fill_r      float8
# fill_t      float8
photo_path = ""
class_val = 0
fill_r = 0.0
fill_t = 0.0

# Load environment variables
load_dotenv()
FIREBASE_URL = os.environ.get('FIREBASE_URL')
FIREBASE_API_KEY = os.environ.get('FIREBASE_API_KEY')
SUPABASE_URL = os.environ.get('SUPABASE_URL')
SUPABASE_API_KEY = os.environ.get('SUPABASE_API_KEY')

firebase_ref = None 
supabase_client = create_client(SUPABASE_URL, SUPABASE_API_KEY)

CONN = None
CUR = None

def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + str(rc))
	client.subscribe("#")

def on_message(client, userdata, message):
    print("Received message " + str(message.payload))

    binupdate = message.payload.decode().split(",")

    if message.topic == topic_recyclables:
        photo_path = binupdate[0]
        class_val = binupdate[1]
        fill_r = binupdate[2]
    elif message.topic == topic_trash:
        photo_path = binupdate[0]
        class_val = binupdate[1]
        fill_t = binupdate[2]
    
    supabase_insert_data(photo_path, class_val, fill_r, fill_t)
    firebase_insert_data(fill_r, fill_t)
        
def supabase_query_data():
    print("Querying data")
    table_name = DB_TABLE_NAME
    response = supabase_client.from_(table_name).select('*').execute()

    data = response.data
    print("Printing rows in data.......")
    for row in data:
        print("RoW:", row)

def supabase_insert_data(photo_path, class_val, fill_r, fill_t, bin_id=0):
    print("Inserting data")
    table_name = DB_TABLE_NAME
    data_to_insert = {
        "photo_path": photo_path,
        "class": class_val,
        "fill_r": fill_r, 
        "fill_t": fill_t
    }

    response = supabase_client.table(table_name).insert(data_to_insert).execute()

def firebase_init():
    # Fetch the service account key JSON file contents
    cred = credentials.Certificate('secrets/smart-rubbish-bin-firebase-adminsdk.json')

    # Initialize the app with a custom auth variable, limiting the server's access
    firebase_admin.initialize_app(cred, {
        'databaseURL': FIREBASE_URL,
        'databaseAuthVariableOverride': {
            'uid': 'smartapp'
        }
    })

    firebase_ref = db.reference('/fill-level')
    return firebase_ref

def firebase_insert_data(fill_r, fill_t):
    data = {
        "trash": {
            "level": fill_t
        }, 
        "recyclables": {
            "level": fill_r
        }
    }

    firebase_ref.set(data)

def firebase_get_data():
    print(firebase_ref.get())

if __name__ == '__main__':
    # Connect to Firebase
    print("Connecting to Firebase...")
    firebase_ref = firebase_init()
    firebase_get_data()
    
    # Connect to Supabase
    print("Connecting to Supabase...")
    supabase_query_data()
    #supabase_insert_data(photo_path, class_val, fill_r, fill_t)

    # Connect to MQTT
    print("Connecting to MQTT...")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("localhost", 1883, 60)
    client.loop_forever()
