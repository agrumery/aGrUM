/*
** Date : 10 April 2010
** Author : Ni NI           
*/

#include "Session.h"
#include "Request.h"
#include <string>
#include <cstring>
#include <stdlib.h>
#define KW_format ".skoor"
#define ERR_Syntax "The syntax is not correct."


int main (int argc, char *argv[]) {
  Session ses;
  bool option_comment = false;
  bool option_outputfile = false;
  char nomfichier[30];
  char nomfichiersortie[30];
  string first_arg;
  string second_arg;
  if(argc>1)
    first_arg = argv[1];
  if(argc>2)
    second_arg = argv[2];

  if (argc == 3 && first_arg=="-v"){
    option_comment = true;
    strcpy(nomfichier,argv[2]);
  }else if(argc==2 && first_arg!="-v")
      strcpy(nomfichier,argv[1]);
  else if(argc==5 && ((first_arg=="-v"&&second_arg=="-o")||(first_arg=="-o"&&second_arg=="-v"))){
      option_comment = true;
      strcpy(nomfichier, argv[3]);
      strcpy(nomfichiersortie, argv[4]);
      option_outputfile = true;
  }else if(argc==4 && first_arg == "-o"){
      strcpy(nomfichier, argv[2]);
      strcpy(nomfichiersortie,argv[3]);
      option_outputfile = true;
  } else {
    cerr << "The command syntax of prm_run is as following : " << endl;
    cerr << "prm_run [filename of request file] : print without comments." << endl;
    cerr << "prm_run [-v] [filename of request file] : print with comments." << endl;
    cerr << "prm_run [-o] [filename of request file] [filename of output file] : write in output file without comments." << endl;
    cerr << "prm_run [-v] [-o] [filename of request file] [filename of output file] : write in output file with comments."<<endl;
    return 1;
  }    
  // SI nomfichier ne contient pas de '.' alors ajouter KW_format
  if (strchr(nomfichier,'.')==NULL){
    strcat (nomfichier, KW_format);}
  
  /****Lecture et remplir les tableaux des indices des mots clés****/ 
 // if(  ses.FileExists(nomfichier))
  ses.parser (nomfichier);
  /*else {
    cerr<<ERR_File<<endl;
    exit(1);
  }*/
  /****Sortie console ou fichier************************************/
    string requestFile = nomfichier;
  if(!ses.getExitStatus()){

  if(option_outputfile){
    ofstream myfile(nomfichiersortie);
   if(myfile.is_open())
     ses.act(requestFile, myfile, option_comment);
   else
     cerr<<"Error : "<<myfile<<" can not be opened."<<endl;
  }
  else
    ses.act(requestFile, cout, option_comment);
  }
  else
    cerr <<requestFile<<":"<<ses.line_error<<": "<<ERR_Syntax<<endl;
  return 0;
}
