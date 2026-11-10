import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.updateMetaData()
sorted(mrf.properties())
# ['creation', 'lastModification', 'name', 'software']
