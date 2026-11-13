import pyagrum as gum

gum.initRandom(42)
first = gum.randomNumpyGenerator().normal(size=3)
gum.initRandom(42)
second = gum.randomNumpyGenerator().normal(size=3)
bool((first == second).all())  # the numpy draws follow the seed of aGrUM
# True

rng = gum.randomNumpyGenerator()  # each call draws a new seed
bool((rng.normal(size=3) == gum.randomNumpyGenerator().normal(size=3)).all())
# False
