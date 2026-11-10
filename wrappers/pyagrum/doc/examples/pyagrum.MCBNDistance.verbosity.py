import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.verbosity()
# False
dist.setVerbosity(True)  # keep the history of the errors
dist.verbosity()
# True
