import redis
import randint

def proRandTable(r,digits,nums):
    r.flushall()
    for i in range(0,nums):
        r.set(i,randint.proRand(digits))

def printTable(r):
    for i in range(0,r.dbsize()):
        print r.get(i)

r = redis.StrictRedis(host='localhost', port=6379, db=0)
proRandTable(r,3,100)
printTable(r)