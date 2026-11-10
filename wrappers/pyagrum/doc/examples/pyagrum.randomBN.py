import pyagrum as gum

bn = gum.randomBN(names="ABCDE", domain_size=3)
sorted(bn.names())
# ['A', 'B', 'C', 'D', 'E']
all(bn.variable(i).domainSize() <= 3 for i in bn.nodes())  # domain_size is the maximal domain size
# True
