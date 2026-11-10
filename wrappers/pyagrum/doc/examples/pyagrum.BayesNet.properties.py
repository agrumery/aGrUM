import pyagrum as gum

bn = gum.fastBN("A->B")
bn.setProperty("author", "me")
bn.setProperty("license", "MIT")
sorted(p for p in bn.properties() if p in ("author", "license"))
# ['author', 'license']
