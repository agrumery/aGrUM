import pyagrum as gum

gum.config.push()  # save the current configuration
gum.config["tensor", "visibleDigits"] = 6
gum.config["tensor", "visibleDigits"]
# '6'
gum.config.pop()
gum.config["tensor", "visibleDigits"]
# '4'
