import pyagrum as gum

gum.quoteFastName("X-ray")  # a '-' would be read as an edge
# "'X-ray'"
gum.quoteFastName("l'eau")  # a quote inside a word is an ordinary character
# "l'eau"
gum.quoteFastName("rock 'n roll")  # a quote at the beginning of a word is doubled
# "'rock ''n roll'"
gum.fastBN(gum.quoteFastName("X-ray") + "[3]->B").variable("X-ray").domainSize()
# 3
