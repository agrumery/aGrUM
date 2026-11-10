import pyagrum as gum

c = gum.LabelizedVariable("c", "color", 0)
c.addLabel("red").addLabel("green")
print(c)
# c:Labelized({red|green})
