# requires: matplotlib, scipy
import pyagrum as gum
import pyagrum.explain as expl

gum.initRandom(42)
bn = gum.fastBN("A->B->Y;C->Y")
gum.generateSample(bn, 500, "data.csv")
pvalues = expl.independenceListForPairs(bn, "data.csv", plot=False)
# the independences implied by bn (with a conditioning set), and the p-value of their test on data
sorted(pvalues)
# [('A', 'C', ()), ('A', 'Y', ('B',)), ('B', 'C', ())]
