import pyagrum as gum

print(gum.fastVariable("A{On|Off|Defun}"))
# A:Labelized({On|Off|Defun})
print(gum.fastVariable("A[4,6]"))
# A:Range([4,6])
print(gum.fastVariable("A[1:6:5]"))
# A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)
print(gum.fastVariable("A{1|3|9}"))
# A:Integer({1|3|9})
