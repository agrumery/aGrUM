import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.rankDir()  # nothing given
# ''
options.setRankDir("LR")
options.rankDir()
# 'LR'
