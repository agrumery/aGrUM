
	LICENCE :
	
SkoobEditor utilise la licence GPL.
Lisez le fichier LICENCE.txt pour plus de renseignement, ou visitez le site
http://www.gnu.org/licenses/gpl.html

	DEPENDANCES :

SkoobEditor dépend de 
 - Qt 4.7+
 - QScintilla 2.4.6+
 - aGrUM 0.8+

Qt et QScintilla doivent préalablement être installés.

Qt et QScintilla doivent se trouver dans les dépôts de votre distribution.
Sinon, vous pouvez les récupérer sur
http://qt.nokia.com/downloads/
http://www.riverbankcomputing.co.uk/software/qscintilla/ 

Le depôt SVN de aGrUM est disponible à l'adresse suivante :
https://alm.lip6.fr/svn/agrum/trunk
Vous trouverez les instructions d'installation dans la documentation du projet :
http://agrum.lip6.fr
 
	INSTRUCTIONS :

Entrer la commande (*) : make
L'exécutable sera dans le dossier bin/Release.

(*) sous windows, make peut soit être nmake ou mingw32-make en fonction de la version de Qt installée.
Seul mingw32-make à été testé.
Pour exécuté le programme, cf INSTALLATEUR WINDOWS ci-dessous ou
vérifiez que la variable d'environnement PATH contient bien %QTDIR%\bin; avec QTDIR la variable contenant le 
répertoire où est installé Qt (Ex: C:\QtSQK\Desktop\Qt\4.7.2\mingw), ainsi que le répertoire où est installé make (Ex: C:\QtSQK\mingw\bin).

	INSTALLATEUR WINDOWS :
	
Sous windows, il est possible de créer un installateur pour installer l'exécutable et ses dépendances correctement
dans le dossier Program Files, de même que de créer les raccourcis dans le menu démarrer.

Pour cela, NSIS doit être installé (dans le répertoire par défaut, à savoir C:\Program Files\NSIS).

Entrer la commande (*) : make installer.
L'installateur sera dans le dossier bin.

	INSTALLATEUR LINUX :
  
Sur les systèmes Linux/Debian ou Linux/Ubuntu (ou tout autre système basé les paquets deb),
il est possible de créer un paquet pour installer facilement SkoobEditor.

Entrer la commande : make dpkg
Le paquet *.deb sera dans le dossier apps.

NB : Pour le moment, Qt, aGrUM et QScintilla doivent quand même être préalablement installés.
	
	BUGS / AMELIORATIONS :
	
Si vous trouvez des bugs ou souhaitez des améliorations ou de nouvelles fonctionnalités,
consultez le fichier BUGS.txt.
