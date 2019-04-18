filename = 'testXYbase.csv'

with open(filename, "w") as file:
    for i in range(1):
        file.write("G1,WT\n")
    for i in range(6):
        file.write("G1,KO1\n")
    for i in range(7):
        file.write("G1,KO2\n")
    for i in range(4):
        file.write("G1,KO3\n")

    for i in range(17):
        file.write("S,WT\n")
    for i in range(17):
        file.write("S,KO1\n")
    for i in range(16):
        file.write("S,KO2\n")
    for i in range(17):
        file.write("S,KO3\n")

    for i in range(15):
        file.write("G2,WT\n")
    for i in range(5):
        file.write("G2,KO1\n")
    for i in range(6):
        file.write("G2,KO2\n")
    for i in range(20):
        file.write("G2,KO3\n")

import random
with open(filename,'r') as source:
    data = [ (random.random(), line) for line in source ]
data.sort()
with open(filename,'w') as target:
    target.write("X, Y\n")
    for _, line in data:
        target.write( line )