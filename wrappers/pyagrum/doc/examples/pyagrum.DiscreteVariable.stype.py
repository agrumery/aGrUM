import pyagrum as gum

gum.fastVariable("c{red|green}").stype()
# 'Labelized'
gum.fastVariable("x[0:10:5]").stype()
# 'Discretized'
