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

CLASSES = ['cardboard', 'glass', 'metal', 'paper', 'plastic', 'trash']
IMG_PATH = "./ai/img.jpg"
MODEL_PATH = "./ai/model.h5"
# IMG_PATH = "img.jpg"
# MODEL_PATH = "model.h5"
IMG_URL = 'http://0.0.0.0/saved-photo'


def saveImage(img_url):
    img = Image.open(requests.get(img_url, stream = True).raw)
    img.save(IMG_PATH)


def pollWebServer(img_url, model):
    """
    """
    #url = Request(IMG_URL,
    #            headers={'User-Agent': 'Mozilla/5.0'})
    url = Request(img_url)
    response = urlopen(url).read()
    currentHash = hashlib.sha224(response).hexdigest()
    print("Polling Web Server...")

    while (True):
        try:
            time.sleep(3)
            response = urlopen(url).read()
            newHash = hashlib.sha224(response).hexdigest()

            if newHash != currentHash:
                print("New Image found!")
                currentHash = newHash
                saveImage(img_url)
                predict(model)
            else:
                print("No change...")
        except Exception as e:
            print("error")


def predict(model):
    img = tf.keras.preprocessing.image.load_img(IMG_PATH, target_size=(256, 256))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array, 0)

    predictions = model.predict(img_array)

    plt.imshow(img)
    print(predictions[0]*100, "\n", CLASSES)
    print("Prediction: ", CLASSES[np.argmax(predictions)], f"{predictions[0][np.argmax(predictions)]*100}%")


if __name__ == "__main__":
    #getImage() 
    model = tf.keras.saving.load_model(MODEL_PATH)
    #while (True):
    pollWebServer(IMG_URL, model)
        #predict(model)
