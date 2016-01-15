#!/usr/bin/python

import urllib2

def open_home():
    content = urllib2.urlopen("http://localhost:8080").read()

for i in range(1, 1000):
    open_home()
    print(str(i))