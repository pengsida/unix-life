import sys
import os
import enchant
from enchant.tokenize import get_tokenizer

class countWords(object):
    def __init__(self,lang='en_US'):
        self.num = 0
        self.lang = lang
        self._dict = enchant.Dict(lang)
        self._parser = get_tokenizer(self.lang)

    def __call__(self, line):
        for word,off in self._parser(line):
            if self._dict.check(word):
                self.num += 1


def countWord(filename):
    file_object = open(filename)
    check = countWords()
    for line in file_object:
        check(line)
    print check.num

countWord('test.txt')