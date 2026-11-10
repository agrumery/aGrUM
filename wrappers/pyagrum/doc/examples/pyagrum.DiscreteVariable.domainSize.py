import pyagrum as gum

gum.fastVariable("c{red|green|blue}").domainSize()
# 3
gum.fastVariable("r[1,4]").domainSize()
# 4
