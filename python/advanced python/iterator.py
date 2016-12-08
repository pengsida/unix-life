#如何实现迭代器
class MyIterator(object):
    def __init__(self,step):
        self.step = step
    
    def next(self): #返回容器的下一个项目
        if self.step == 0:
            raise StopIteration
        self.step -= 1
        return self.step

    def __iter__(self): #返回迭代器本身
        return self