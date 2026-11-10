import pyagrum as gum

bn = gum.BayesNet()
bn.add(gum.LabelizedVariable("smoking", "does the patient smoke ?", 2))
bn.variable("smoking").description()
# 'does the patient smoke ?'
