import pyagrum as gum

bn = gum.randomBN(names="ABCDE", domain_size=3)
sorted(bn.names())
# ['A', 'B', 'C', 'D', 'E']
all(bn.variable(i).domainSize() <= 3 for i in bn.nodes())  # domain_size is the maximal domain size
# True

bn = gum.randomBN(n=10, ratio_arc=1.5)  # names X00, X01, ... in a topological order
bn.size()
# 10
sorted(gum.randomBN().names())  # 5 nodes by default
# ['X00', 'X01', 'X02', 'X03', 'X04']
