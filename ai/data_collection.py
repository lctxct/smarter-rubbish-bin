import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import random
import math
import os
import requests
import time
from PIL import Image
import hashlib
from urllib.request import urlopen, Request
 
CLASSES = ['ewaste', 'glass', 'metal', 'paper', 'plastic', 'trash']
IMG_PATH = "./ai/img.jpg"
MODEL_PATH = "./ai/model.h5"

CAM_IP = 'http://'
IMG_SUB_URL = 'saved-photo'
POLL_DELAY = 3

def saveImage(img_url, imgCount):
    img = Image.open(requests.get(img_url, stream = True).raw)
    imgPath = "./ai/dataset/plastic/candybox" + str(imgCount) + ".jpg" 
    img.save(imgPath)
    print("Image saved at" + imgPath)


def pollWebServer(img_url):
    """
    """
    try:
        url = Request(img_url)
        response = urlopen(url).read()
        currentHash = hashlib.sha224(response).hexdigest()
        count = 0
        imgCount = 0
        print("Polling Web Server...")
    except Exception as e:
        print("error 1")
    while (True):
        try:
            time.sleep(POLL_DELAY)
            response = urlopen(url).read()
            newHash = hashlib.sha224(response).hexdigest()

            if newHash != currentHash:
                print("New Image found!")
                currentHash = newHash
                saveImage(img_url, imgCount)
                imgCount += 1
            else:
                print("No change...") if (count % 2 == 0) else print("No change..")
                count += 1
        except Exception as e:
            print("error 2")


if __name__ == "__main__":
    pollWebServer(CAM_IP + IMG_SUB_URL)