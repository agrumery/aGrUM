import pyagrum as gum

v = gum.fastVariable("x[0:10:5]")
w = v.asDiscretizedVar()  # a DiscretizedVariable (a copy of v)
print(w.addTick(20))
# x:Discretized(<[0;2[,[2;4[,[4;6[,[6;8[,[8;10[,[10;20]>)
