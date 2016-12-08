import fileinput
import enchant

from enchant.tokenize import get_tokenizer
from enchant.tokenize import  HTMLChunker

class HTMLSpellChecker:
    def __init__(self,lang='en_US'):
        # self.lang = enchant.Dict(self.lang)
        self.lang = lang
        # These objects are used to check the spelling of words and to get suggestions for misspelled words
        self._dict = enchant.Dict(self.lang)
        #  provide an appropriate tokenization function which can be used to split the text
        # A chunker is simply a special tokenizer function that breaks text up into large chunks rather than individual tokens
        self._tk = get_tokenizer(self.lang,chunkers=(HTMLChunker,))

    def __call__(self, line):
        for word,off in self._tk(line):
            if not self._dict.check(word):
                yield word, self._dict.suggest(word)


if __name__ == '__main__':
    check = HTMLSpellChecker()
    for line in fileinput.input():
        for word,suggestions in check(line):
            print "error on line %d (%s) in file %s. \
                   Did you mean one of %s?" % \
                  (fileinput.filelineno(), word, \
                   fileinput.filename(),
                   ', '.join(suggestions))