import pyagrum as gum

gum.initRandom(42)
v = gum.fastVariable("x[0:10:5]")  # a DiscretizedVariable with intervals of width 2
x = v.draw(1)  # a random value in the interval [2;4[
2 <= x < 4
# True
