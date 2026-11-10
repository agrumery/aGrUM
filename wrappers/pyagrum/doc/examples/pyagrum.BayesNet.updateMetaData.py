import pyagrum as gum

bn = gum.fastBN("A->B")
sorted(bn.properties())
# ['name']
bn.updateMetaData()
sorted(bn.properties())
# ['creation', 'lastModification', 'name', 'software']
