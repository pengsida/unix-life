import nltk
import collections
from nltk.corpus import movie_reviews
from nltk.corpus import stopwords
from nltk.classify import NaiveBayesClassifier

def bag_of_words(words):
    return dict([word,True] for word  in words)

def bag_of_words_not_int_set(words,badwords):
    return bag_of_words(set(words)-set(badwords))

def bag_of_non_stopwords(words,stopfile='english'):
    badwords = stopwords.words(stopfile)
    print badwords
    return bag_of_words_not_int_set(words,badwords)

def label_feats_from_corpus(corp,feature_detector=bag_of_words):
    label_feats = collections.defaultdict(list)
    for label in corp.categories():
        for fileid in corp.fileids(categories=[label]):
            feats = feature_detector(corp.words(fileids=[fileid]))
            label_feats[label].append(feats)
    return label_feats

def split_label_feats(lfeats,split=0.75):
    train_feats = []
    test_feats = []
    for label, feats in lfeats.iteritems():
        print label
        cutoff = int(len(feats)*split)
        train_feats.extend([(feat,label) for feat in feats[:cutoff]])
        test_feats.extend([(feat,label) for feat in feats[cutoff:]])
    return train_feats,test_feats

lfeats = label_feats_from_corpus(movie_reviews)
train_feats, test_feats = split_label_feats(lfeats)
nb_classifier = NaiveBayesClassifier.train(train_feats)
