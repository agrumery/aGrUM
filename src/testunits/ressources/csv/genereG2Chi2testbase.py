import random

filename = "testXYbase.csv"

with open(filename, "w") as file:
  file.writelines("G1,WT\n" for i in range(1))
  file.writelines("G1,KO1\n" for i in range(6))
  file.writelines("G1,KO2\n" for i in range(7))
  file.writelines("G1,KO3\n" for i in range(4))

  file.writelines("S,WT\n" for i in range(17))
  file.writelines("S,KO1\n" for i in range(17))
  file.writelines("S,KO2\n" for i in range(16))
  file.writelines("S,KO3\n" for i in range(17))

  file.writelines("G2,WT\n" for i in range(15))
  file.writelines("G2,KO1\n" for i in range(5))
  file.writelines("G2,KO2\n" for i in range(6))
  file.writelines("G2,KO3\n" for i in range(20))


with open(filename) as source:
  data = [(random.random(), line) for line in source]
data.sort()
with open(filename, "w") as target:
  target.write("X, Y\n")
  target.writelines(line for _, line in data)
