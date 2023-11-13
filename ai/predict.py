import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import random
import math
import os
import requests
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures, StandardScaler, LabelEncoder
from sklearn.model_selection import train_test_split

DIR = "dataset-resized"
# classes = train_dataset.class_names
# numClasses = len(train_dataset.class_names)
# print(classes)
CLASSES = ['ewaste', 'glass', 'metal', 'paper', 'plastic', 'trash']
# # img_data = requests.get("https://images.unsplash.com/photo-1591872203534-278fc084969e?ixlib=rb-1.2.1&ixid=MnwxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8&auto=format&fit=crop&w=1064&q=80").content
# # with open('img.jpg', 'wb') as handler:
# #     handler.write(img_data)
IMG_PATH = "./ai/img.jpg"
MODEL = "./ai/model2.h5"
# train_dataset = tf.keras.preprocessing.image_dataset_from_directory(DIR, validation_split=0.1, subset="training", seed=42, batch_size=128, smart_resize=True, image_size=(256, 256))
# test_dataset = tf.keras.preprocessing.image_dataset_from_directory(DIR, validation_split=0.1, subset="validation", seed=42, batch_size=128, smart_resize=True, image_size=(256, 256))

# AUTOTUNE = tf.data.AUTOTUNE

# train_dataset = train_dataset.prefetch(buffer_size=AUTOTUNE)


def predict(model):
    img = tf.keras.preprocessing.image.load_img(IMG_PATH, target_size=(256, 256))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array, 0)

    predictions = model.predict(img_array)

    plt.imshow(img)
    print(predictions[0]*100, "\n", CLASSES)
    print("Prediction: ", CLASSES[np.argmax(predictions)], f"{predictions[0][np.argmax(predictions)]*100}%")


if __name__ == "__main__":
    model = tf.keras.saving.load_model(MODEL)
    predict(model)
