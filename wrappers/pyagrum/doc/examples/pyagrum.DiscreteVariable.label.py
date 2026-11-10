import pyagrum as gum

v = gum.fastVariable("c{red|green|blue}")
v.label(1)
# 'green'
gum.fastVariable("x[0:10:5]").label(1)
# '[2;4['
