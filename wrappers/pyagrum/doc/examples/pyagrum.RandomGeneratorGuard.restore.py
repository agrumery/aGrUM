import pyagrum as gum

reference = gum.RandomGeneratorGuard()
expected = gum.randomProba()  # the next draw of the program
reference.restore()  # back before this draw

guard = gum.RandomGeneratorGuard()
gum.initRandom(42)
draws = [gum.randomProba() for _ in range(3)]  # reproducible draws (seed 42)
guard.restore()
gum.randomProba() == expected  # the program goes on as if nothing had been drawn
# True

# in a with block, the state is restored at the end of the block, even if it is left by an exception
seed = gum.randomGeneratorSeed()
with gum.RandomGeneratorGuard():
  gum.initRandom(7)
  bn = gum.randomBN(n=4)
gum.randomGeneratorSeed() == seed
# True
