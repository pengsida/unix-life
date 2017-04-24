# -*- coding:utf-8 -*-
import urllib
import urllib2
from bs4 import BeautifulSoup

class solution0008(object):
    def __init__(self,url):
        try:
            self.html = urllib.urlopen(url)
        except urllib2.HTTPError as e:
            print "hello world"
            return None
        self.obj = BeautifulSoup(self.html.read(),'lxml')

    def getArticle(self):
        article = self.obj.find_all("article")
        return article

so = solution0008("https://github.com/Yixiaohan/show-me-the-code")
articles = so.getArticle()
for article in articles:
    print article.text