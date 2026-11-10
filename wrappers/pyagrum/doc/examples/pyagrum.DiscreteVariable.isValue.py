import pyagrum as gum

v = gum.fastVariable("n{1|3|5}")  # an IntegerVariable
v.isValue(3)
# True
v.isValue(4)
# False
