import pyagrum as gum

gum.fastVariable("c{red|green}").isNumerical()
# False
gum.fastVariable("n{1|3|5}").isNumerical()
# True
