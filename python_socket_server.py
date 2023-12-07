import asyncio
import websockets
import time
from os import listdir
import cv2
import numpy as np
import pickle
from sklearn.preprocessing import OneHotEncoder
from sklearn.model_selection import train_test_split
from keras.applications.vgg16 import VGG16
from keras.layers import Input, Flatten, Dense, Dropout
from keras.models import Model
from keras.callbacks import ModelCheckpoint
import matplotlib.pyplot as plt
import random
from keras.models import load_model
import sys
import urllib.request
import threading

nilon=0
nhua=0
giay=0

url = 'http://192.168.217.100/cam-lo.jpg'
cap = cv2.VideoCapture(0)

#class_name = ['Giay','nilon', 'Nhua', 'rackhac']
class_name = ['Giay','Nhua','nilon','rackhac']


def get_model():
    model_vgg16_conv = VGG16(weights='imagenet', include_top=False)

    # Dong bang cac layer
    for layer in model_vgg16_conv.layers:
        layer.trainable = False

    # Tao model
    input = Input(shape=(128, 128, 3), name='image_input')
    output_vgg16_conv = model_vgg16_conv(input)

    # Them cac layer FC va Dropout
    x = Flatten(name='flatten')(output_vgg16_conv)
    x = Dense(4096, activation='relu', name='fc1')(x)
    x = Dropout(0.5)(x)
    x = Dense(4096, activation='relu', name='fc2')(x)
    x = Dropout(0.5)(x)
    x = Dense(4, activation='softmax', name='predictions')(x)

    # Compile
    my_model = Model(inputs=input, outputs=x)
    my_model.compile(loss='categorical_crossentropy',
                     optimizer='adam', metrics=['accuracy'])

    return my_model


# Load weights model da train
my_model = get_model()
my_model.load_weights("weights-08-1.00.hdf5")

def kt():
    kt = 0
    img_req = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_req.read()), dtype=np.uint8)
    image_org = cv2.imdecode(imgnp, -1)

    # ret, image_org = cap.read()
    # if not ret:
    #     continue
    image_org = cv2.resize(image_org, dsize=None, fx=0.5, fy=0.5)
    # Resize
    image = image_org.copy()
    image = cv2.resize(image, dsize=(128, 128))
    image = image.astype('float')*1./255
    # Convert to tensor
    image = np.expand_dims(image, axis=0)

    # Predict
    predict = my_model.predict(image)
    print("This picture is: ", class_name[np.argmax(predict[0])], (predict[0]))
    print(np.max(predict[0], axis=0))
    if (np.max(predict) >= 0.9) and (np.argmax(predict[0]) != 3):
        if (class_name[np.argmax(predict[0])] == "nilon"):
            print("truenilon")
            kt = 1
            #/await websocket.send("1")
   
        if (class_name[np.argmax(predict[0])] == "Nhua"):
            print("truenhua")
            kt =2
            #await websocket.send("2")

        if (class_name[np.argmax(predict[0])] == "Giay"):
            print("truegiay")
            kt =3
            #/await websocket.send("3")
        
    cv2.imshow("Picture", image_org)

    cap.release()
    cv2.destroyAllWindows()

    return kt
    

async def echo(websocket):
    # async for message in websocket:

    # Capture frame-by-frame
    #
    img_req = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_req.read()), dtype=np.uint8)
    image_org = cv2.imdecode(imgnp, -1)

    # ret, image_org = cap.read()
    # if not ret:
    #     continue
    image_org = cv2.resize(image_org, dsize=None, fx=0.5, fy=0.5)
    # Resize
    image = image_org.copy()
    image = cv2.resize(image, dsize=(128, 128))
    image = image.astype('float')*1./255
    # Convert to tensor
    image = np.expand_dims(image, axis=0)

    # Predict
    predict = my_model.predict(image)
    print("This picture is: ", class_name[np.argmax(predict[0])], (predict[0]))
    print(np.max(predict[0], axis=0))

  
    if (np.max(predict) >= 0.9) and (np.argmax(predict[0]) != 3):
        #time.sleep(10)
        
        if (class_name[np.argmax(predict[0])] == "nilon"):
            print("truenilon")
            global nilon
            nilon =nilon+1
            if(nilon==3):
                await websocket.send("3")
                print(nilon)
                nilon=0
            #time.sleep(10);
   
        if (class_name[np.argmax(predict[0])] == "Nhua"):
            print("truenhua")
            global nhua
            nhua =nhua+1
            if(nhua==3):
                await websocket.send("1")
                print(nhua)
                nhua=0

        if (class_name[np.argmax(predict[0])] == "Giay"):
            print("truegiay")
            global giay
            giay = giay + 1
            if(giay == 3):
                await websocket.send("2")
                print(giay)
                giay=0
    
    #cv2.imshow("Picture", image_org)
      

cap.release()
cv2.destroyAllWindows()


async def main():
    async with websockets.serve(echo, "192.168.217.140", 8765):
        await asyncio.Future()  # run forever
        
        
       


asyncio.run(main())


    
    
