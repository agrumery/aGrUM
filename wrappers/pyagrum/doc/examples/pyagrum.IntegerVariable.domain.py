import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.domain()
# '{1|3|10}'
