import pyagrum as gum

arc = gum.Arc(2, 5)  # the arc 2->5
arc.other(2)
# 5
arc.other(5)
# 2
