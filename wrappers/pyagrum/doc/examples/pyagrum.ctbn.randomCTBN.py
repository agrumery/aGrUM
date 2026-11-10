import pyagrum.ctbn as ct

ctbn = ct.randomCTBN((0.5, 2.0), n=3, parMax=1, modal=2)
ctbn.names()
# ['V1', 'V2', 'V3']
ctbn.labels("V1")
# ('v1_1', 'v1_2')
all(len(ctbn.parents(name)) <= 1 for name in ctbn.names())
# True
