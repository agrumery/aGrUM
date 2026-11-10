import pyagrum as gum

gum.initRandom(42)
x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
v = x.draw(1)  # a random value in the interval [1;5[
1 <= v < 5
# True
