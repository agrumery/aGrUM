
	LICENCE :
	
SkoobEditor utilise la licence GPL.
Lisez le fichier LICENCE.txt pour plus de renseignement, ou visitez le site
http://www.gnu.org/licenses/gpl.html

	DEPENDANCES :

SkoobEditor dépend de 
 - Qt 4.7+
 - QScintilla 2.4.6+
 - aGrUM 0.8+

Pour Windows, la dll d'aGrUM est fournie, mais Qt et QScintilla (pour Qt4) doivent préalablement être installés.

Pour Unix, Qt, QScintilla et aGrUM doivent préalablement être installés.

Qt et QScintilla doivent se trouver dans les dépôts de votre distribution.
Sinon, vous pouvez les récupérer sur
http://qt.nokia.com/downloads/
http://www.riverbankcomputing.co.uk/software/qscintilla/ 

Le depôt SVN de aGrUM est disponible à l'adresse suivante :
https://alm.lip6.fr/svn/agrum/trunk
Vous trouverez les instructions d'installation dans la documentation du projet :
http://agrum.lip6.fr
 
	INSTRUCTIONS :

1) Entrer la commande (*) : make
L'exécutable sera dans le dossier bin.

(*) sous windows, make peut soit être nmake ou mingw32-make en fonction de la version de Qt installée.
Vérifiez que la variable d'environnement PATH contient bien %QTDIR%\qt\bin; avec %QTDIR% le 
répertoire ou vous avez installé Qt, pour que l'exécutable trouve les dll nécessaires.

	INSTALLATEUR 1 :
	
Sous windows, il est possible de créer un installateur pour installer l'exécutable et ses dépendances correctement
dans le dossier Program Files, de même que de créer les raccourcis dans le menu démarrer.

Pour cela, NSIS doit être installé (dans le répertoire par défaut, à savoir C:\Program Files\NSIS).

Entrer la commande (*) : make installer.
L'installateur sera dans le dossier bin.

	INSTALLATEUR 2 :
  
Sur les systèmes Linux/Debian ou Linux/Ubuntu (ou tout autre système basé les paquets deb),
il est possible de créer un paquet pour installer facilement SkoobEditor.

Entrer la commande : make dpkg
Le paquet sera dans le dossier apps.

NB : Qt, aGrUM et QScintilla doivent quand même être préalablement installés.
	
	BUGS / AMELIORATIONS :
	
Si vous trouvez des bugs ou souhaitez des améliorations ou de nouvelles fonctionnalités,
consultez le fichier BUGS.txt.
