import pyagrum as gum

gum.initRandom(42)
x = gum.randomProba()
gum.initRandom(42)  # the same seed gives the same sequence
x == gum.randomProba()
# True
