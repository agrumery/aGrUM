import pyagrum as gum

v = gum.fastVariable("r[1,4]")
r = v.asRangeVar()  # a RangeVariable (a copy of v)
r.setMaxVal(6)
print(r)
# r:Range([1,6])
