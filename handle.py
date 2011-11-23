#!/usr/bin/python
# File

myfile = open('/tmp/mag')
mylist = list()
while True:
    line = myfile.readline()
    if len(line) == 0:
        break;
    mylist.append(float(line))

myfile.close()

mylist.sort()
print mylist[:100]
