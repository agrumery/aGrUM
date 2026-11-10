import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.setMaxVal(8)
r.domain()
# '[2,8]'
