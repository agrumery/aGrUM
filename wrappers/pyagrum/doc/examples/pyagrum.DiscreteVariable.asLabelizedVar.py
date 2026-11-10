import pyagrum as gum

v = gum.fastVariable("c{red|green}")
w = v.asLabelizedVar()  # a LabelizedVariable (a copy of v)
print(w.addLabel("blue"))
# c:Labelized({red|green|blue})
