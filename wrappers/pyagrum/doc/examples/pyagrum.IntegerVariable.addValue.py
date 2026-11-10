import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.addValue(5)
print(n)
# n:Integer({1|3|5|10})
