import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;B-D")
g.idsFromNames({"B": ["A", "C"]})  # keys and values of a dict
# {1: [0, 2]}
