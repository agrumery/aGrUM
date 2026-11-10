import pyagrum as gum

gum.fastVariable("c{red|green}").empty()
# False
gum.fastVariable("c{red}", 1).empty()  # domain size < 2 (allowed by the minimal domain size 1)
# True
