import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import random
import math
import os
import requests
import time
from PIL import Image
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures, StandardScaler, LabelEncoder
from sklearn.model_selection import train_test_split
import hashlib
from urllib.request import urlopen, Request
 
import threading

CLASSES = ['ewaste', 'glass', 'metal', 'paper', 'plastic', 'trash']
IMG_PATH = "./ai/img.jpg"
MODEL_PATH = "./ai/model.h5"

CAM_IP = 'http://'
IMG_SUB_URL = '/saved-photo'
POLL_DELAY = 3

def saveImage(img_url):
    img = Image.open(requests.get(img_url, stream = True).raw)
    img.save(IMG_PATH)
    print("Image saved as" + IMG_PATH)
    #img.save(IMG_PATH)


def pollWebServer(img_url, model):
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
            time.sleep(POLL_DELAY)
            response = urlopen(url).read()
            newHash = hashlib.sha224(response).hexdigest()

            if newHash != currentHash:
                print("New Image found!")
                currentHash = newHash
                saveImage(img_url)
                prediction, confidence = predict(model)
                sendByMqtt(prediction, confidence)
            else:
                print("No change...") if (count % 2 == 0) else print("No change..")
                count += 1
        except Exception as e:
            print("error")


def predict(model):
    img = tf.keras.preprocessing.image.load_img(IMG_PATH, target_size=(256, 256))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array, 0)

    predictions = model.predict(img_array)

    plt.imshow(img)
    print(predictions[0]*100, "\n", CLASSES)
    prediction = CLASSES[np.argmax(predictions)]
    confidence = predictions[0][np.argmax(predictions)] * 100
    print("Prediction: ", CLASSES[np.argmax(predictions)], f"{predictions[0][np.argmax(predictions)]*100}%")
    return prediction, confidence

def sendByMqtt(prediction, confidence):
    
    return


if __name__ == "__main__":
    #getImage() 
    model = tf.keras.saving.load_model(MODEL_PATH)
    #while (True):
    pollWebServer(CAM_IP + IMG_SUB_URL, model)