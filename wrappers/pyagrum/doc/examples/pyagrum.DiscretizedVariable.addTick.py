import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.addTick(20)
print(x)
# x:Discretized(<[0;1[,[1;5[,[5;10[,[10;20]>)
