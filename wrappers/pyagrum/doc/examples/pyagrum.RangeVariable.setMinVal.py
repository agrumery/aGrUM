import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.setMinVal(0)
r.domain()
# '[0,5]'
