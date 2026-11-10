import pyagrum as gum

gum.config.push()
gum.config["tensor", "visibleDigits"] = 6
gum.config.reset()  # back to the defaults
gum.config["tensor", "visibleDigits"]
# '4'
gum.config.pop()
