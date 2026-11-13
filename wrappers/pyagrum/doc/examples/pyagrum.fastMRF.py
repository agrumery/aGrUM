import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")  # a factor over A,B,C and another one over C,D
mrf.factors()
# [{0, 1, 2}, {2, 3}]

# the second argument is the default domain size of the variables without specification
mrf = gum.fastMRF("A--B[1,3]--C{yes|No};C--D[2,4]--E[1,2.5,3.9]", 6)
print(mrf.variable("A"))
# A:Range([0,5])
print(mrf.variable("E"))
# E:Discretized(<[1;2.5[,[2.5;3.9]>)

# a name between single quotes may contain '--'
mrf = gum.fastMRF("'a--b'--C")
sorted(mrf.names())
# ['C', 'a--b']
