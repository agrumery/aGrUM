import pyagrum as gum

gum.fastVariable("c{red|green}").toFast()
# 'c{red|green}'
gum.fastVariable("r[1,4]").toFast()
# 'r[1,4]'
