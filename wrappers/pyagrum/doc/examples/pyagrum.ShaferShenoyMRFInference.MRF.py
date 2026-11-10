import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.MRF().size()
# 4
sorted(ie.MRF().names())
# ['A', 'B', 'C', 'D']
