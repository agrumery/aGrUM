import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.eraseValue(3)
print(n)
# n:Integer({1|10})
