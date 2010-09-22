/*
** Date : 10 April 2010
** Author : Ni NI           
*/
#include "Session.h"
#include <stdexcept>
#include <typeinfo>
#include <sys/stat.h>
#include <stdlib.h>
#include <agrum/core/timer.h>

#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/PRMInference.h>
#include <agrum/prm/groundedInference.h>
#include <agrum/prm/SVE.h>
#include <agrum/prm/SVED.h>
#include <agrum/prm/PRMFactory.h>

#define KW_request "request"
#define KW_import "import"
#define KW_package "package"
#define KW_interrogation "?"
#define KW_unobservation "unobserve"
#define KW_engine "engine"
#define KW_output "output"
#define ERR_OutRequest "Action doit être écrit à l'intérieur d'un requête."
#define ERR_VariableNotExists "Le variable n'existe pas."
#define ERR_File "The file does not exist."

/*******************************************************/
/****Constructeur et Destructeur************************/
/*******************************************************/

Session::Session () {
  this->inside_request = false;
  this->quit = false;
}

Session::~Session () {
  imports.erase (imports.begin (), imports.end ());
  requests.erase (requests.begin (), requests.end ());
}

/*******************************************************/
/****Inserer indice & trouver indice********************/
/****Ainsi que les Recuperations************************/
/*******************************************************/
void
Session::insertPackageInformation (int line, string str) {
  package_line = line;
  package = str;
}

void
Session::insertImport (Import & imp) {
  imports.push_back (imp);
}

void Session::insertEngine(Engine& eng){
  engine = eng;
}

void Session::setExitStatus(bool status){
  quit = status;
}

bool Session::getExitStatus(){
  return quit;
}

vector < Import > &Session::getImports () {
  return imports;
}

vector < Request > &Session::getRequests () {
  return requests;
}

string Session::getPackageName () {
  return package;
}

Engine& Session::getEngine(){
  return engine;
}
/************************r*******************************/
/****Recuperer la taille des tableaux*******************/
/*******************************************************/
int
Session::getImportsSize () {
  return imports.size ();
}

int
Session::getRequestsSize () {
  return requests.size ();
}

/*******************************************************/
/****Convertir une string en une liste de string********/
/*******************************************************/
vector < string > Session::convertStringList (string s) {
  vector < string > str_list;
  istringstream iss (s);
  copy (istream_iterator < string > (iss),
	istream_iterator < string > (), back_inserter (str_list));
  return str_list;
}

/*******************************************************/
/****Verifier syntaxe request***************************/
/*******************************************************/

void
Session::enterRequest (Request q) {
  this->inside_request = true;
  requests.push_back (q);
}

Request *
Session::getCurrentRequest () {

  return &requests.back ();
}

bool Session::isInsideRequest () {
  return this->inside_request;
}

void
Session::quitRequest () {
  this->inside_request = false;
}



/*******************************************************/
/****Parser et show -> exécuter (plus tard)*************/
/*******************************************************/
void
Session::parser (char *nomfichier) {
  fstream fichier (nomfichier);
  if(!fichier){
    cerr <<ERR_File<<endl;
    exit(1);
  }
  else{
    string line;
    int nb_line = 0;
    int i = 0;

    while (!fichier.eof ()) {
      //*char pch;
      //si on ne precise pas le type d'engine, le type par defaut est SVE
      Engine eng;
      eng.insertLine (nb_line);
      
      getline (fichier, line);
      nb_line++;
      //ne traite pas les commentaires '//'
      if (line.find ("//", 0) == line.npos) {
	vector < string > vmots = this->convertStringList (line);

	if (vmots.size () == 1 && !vmots[0].compare ("}")) {
	  this->quitRequest ();
	}
	else if (vmots.size () >= 1) {
	  string mot = vmots[0];

	  if (!mot.compare (KW_request) && vmots.size () >= 2) {
	    Request q (nb_line, vmots[1]);
	    this->enterRequest (q);
	  }
	  else if (!mot.compare (KW_import) && vmots.size () >= 2) {
	    string completImport = vmots[1];
	    TokenizableString str_imp(completImport);
	    vector<string> elts_import = str_imp.strtoken(";");
	    string resu = elts_import[0];
	    Import imp (nb_line, resu);
	    this->insertImport (imp);

	    
	  }//fin Import
	  else if (!mot.compare (KW_package) && vmots.size () >= 2){
	    this->insertPackageInformation (nb_line, vmots[1]);

	  }
	  else if (!mot.compare (KW_engine) && vmots.size () >= 2) {
	    Engine eng;
	    try {
	      string typeEng = vmots[1];
	      TokenizableString str (typeEng);
	      vector < string > elements = str.strtoken (";");
	      eng.insertType (eng.convertStringType (elements[0]));
	      eng.insertLine (nb_line);
	      this->insertEngine(eng);
	    }
	    catch (const exception & e) {
	      cerr << e.what () << endl;
	    }

	  }
	  /*else if (!mot.compare (KW_output) && vmots.size () >= 2){
	    this->insertOutFileInformation(nb_line,vmots[1]);
	    this->outFile_exists = true;
	  }*/
	  else {
	    if (!this->isInsideRequest ()) {
	      cerr << nb_line << endl;
	      cerr << ERR_OutRequest << endl;
	      cerr << line + "est ignoré." << endl;
	    }
	    else {
	      if (!mot.compare (KW_unobservation) && vmots.size () >= 2) {
		Request *q = this->getCurrentRequest ();
		Unobservation unobs (vmots[1]);
		unobs.insertLine (nb_line);
		q->insertAction (unobs);
	      }
	      else if (!mot.compare (KW_interrogation) && vmots.size () >= 2) {
		Request *q = this->getCurrentRequest ();
		Interrogation interro (vmots[1]);
		interro.insertLine (nb_line);
		q->insertAction (interro);
	      }
	      else if (!vmots[1].compare ("=")&& vmots.size () >= 3){
		Request *q = this->getCurrentRequest ();
		Observation obs (mot);
		obs.setValue (vmots[2]);
		obs.insertLine (nb_line);
		q->insertAction (obs);
	      }
	      else{
		this->setExitStatus(true);
		line_error = nb_line;
	      }
	   }
	  }
	  i += vmots.size ();
	}
      }
    }
    }
    fichier.close ();
}

