import pyagrum as gum

bn = gum.fastBN("A->B")
bn.setProperty("author", "me")
bn.property("author")
# 'me'
