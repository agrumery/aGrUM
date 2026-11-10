import pyagrum as gum

v = gum.fastVariable("x[0:10:5]")  # the middle of each interval
v.numerical(1)
# 3.0
gum.fastVariable("n{1|3|5}").numerical(2)
# 5.0
