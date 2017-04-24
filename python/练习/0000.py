from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

im = Image.open("test.png")
draw = ImageDraw.Draw(im)
draw.text((im.size[0]-10,0),"1",fill=(255,0,0))
del draw
im.show()