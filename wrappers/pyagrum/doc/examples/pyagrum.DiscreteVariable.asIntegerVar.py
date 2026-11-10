import pyagrum as gum

v = gum.fastVariable("n{1|3|5}")
w = v.asIntegerVar()  # an IntegerVariable (a copy of v)
print(w.addValue(7))
# n:Integer({1|3|5|7})
