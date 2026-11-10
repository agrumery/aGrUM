import pyagrum as gum

v = gum.LabelizedVariable("smoking", "does the patient smoke ?", 2)
v.toStringWithDescription()
# 'does the patient smoke ?{0|1}'
