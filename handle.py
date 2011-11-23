#!/usr/bin/python
# File

def ele_cmp(e1, e2):
    return e1[0] - e[0]

def read_list(e1, myfile):
    while True:
        line = myfile.readline()
        if len(line) == 0:
            break;
        e1.append(line)

myfile = open('/tmp/log')
mylist = list()
img1 = list()
img2 = list()

while True:
    line = myfile.readline()
    if len(line) <= 2:
        break;
    img1.append(line)
while True:
    line = myfile.readline()
    if len(line) <= 2:
        break;
    img2.append(line)

while True:
    line = myfile.readline()
    if len(line) == 0:
        break;
    cmds = line.split()
    mylist.append([float(cmds[0]), cmds[1], cmds[2]])

myfile.close()

mylist.sort()
for item in mylist[:20]:
    print item
    print img1[int(item[1])]
    print img2[int(item[2])]
    print '-----------------'
    
