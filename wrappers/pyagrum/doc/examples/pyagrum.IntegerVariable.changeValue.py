import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.changeValue(3, 4)
print(n)
# n:Integer({1|4|10})
