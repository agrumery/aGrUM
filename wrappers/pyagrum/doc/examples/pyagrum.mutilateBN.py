import pyagrum as gum

bn = gum.fastBN("A->B->C")
inter, evs = gum.mutilateBN(bn, intervention={"B": ["1"]})  # do(B=1): B is removed, C depends on its value
sorted(inter.names())
# ['A', 'C']
inter, evs = gum.mutilateBN(bn, observation={"B": [0, 1]})  # B=1 observed
evs
# {'B': [0, 1]}
