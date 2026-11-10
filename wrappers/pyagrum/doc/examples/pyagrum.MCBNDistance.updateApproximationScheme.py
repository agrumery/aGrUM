import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.initApproximationScheme()
dist.updateApproximationScheme()  # one more iteration
dist.updateApproximationScheme(10)  # ten more iterations
dist.nbrIterations()
# 11
