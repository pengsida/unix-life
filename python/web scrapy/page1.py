# -*- coding:utf-8 -*-
import urllib2
import urllib
import re
from bs4 import BeautifulSoup

# #为了避免连接不上
# try:
#     html = urllib.urlopen("http://www.pythonscraping.com/pages/page1.html")
# except urllib.HTTPError as e:
#     print e
#
# # BeautifulSoup 对象表示的是一个文档的全部内容
# bsObj = BeautifulSoup(html.read(),'lxml')
# print bsObj.body.div.text

class scrapy_psd(object):
    def __init__(self,url):
        try:
            self.html = urllib.urlopen(url)
        except urllib2.HTTPError as e:
            print e
            return None
        self.obj = BeautifulSoup(self.html.read(),"lxml")

    def getTitle(self):
        try:
            title = self.obj.body.h1
        except AttributeError as e:
            return None
        return title

    def getTag(self,tag,attributes):
        tagList = self.obj.findAll(tag,attributes)
        return tagList

    def getTextNum(self,text):
        textList = self.obj.findAll(text)
        return len(textList)

    def getHref(self,tag,res):
        tagList = self.obj.findAll(tag,{"href": res})
        tagHref = []
        for tag in tagList:
            if 'href' in tag.attrs:
                tagHref.append(tag['href'])
        return tagHref

    def getWikiHref(self,tag):
        res = re.compile("^(/wiki/)((?!:).)*$")
        tagList = self.obj.findAll(tag,{"href": res})
        tagHref = []
        for tag in tagList:
            if 'href' in tag.attrs:
                tagHref.append(tag['href'])
        return tagHref


# check = scrapy_psd("http://www.pythonscraping.com/pages/page1.html")
# check = scrapy_psd("http://www.pythonscraping.com/pages/warandpeace.html")
# check = scrapy_psd("http://www.pythonscraping.com/pages/page3.html")
check = scrapy_psd("https://en.wikipedia.org/wiki/Kevin_Bacon")
res = re.compile("^(/wiki/)((?!:).)*$")
href = check.getHref('a',res)
for hre in href:
    print hre