class MyStuff(object):
    def __init__(self):
        self.name = "psd's stuff"
    
    def printName(self):
        print "I am psd's stuff"

thing = MyStuff()
print "name = %s" %thing.name
thing.printName()