import pyagrum as gum

guard = gum.RandomGeneratorGuard()
gum.initRandom(42)
guard.isRestored()
# False
guard.restore()
guard.isRestored()
# True
