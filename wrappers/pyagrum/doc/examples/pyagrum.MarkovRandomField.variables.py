import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
sorted(v.name() for v in mrf.variables())
# ['A', 'B', 'C']
sorted(v.name() for v in mrf.variables({mrf.idFromName("A"), mrf.idFromName("C")}))
# ['A', 'C']
