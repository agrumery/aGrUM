#!/usr/bin/python

import re
import sys

classesToSkip = ['Vector_int','Vector_double','Vector_string','SwigPyIterator','GumException','SyntaxError'
           ,'MultiDimContainer_double','UtilityTable_double','BayesNetInference_double']

methodsToSkip = ['swig_import_helper','setTriangulation','statsObj','whenNodeAdded','whenNodeDeleted','whenArcAdded','whenArcDeleted','whenLoading','whenProgress','whenStop']

def noDocstring2(lines,dic):
    for i, line in enumerate(lines):
        if re.match("^def [^_].*",line, flags = 0) :
            nextline = lines[i+1]
            methodName = getMethodName(line)
            if not(re.match("^[ ]{4}\"\"\".*",nextline, flags = 0)) and not(methodName) in methodsToSkip :
                if not("" in dic):
                    dic[""]=[]
                dic[""].append(getMethodName(line)+" does not have any doc")
        if re.match("^[ ]{4}def [^_].*",line, flags = 0) :
            i+=1
            nextline = lines[i]
            if not(re.match("^[ ]{8}\"\"\"",nextline, flags = 0)) :
                methodClass = getClass(lines,i)
                methodName = getMethodName(line)
                if not(methodClass) in classesToSkip and not(methodName) in methodsToSkip:
                    if not(methodClass in dic) :
                        dic[methodClass]=[]
                    dic[methodClass].append(getMethodName(line)+" does not have any doc")
                
def signatureOnly2(lines,dic):
    for i, line in enumerate(lines):
        if re.match("^def [^_].*",line, flags = 0) :
            nextline = lines[i+1]
            methodName = getMethodName(line)
            if re.match("^\ {4}\"\"\".*\"\"\"",nextline, flags = 0) and not(methodName) in methodsToSkip :
                if not("" in dic):
                    dic[""]=[]
                dic[""].append(getMethodName(line)+" only has a signature")
        if re.match("^[ ]{4}def [^_].*",line, flags = 0) :
            nextline = lines[i+1]
            if re.match("^[ ]{8}\"\"\".*\"\"\"",nextline, flags = 0) :
                methodClass = getClass(lines,i)
                methodName = getMethodName(line)
                if not(methodClass) in classesToSkip and not(methodName) in methodsToSkip:
                    if not(methodClass in dic) :
                        dic[methodClass]=[]
                    dic[methodClass].append(methodName+" only has a signature")
                
def multipleSignatureOnly2(lines,dic):
    for i, line in enumerate(lines):
        if re.match("^def [^_].*",line, flags = 0):
            i+=1
            nextline = lines[i]
            methodName = getMethodName(line)
            if re.match("^[ ]{4}\"\"\"\n",nextline, flags = 0) and not(methodName) in methodsToSkip :
                isOnlySignatures = True
                i+=1
                nextline = lines[i]
                while not(re.match("^[ ]{4}\"\"\"",nextline, flags = 0)):
                    pattern = "^[ ]{,}"+getMethodName(line)+"\(.*"
                    if not(re.match(pattern,nextline, flags = 0)):
                        isOnlySignatures = False
                        break
                    else :
                        i+=1
                        nextline = lines[i]
                if isOnlySignatures :
                    if not("" in dic):
                        dic[""]=[]
                    dic[""].append(getMethodName(line)+" only has multiple signatures")
        if re.match("^[ ]{4}def [^_].*",line, flags = 0) :
            i+=1
            nextline = lines[i]
            if re.match("^[ ]{8}\"\"\"\n",nextline, flags = 0) :
                isOnlySignatures = True
                i+=1
                nextline = lines[i]
                while not(re.match("^[ ]{8}\"\"\"",nextline, flags = 0)):
                    pattern = "^[ ]{8}"+getMethodName(line)+"\(.*"
                    if not(re.match(pattern,nextline, flags = 0)):
                        isOnlySignatures = False
                        break
                    else :
                        i+=1
                        nextline = lines[i]
                if isOnlySignatures :
                    methodClass = getClass(lines,i)
                    methodName = getMethodName(line)
                    if not(methodClass) in classesToSkip:
                        if not(methodClass in dic) :
                            dic[methodClass]=[]
                        dic[methodClass].append(methodName+" only has multiple signatures")
                
def getClass(lines,i):
    previousline = lines[i-1]
    while not(re.match("^class",previousline, flags = 0)):
        i-=1
        previousline = lines[i]
    return re.search('class (.+?)\(.*', previousline).group(1)

def getMethodName(line):
    return re.search('def (.+?)\(.*', line).group(1)

def prettyprint(dic):
    for key in sorted(dic.keys()):
        if key=="":
            for value in dic[key]:
                print(value)
            print()
        else :
            print(key)
            print(''.join(["="]*len(key)))
            for value in dic[key]:
                print("\t"+value)
            print()

def numberOfMethods(lines):
    nb = 0
    for i, line in enumerate(lines):
        if re.match("^def [^_].*",line, flags = 0):
            nb += 1
        if re.match("^[ ]{4}def [^_].*",line, flags = 0):
            nb += 1
    return nb

def numberOfUndocumentedMethods(dic):
    i = 0
    for key in dic.keys():
        for val in dic[key]:
            i+=1
    return i

def main(argv):
    print()
    with open(argv[0], "r") as ins:
        lines = []
        for line in ins:
            lines.append(line)

    dic = dict()
    noDocstring2(lines,dic)
    signatureOnly2(lines,dic)
    multipleSignatureOnly2(lines,dic)
    prettyprint(dic)
    rate = ((numberOfMethods(lines)-numberOfUndocumentedMethods(dic))/numberOfMethods(lines))*100
    print("Doc completion rate : ","{0:.2f}".format(round(rate,2))+"%")

if __name__ == "__main__":
   main(sys.argv[1:])