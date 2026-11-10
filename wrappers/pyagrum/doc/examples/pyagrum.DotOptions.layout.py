import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.layout()  # nothing given
# ''
options.setLayout("neato")
options.layout()
# 'neato'
