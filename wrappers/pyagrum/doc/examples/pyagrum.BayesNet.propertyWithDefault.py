import pyagrum as gum

bn = gum.fastBN("A->B")
bn.setProperty("author", "me")
bn.propertyWithDefault("author", "unknown")
# 'me'
bn.propertyWithDefault("license", "unknown")
# 'unknown'
