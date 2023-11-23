import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

CLASSES = ['ewaste', 'metal', 'paper', 'plastic', 'trash']
IMG_PATH = "./ai/img.jpg"
MODEL = "./ai/model3.h5"

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
