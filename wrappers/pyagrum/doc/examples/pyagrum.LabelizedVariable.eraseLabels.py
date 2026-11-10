import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.eraseLabels()
c.domainSize()
# 0
