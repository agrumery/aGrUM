import pyagrum as gum

gum.initRandom(0)  # a seed taken from the current time
seed = gum.randomGeneratorSeed()
x = gum.randomProba()
gum.initRandom(seed)  # replays the same sequence
x == gum.randomProba()
# True
