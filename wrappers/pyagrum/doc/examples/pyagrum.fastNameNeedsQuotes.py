import pyagrum as gum

gum.fastNameNeedsQuotes("Visit Asia")  # spaces are allowed in the name of a variable
# False
gum.fastNameNeedsQuotes("X-ray")
# True
gum.fastNameNeedsQuotes("X[0]")
# True
