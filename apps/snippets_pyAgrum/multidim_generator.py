# -*- coding: utf-8 -*-

def multidim_generator(bornes,sauf={}):
	"""
	Générateur pour une itération sur une série de variables dont on connait le nombre de modalités.
	
	bornes est un dictionnaire du type de {key1:3,key2:4,key3:5} (key1 est une variable à 3 modalités, etc.)
	sauf est un dictionnaire du type de {key2:1} : on ne veut pas itérer sur key2, qui vaudra 1 dans l'itérateur
	"""
	valeurs={}
	for k in bornes:
		if (sauf.has_key(k)):
			valeurs[k]=sauf[k]
		else:
			valeurs[k]=0

	done=False
	while (not done):
		yield valeurs
		done=True
		for k in valeurs:
			if (sauf.has_key(k)):
				continue
			if (valeurs[k]<bornes[k]-1):
				valeurs[k]+=1
				done=False
				break
			else:
				valeurs[k]=0

print "#### ITERATION SUR LES 3 VARIABLES #####"
for i in multidim_generator(dict(x=4,y=3,z=2)):
	print i

print "#### ITERATION SUR Y ET Z (X VAUT 3) #####"
for i in multidim_generator(dict(x=4,y=3,z=2),dict(x=3)):
	print i

print "#### ITERATION SUR X ET Y (Z VAUT 1) #####"
for i in multidim_generator(dict(x=4,y=3,z=2),dict(z=1)):
	print i

print "#### ITERATION SUR Y (X VAUT 1 et Z VAUT 0) #####"
for i in multidim_generator(dict(x=4,y=3,z=2),dict(x=1,z=0)):
	print i