string Session::getPackageSystemName(string systemNameInRequest){
 TokenizableString str(getPackageName()); 
 vector<string> elts = str.strtoken(".;");
 string repertory = elts[0];
 for (int i=1;i<elts.size()-1;i++){
   repertory += "/";
   repertory += elts[i];
 }
 repertory += ".";
 repertory += systemNameInRequest;
 return repertory;
}

string Session::getPackageSystemFilename(string systemImport){
 TokenizableString str(getPackageName()); 
 vector<string> elts = str.strtoken(".;");
 string repertory = elts[0];
 for (int i=1;i<elts.size()-1;i++){
   repertory += "/";
   repertory += elts[i];
 }
 repertory += "/";
 repertory += systemImport;
 repertory += ".skool";
 return repertory;
}
/******************************************************************/

void Session::act (string requestFile, ostream& o, bool option) {
  time_t temps_act;
  time(&temps_act);
  gum::Timer t;
  o << "Date of parsing : "<< ctime(&temps_act)<<endl;
      //type inférence choisir
  TypeEngine type = this->getEngine().getType();
  string packageSystemFilename = getPackageSystemFilename(getImports().at(0).getSystemName());
  gum::prm::skool::SkoolReader reader; 

      try {
 
	reader.readFile (packageSystemFilename);

      } catch (gum::Exception & e) {
	std::cerr << e.getContent () << std::endl;
	std::cerr << e.getCallStack () << std::endl;
	return;
      }

    
  for (int i = 0; i < this->getRequestsSize (); i++) {

      t.reset();
      Request r = this->getRequests ().at (i);
      string request = r.getRequestName ();
      TokenizableString str (request);
      vector < string > elements = str.strtoken ("{");
      o << requestFile << ":"<<r.getLine()<<": "<<elements[0]<<endl<<endl;
     
      string firstAction = r.getActions().at(0).getNode_path();
      TokenizableString strName(firstAction);
      vector<string> elts = strName.strtoken(".");
      string systemName = elts[0];
      
    
      
      gum::prm::PRMInference* inf = 0;
      //on ne peut charger un système à la fois
      string sys = getPackageSystemName(systemName);
      switch(type){	 
	case 0 :{
	  gum::prm::PRM *prm = reader.prm();
	  gum::prm::GroundedInference* grd = new gum::prm::GroundedInference(*prm, prm->getSystem(sys));
	  gum::BayesNet<gum::prm::prm_float>* bn = new gum::BayesNet<gum::prm::prm_float>();
	  gum::BayesNetFactory<gum::prm::prm_float> bn_factory(bn);
	  prm->getSystem(sys).groundedBN(bn_factory);
	  gum::VariableElimination<gum::prm::prm_float>* ve = new gum::VariableElimination<gum::prm::prm_float>(*bn);
	  grd->setBNInference(ve);
	  inf = grd;
 


	  for (int j = 0; j < r.getActionsSize (); j++) {
	    Action *my_act = &r.getActions ().at (j);
	    try{
	      my_act->write(prm, inf,sys,requestFile,o,option);
	    }catch (const exception & e) {			     
		  cerr << e.what () << endl;
	    }
	  }
	  break;
	}
	case 1 :{
	  gum::prm::PRM *prm = reader.prm();
	  inf = new gum::prm::SVE(*prm, prm->getSystem(sys));
	  for (int j = 0; j < r.getActionsSize (); j++) {
	    Action *my_act = &r.getActions ().at (j);
	    try{
	    my_act->write(prm, inf, sys,requestFile,o,option);
	    }catch (const exception & e) {
	  	  cerr << e.what () << endl;
	  	}
	  }
	  break;
	}
	case 2 :{
	  gum::prm::PRM *prm = reader.prm();
	  inf = new gum::prm::SVED(*prm, prm->getSystem(sys));

	  for (int j = 0; j < r.getActionsSize (); j++) {
	    Action *my_act = &r.getActions ().at (j);
	    try{
	    my_act->write(prm, inf,sys, requestFile,o,option);
	    }catch (const exception & e) {
	  	  cerr << e.what () << endl;
	  	}
	  }
	  break;
	}
      
      }
    
      	  //nettoyage du mémoire
      o << requestFile << ":"<<r.getLine()<<": "<<elements[0]<<": processing "<< t.step()<<endl<<endl;
      delete reader.prm();
  }
}

