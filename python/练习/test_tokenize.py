import nltk

# Tokenizing text into sentences
para = "Hello World. It's good to see you. Thanks for buying this book."
print nltk.sent_tokenize(para)

# Tokenizing sentences into words
words = nltk.sent_tokenize(para)
for word in words:
    print nltk.word_tokenize(word)