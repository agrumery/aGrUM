import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.size()  # nothing given
# ''
options.setSize("3!")
options.size()
# '3!'
