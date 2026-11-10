import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setMaxIter(1000)
sorted(dist.compute())
# ['bhattacharya', 'errorPQ', 'errorQP', 'hellinger', 'jensen-shannon', 'klPQ', 'klQP']
copy = gum.BayesNet(p)
same = gum.MCBNDistance(p, copy)
same.compute()["hellinger"]  # no difference between a BN and its copy
# 0.0
