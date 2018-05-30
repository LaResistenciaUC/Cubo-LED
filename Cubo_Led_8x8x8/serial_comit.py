import serial,time
from image_to_pixel import image_to_byte

arduino = serial.Serial("COM4", 250000)
print(arduino)
time.sleep(2)
images = ["pixel.png", "crepper.png", "preview.png"]

while True:
    for image in images:
        array = image_to_byte(image)
        print(array, image)
        arduino.write(array)
        time.sleep(0.3)
