import pyagrum as gum

bn = gum.fastBN("A->B")
bn.setProperty("author", "me")
bn.existsProperty("author")
# True
bn.existsProperty("license")
# False
