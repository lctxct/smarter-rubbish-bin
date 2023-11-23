import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import os
import requests
import paho.mqtt.client as mqtt
from time import sleep 
from PIL import Image
import hashlib
from urllib.request import urlopen, Request

CLASSES = ['ewaste', 'metal', 'paper', 'plastic', 'trash']
RECYCLABLE = ['metal', 'paper', 'plastic']
TRASH = ['ewaste', 'trash']

IMG_FOLDER_PATH = "images/"
TEMP_IMG_PATH = "img.jpg"
MODEL_PATH = "model3.h5"

CAM_IP = 'http://172.20.10.13/'
IMG_SUB_URL = 'saved-photo'
MQTT_BROKER_IP = '172.20.10.12'
POLL_DELAY = 4

def downloadImage(img_url):
    img = Image.open(requests.get(img_url, stream = True).raw)
    img.save(TEMP_IMG_PATH)
    print("Image temporarily saved at " + TEMP_IMG_PATH)
    return img


def fileImage(img, prediction):
    folderPath = IMG_FOLDER_PATH + prediction + '/'
    entries = os.listdir(folderPath)
    # Find latest entry
    latestEntryIndex = 0
    if entries:
        for entry in entries:
            entryIndex = int(entry[len(prediction): len(entry)- len('.jpg')])
            if entryIndex > latestEntryIndex:
                latestEntryIndex = entryIndex
        newEntry = prediction + str(latestEntryIndex + 1) + '.jpg'
    else:
        newEntry = prediction + '0' + '.jpg'
    
    imgDir = folderPath + newEntry
    img.save(imgDir)
    print("Image saved as " + newEntry)
    return imgDir


def pollWebServer(img_url, model, client):
    """
    """
    #url = Request(IMG_URL,
    #            headers={'User-Agent': 'Mozilla/5.0'})
    url = Request(img_url)
    response = urlopen(url).read()
    currentHash = hashlib.sha224(response).hexdigest()
    count = 0
    print("Polling Web Server...")

    while (True):
        try:
            sleep(POLL_DELAY)
            response = urlopen(url).read()
            newHash = hashlib.sha224(response).hexdigest()

            if newHash != currentHash:
                print("New Image found!")
                currentHash = newHash
                img = downloadImage(img_url)
                prediction = predict(model)
                imgPath = fileImage(img, prediction)
                sendByMqtt(client, imgPath, prediction)
            else:
                print("No change...") if (count % 2 == 0) else print("No change..")
                count += 1
        except Exception as e:
            print("The ESP32-CAM might be asleep!")


def predict(model):
    img = tf.keras.preprocessing.image.load_img(TEMP_IMG_PATH, target_size=(256, 256))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array, 0)

    predictions = model.predict(img_array)

    plt.imshow(img)
    print(predictions[0]*100, "\n", CLASSES)
    prediction = CLASSES[np.argmax(predictions)]
    confidence = predictions[0][np.argmax(predictions)] * 100
    print("Prediction: ", CLASSES[np.argmax(predictions)], f"{predictions[0][np.argmax(predictions)]*100}%")
    return prediction


def sendByMqtt(client, imgPath, prediction):
    if prediction in RECYCLABLE:
         classification = 'recyclable'
    elif prediction in TRASH:
         classification = 'trash'
    
    client.publish("classification", classification)
    client.publish("ai/image", imgPath)
    print(f"Classification {classification} and Filepath {imgPath} published.")


def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + str(rc))
	client.subscribe("#")


def on_message(client, userdata, message):
	print("Received message " + str(message.payload))
     

if __name__ == "__main__":
    model = tf.keras.saving.load_model(MODEL_PATH)
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER_IP, 1883, 60)
    pollWebServer(CAM_IP + IMG_SUB_URL, model, client)  