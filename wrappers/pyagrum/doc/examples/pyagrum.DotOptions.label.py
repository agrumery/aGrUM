import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.label()  # nothing given
# ''
options.setLabel("my BN")
options.label()
# 'my BN'
