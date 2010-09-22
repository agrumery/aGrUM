#include "Action.h"
#include <iostream>
#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/groundedInference.h>
#include <agrum/BN/BayesNetFactory.h>
#include <agrum/BN/inference/variableElimination.h>

#include <agrum/prm/groundedInference.h>
#include <agrum/prm/SVE.h>
#include <agrum/prm/SVED.h>
#include "SessionException.cpp"

using namespace std;

Action::Action (string node_path)
{
  this->node_path = node_path;
  type = tUnknown;
}

Action::Action ()
{
}

Action::~Action ()
{
  node_path.erase (node_path.begin (), node_path.end ());
}

int
Action::getLine ()
{
  return line;
}

void
Action::insertLine (int line)
{
  this->line = line;
}

string Action::getNode_path ()
{
  return node_path;
}

string Action::getValue ()
{
  return value;
}

void
Action::setValue (string value)
{
  this->value = value;
}

/*********************************************************/
/****sorti dans un terminal ou dans un fichier************/
/*********************************************************/
void
Action::write (PRM *prm, PRMInference* inf, string systemName, string requestFile, ostream & os, bool option)
{
  TokenizableString str (this->getNode_path ());
  vector < string > elements = str.strtoken (".;");
  int nb_elts = elements.size ();
  switch (getType ()) {
    case tObservation:
      {
        //on averti si la variable a déjà été observé
       
        gum::prm::System & syst = prm->getSystem (systemName);
        if (inf->hasEvidence (std::make_pair(&(syst.get(elements[nb_elts - 2])), &(syst.get(elements[nb_elts - 2]).get(elements[nb_elts - 1])))))  {
          cerr << "Il y a déjà une observation !" << endl;
        } else {
          TokenizableString str_v (getValue ());
          vector < string > eltValue = str_v.strtoken (";");

          gum::prm::PRMInference::Chain chain = std::make_pair(&(syst.get(elements[nb_elts - 2])),
                                                               &(syst.get(elements[nb_elts - 2]).get(elements[nb_elts - 1])));
          gum::Potential < gum::prm::prm_float > e;
          const gum::DiscreteVariable& var = chain.second->type().variable();
          e.add(var);
          gum::Instantiation i(e);
          for (i.setFirst(); not i.end (); i.inc()) {
            if (var.label(i.val(var)) == eltValue[0]) {
	 
              e.set(i, (gum::prm::prm_float) 1.0);
            } else {
	    
              e.set(i, (gum::prm::prm_float) 0.0);
            }
          }
          //ajout observation dans l'inference
          try {
            inf->addEvidence(chain, e);
	    //affichage en demande
	  if (option) {
	    os << "  # " << requestFile << ":" << getLine () << ": ";
	    os << elements.at (0) << "/";
          if (nb_elts > 2) {
            for (int k = 1; k < nb_elts - 2; k++) {
              os << elements.at (k) << "/";
            }
            os << elements.at (nb_elts - 2);
          } else {
            os << elements.at (1);
          }
          os << " : " << elements.at (nb_elts - 1);
          TokenizableString str_v (getValue ());
          vector < string > eltValue = str_v.strtoken (";");
          os << " <- " << eltValue[0] << endl;
        }



          } catch (gum::OperationNotAllowed & ex) {
            cerr << ex.getContent () << endl;
          }
        }
      
        break;
      }//fin observation
    case tUnobservation:
      {
        gum::prm::System & syst = prm->getSystem (systemName);
        TokenizableString str_v (getValue ());
        vector < string > eltValue = str_v.strtoken (";");
	gum::prm::PRMInference::Chain chain = std::make_pair(&(syst.get (elements[nb_elts - 2])),	// on récupère l'instance
							     &(syst.get(elements[nb_elts - 2]).get(elements[nb_elts - 1])));
        if (inf->hasEvidence(chain)) {
          try {
            inf->removeEvidence (chain);
	    if (option) {
		os << "  # " << requestFile << ":" << getLine () << ": ";
		os << elements.at (0) << "/";
		if (nb_elts > 2) {
		  for (int k = 1; k < nb_elts - 2; k++) {
		    os << elements.at (k) << "/";
		  }
		  os << elements.at (nb_elts - 2);
		} else {
		  os << elements.at (1);
		}
		os << " : " << elements.at (nb_elts - 1) << " unobserved" << endl;
	    }
          } catch (gum::OperationNotAllowed & ex) {
            cerr << ex.getContent () << endl;
          }
        } else {
          cerr << "The variable hasn't been observed : " << getLine () << endl;
        }
        break;
      }
    case tInterrogation:
      {
     
        gum::prm::System & syst = prm->getSystem (systemName);
        gum::prm::PRMInference::Chain chain = std::make_pair(&(syst.get (elements[nb_elts - 2])),	
                                                             &(syst.get (elements[nb_elts - 2]).get(elements[nb_elts - 1])));
        gum::Potential < gum::prm::prm_float > m;
        inf->marginal(chain, m);
        gum::Instantiation j (m);
        for (j.setFirst (); not j.end (); j.inc ()) {
          os << j << ": " << m.get (j) << endl;
        }
        	  os << endl;
        break;
      }
    default:
      {
        throw (SessionException (Err_unknownAction, this->getNode_path ()));
        break;
      }
  }
}
