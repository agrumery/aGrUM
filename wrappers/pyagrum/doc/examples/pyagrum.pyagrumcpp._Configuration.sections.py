import pyagrum as gum

gum.config.sections()[:3]
# ('core', 'approximation', 'sampling')
"tensor" in gum.config.sections()
# True
