import pyagrum as gum

v = gum.fastVariable("c{red|green|blue}")  # a LabelizedVariable
v.posLabel("green")
# 1
