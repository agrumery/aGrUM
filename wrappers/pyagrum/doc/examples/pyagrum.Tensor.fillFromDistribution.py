import scipy.stats as stats

import pyagrum as gum

bn = gum.fastBN("A[3]->B[5]")
bn.cpt("B").fillFromDistribution(stats.binom, n=4, p="(A+1)/4")
round(float(bn.cpt("B")[{"A": 2}].sum()), 4)  # each distribution sums to 1
# 1.0
int(bn.cpt("B")[{"A": 2}].argmax())  # the mode of binom(n=4, p=0.75)
# 3
