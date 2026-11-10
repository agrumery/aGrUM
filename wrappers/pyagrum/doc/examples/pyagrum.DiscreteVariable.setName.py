import pyagrum as gum

v = gum.fastVariable("c{red|green}")
v.setName("color")
print(v)
# color:Labelized({red|green})
