import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.isNumerical()  # closestIndex is meaningful only for numerical variables
# False
try:
  c.closestIndex(1.5)
  msg = ""
except gum.NotImplementedYet as e:
  msg = str(e)
msg
# '[pyAgrum] Not implemented yet: closestIndex has no meaning for LabelizedVariable'
