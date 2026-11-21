import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;B-D")
sorted(g.namesFromIds(g.neighbours(1)))
# ['A', 'C', 'D']
