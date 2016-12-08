from PIL import Image
import os

def resizeImg(filename):
    img = Image.open(filename)
    Image._show(img)
    img = img.resize((img.size[0] / 2, img.size[1] / 2), )
    Image._show(img)

filenames = os.listdir(os.getcwd())
for filename in filenames:
    print filename.split('.')[-1]
    if filename.split('.')[-1] in "png jpg jpeg bmp":
        resizeImg(filename)
