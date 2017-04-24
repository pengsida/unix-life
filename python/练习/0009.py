# -*- coding=utf-8 -*-
import urllib
import urllib2
from bs4 import BeautifulSoup
import re

class solution0009(object):
    def __init__(self,url):
        try:
            self.html = urllib.urlopen(url)
        except urllib2.HTTPError as e:
            print "hello world"
            return None
        self.bsobj = BeautifulSoup(self.html.read(),'lxml')

    def getHref(self):
        hrefs = self.bsobj.find_all(href=re.compile(".*://.*"))
        return hrefs

so = solution0009("https://github.com/Yixiaohan/show-me-the-code")
hrefs = so.getHref()
for href in hrefs:
    print href['href']