import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;B o-o D")
g.neighbours(1)  # whatever the marks
# {0, 2, 3}
