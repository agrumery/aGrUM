import pyagrum as gum

vX = gum.DiscretizedVariable("X", "X has been discretized").addTick(1).addTick(2).addTick(3).addTick(3.1415)
print(vX)
# X:Discretized(<[1;2[,[2;3[,[3;3.1415]>)
vX.isTick(4)
# False
vX.labels()
# ('[1;2[', '[2;3[', '[3;3.1415]')
vX.index("2.5")  # where is the real value 2.5 ?
# 1
vY = gum.DiscretizedVariable("Y", "Y", [0, 0.5, 1], True)  # empirical: values out of bounds are accepted
print(vY)
# Y:Discretized(<(0;0.5[,[0.5;1)>)
vY.index("7")
# 1
