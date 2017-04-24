# -*- coding=utf-8 -*-

import urllib
import urllib2
from bs4 import BeautifulSoup
from PIL import ImageFile
import os

class solution0012(object):
    def __init__(self,url):
        try:
            self.html = urllib.urlopen(url)
        except urllib2.HTTPError as e:
            return None
        self.htmlobj = BeautifulSoup(self.html,'lxml')

    def getImgSrcs(self):
        images = self.htmlobj.find_all(name='img',attrs={"class":"BDE_Image"})
        self.srcs = []
        for img in images:
            self.srcs.append(img['src'])

    def saveImgs(self):
        if not os.path.exists("temp"):
            os.makedirs("temp")
        for src in self.srcs:
            urllib.urlretrieve(url=src,filename="temp/"+src.split('/')[-1])

so = solution0012("http://tieba.baidu.com/p/2166231880")
so.getImgSrcs()
so.saveImgs()