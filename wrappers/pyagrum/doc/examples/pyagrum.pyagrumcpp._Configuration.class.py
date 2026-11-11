import pyagrum as gum

gum.config.push()
gum.config["dynamicBN", "graphSize"] = 10
gum.config["dynamicBN", "graphSize"]  # values are stored as strings
# '10'
gum.config.typed["tensor", "visibleDigits"]  # converted according to the declared type
# 4
gum.config.pop()
