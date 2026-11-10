import pyagrum as gum

gum.config.push()
gum.config.set("tensor", "visibleDigits", 6)  # same as gum.config["tensor", "visibleDigits"] = 6
gum.config["tensor", "visibleDigits"]
# '6'
gum.config.pop()
