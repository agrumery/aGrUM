import pyagrum as gum

calls = []
gum.config.add_hook(lambda: calls.append("hook"))
gum.config.run_hooks()
calls
# ['hook']
