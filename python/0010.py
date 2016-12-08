import string
import random
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
class solution0010(object):
    def __init__(self,digits):
        self.digits = digits
        self.size = 30
        self.im = Image.new("RGB",(self.size*self.digits+10,self.size+20),(255,255,255))

    def drawLetters(self):
        draw = ImageDraw.Draw(self.im)
        self.size = 30
        font = ImageFont.truetype(font="/System/Library/Fonts/Hiragino Sans GB W6.ttc",size=self.size)
        for i in range(self.digits):
            fill = (random.randint(0,255),random.randint(0,255),random.randint(0,255))
            draw.text((5 + self.size * i, 10), random.choice(string.letters), fill, font)
        del draw

    def drawPixel(self):
        for i in range(self.im.width):
            for j in range(self.im.height):
                if self.im.getpixel((i,j)) == (255,255,255):
                    self.im.putpixel((i,j),(random.randint(0,255),random.randint(0,255),random.randint(0,255)))

    def show(self):
        Image._show(self.im)

so = solution0010(4)
so.drawLetters()
so.drawPixel()
so.show()