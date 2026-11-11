import pyagrum as gum

vI = gum.RangeVariable("I", "I in [4,10]", 4, 10)
print(vI)
# I:Range([4,10])
vI.maxVal()
# 10
vI.belongs(1)
# False
vI.index("5")  # where is the value 5 ?
# 1
vI.labels()
# ('4', '5', '6', '7', '8', '9', '10')
