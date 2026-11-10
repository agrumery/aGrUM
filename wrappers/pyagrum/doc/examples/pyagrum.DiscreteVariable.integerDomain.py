import pyagrum as gum

v = gum.fastVariable("n{1|3|5}")  # an IntegerVariable
v.integerDomain()
# (1, 3, 5)
