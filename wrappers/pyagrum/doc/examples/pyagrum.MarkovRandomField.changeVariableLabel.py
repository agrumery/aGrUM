import pyagrum as gum

mrf = gum.fastMRF("A{no|yes}--B")
mrf.changeVariableLabel("A", "yes", "maybe")
mrf.variable("A").labels()
# ('no', 'maybe')
