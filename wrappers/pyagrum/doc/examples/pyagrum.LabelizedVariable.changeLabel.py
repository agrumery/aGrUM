import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.changeLabel(1, "yellow")
print(c)
# c:Labelized({red|yellow|blue})
