import pyagrum as gum

gum.config.push()
gum.config["tensor", "visibleDigits"] = 6
gum.config.pop()  # back to the pushed configuration
gum.config["tensor", "visibleDigits"]
# '4'
