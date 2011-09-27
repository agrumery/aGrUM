/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Source implementation of MCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Ariele Maesano
 *
 */
// ============================================================================
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include "MCBayesNetGenerator.h"
// ============================================================================
namespace gum {

// Default constructor.
// Use the SimpleCPTGenerator for generating the BNs CPT.
template<typename T_DATA>
MCBayesNetGenerator<T_DATA>::MCBayesNetGenerator(Size nbNodes, Size max_modality, Size max_parents, double max_density):
        __cptGenerator(new SimpleCPTGenerator()) {
    c = 0;
    __bayesNet = 0;
    __nbNodes = nbNodes;
    if (max_density < (double)(nbNodes - 1) / (double)(nbNodes * nbNodes ))
        GUM_ERROR(gum::Exception,"max density too low for complete connection");
    __max_parents = max_parents;
    __max_density = max_density;
    __max_modality = max_modality;
    GUM_CONSTRUCTOR(MCBayesNetGenerator);
}

// Use this constructor if you want to use a different policy for generating
// CPT than the default one.
// The cptGenerator will be erased when the destructor is called.
// @param cptGenerator The policy used to generate CPT.
template<typename T_DATA>
MCBayesNetGenerator<T_DATA>::MCBayesNetGenerator( CPTGenerator* cptGenerator,Size nbNodes ,Size max_modality, Size max_parents, double max_density):
        __cptGenerator(cptGenerator) {
    c = 0;
    __bayesNet=0;
    __nbNodes = nbNodes;
    if (max_density < (double)(__nbNodes - 1) / (double)(__nbNodes * __nbNodes )) GUM_ERROR(gum::Exception,"max density too low for complete connection");
    __max_parents = max_parents;
    __max_density = max_density;
    __max_modality = max_modality;
    GUM_CONSTRUCTOR(MCBayesNetGenerator);
}

// Destructor.
template<typename T_DATA>
MCBayesNetGenerator<T_DATA>::~MCBayesNetGenerator() {
    GUM_DESTRUCTOR(MCBayesNetGenerator);
    delete __cptGenerator;
}

template<typename T_DATA>
BayesNet<T_DATA>* MCBayesNetGenerator<T_DATA>::generateBN(gum::Idx iteration, gum::Idx p,gum::Idx q) {
    __bayesNet = new BayesNet<T_DATA>();
      GUM_TRACE("createDAG");
    __createTree(__nbNodes);
    __transformPoly(__nbNodes/2);
      GUM_TRACE("modification");
    __PMMx_poly(iteration, p, q);

    for ( NodeId iter = 0.; iter < __bayesNet->size(); iter++ ) {
        __cptGenerator->generateCPT ( __bayesNet->cpt ( iter ).pos ( __bayesNet->variable ( iter ) ), __bayesNet->cpt ( iter ) );
    }
    return __bayesNet;
}
// density represent de
template<typename T_DATA>
BayesNet<T_DATA>* MCBayesNetGenerator<T_DATA>::modifyingExistingBN(BayesNet<T_DATA> * bayesNetinit, gum::Idx iteration, gum::Idx p,gum::Idx q) {
    __bayesNet = new BayesNet<T_DATA>(bayesNetinit);
    __PMMx_poly(iteration, p, q);
    for ( NodeId iter = 0.; iter < __bayesNet->size(); iter++ ) {
        __cptGenerator->generateCPT ( __bayesNet->cpt ( iter ).pos ( __bayesNet->variable ( iter ) ), __bayesNet->cpt ( iter ) );
    }
    return __bayesNet;
}



template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__PMMx_poly(gum::Idx iteration, gum::Idx p, gum::Idx q) {
        if (!iteration)
        return;
    srand(time(NULL) + c++);
    gum::Idx per = rand() % 100;
    if (per < p) {
        __AorR();
        if (!__isPolytree()) __PMMx_multi(--iteration,p,q);
        else __PMMx_poly(--iteration,p,q);//
    }
    else {
        if (per < p + q) {
            __AR();
            __PMMx_poly(--iteration,p,q);

        }
        else {
            __jump_poly();
            if (!__isPolytree()) __PMMx_multi(--iteration,p,q);
            else __PMMx_poly(--iteration,p,q);

        }
    }
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__PMMx_multi(gum::Idx iteration, gum::Idx p,gum::Idx q) {
    if (!iteration)
        return;
    srand(time(NULL) + c++);
    gum::Idx per = rand() % 100;
    if (per < p + q) {
        BayesNet<T_DATA> * newbayes = new BayesNet<T_DATA>(*__bayesNet);
        __AorR();
        if (__isPolytree()) {
            if (per > p) {

                delete __bayesNet;
                __bayesNet= newbayes;

                __PMMx_poly(--iteration,p,q);
            }
            else delete newbayes;
        }
        else {
            delete newbayes;
            __PMMx_multi(--iteration,p,q);
        }
    }
    else {
        __jump_multi();
        if (!__isPolytree())__PMMx_multi(--iteration,p,q);
        else __PMMx_poly(--iteration,p,q);
    }
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__jump_poly() {
    srand(time(NULL) + c++);
    gum::NodeId i,j;
    __choosenodes(i,j);
    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(i,j) || __dag.existsArc(j,i)) {
        return;
    }
    else
        if (!__directedPath(j,i) &&
                (!__max_parents || ((__dag.parents(j).size() + 1) <= __max_parents)) &&
                ((double)(__dag.sizeArcs() + 1) /(double)(__dag.size()*__dag.size()) <= __max_density)) {
            __bayesNet->insertArc(i,j);

        }
}


template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__jump_multi() {
    srand(time(NULL)+ c++);
    gum::NodeId i,j;
    __choosenodes(i,j);
    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(i,j) && __dag.parents(j).size() > 1) {
        __bayesNet->eraseArc(i,j)
        ;
    }
    else
        if (__dag.existsArc(j,i)&& __dag.parents(i).size() > 1) {
            __bayesNet->eraseArc(j,i)
            ;

        }
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__AorR() {
    srand(time(NULL) + c++);
    gum::NodeId i,j;
    __choosenodes(i,j);
    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(i,j)&& __dag.parents(j).size() > 1) {
        __bayesNet->eraseArc(i,j);
    }
    else {
            if (!__dag.existsArc(i,j)&&!__directedPath(j,i) &&
                (!__max_parents || __dag.parents(j).size() + 1 <= __max_parents) &&
                (double)(__dag.sizeArcs() + 1) /(double)(__dag.size()*__dag.size()) < __max_density) {
            __bayesNet->insertArc(i,j);
        }
    }
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__AR() {
    srand(time(NULL) + c++);
    gum::NodeId i, j;
    __choosenodes(i,j);
    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(i,j) || __dag.existsArc(j,i)) {
        return;
    }
    else {
        gum::Idx per = rand() % 100;
        gum::Idx temp = j;
        if (per < 50) {
            j = i;
            i = temp;
        }
        if (__max_parents && __dag.parents(j).size() + 1 > __max_parents ) return;

        if (__directedPath(i,j)) {
            for (gum::NodeSetIterator it =__dag.parents(j).begin(); it != __dag.parents(j).end(); ++it )
                if (__directedPath(i,*it)) {
                    __bayesNet->eraseArc(*it,j);
               }
        }
        else if (__directedPath(j,i)) {
           for (gum::NodeSetIterator it =__dag.parents(i).begin(); it != __dag.parents(i).end(); ++it )
                if (__directedPath(j,*it)) {
                    __bayesNet->eraseArc(*it,i);

                }
        }
        else if (__dag.parents(j).size()) __bayesNet->eraseArc(*(__dag.parents(j).begin()),j);
        else if (__dag.parents(i).size()) __bayesNet->eraseArc(*(__dag.parents(i).begin()),i);

        try {
            __bayesNet->insertArc(i,j);
        }catch (gum::InvalidCircuit) {
            std::cout << "ins7 i "<< __bayesNet->variable(i).name() <<"  j " << __bayesNet->variable(j).name()
                      << " i parents :"<< __dag.parents(i).size()<< " j parents :"<< __dag.parents(j).size()
                      << " connected :"<< __connect(i,j)<< " dir i j   :"<< __directedPath(i,j)<< " dir j i  : "<< __directedPath(j,i)
                      << " is polytree " << __isPolytree()<<std::endl;
            std::cerr << __bayesNet->toDot()<< std::endl;
            exit(666);
        }
    }
}

template<typename T_DATA> INLINE
void MCBayesNetGenerator<T_DATA>::__choosenodes(gum::NodeId &i,gum::NodeId &j) {
    srand(time(NULL)+ c++);
    i = rand() % __bayesNet->size();
    j = rand() % __bayesNet->size();
    while (i==j)
        j = rand() % __bayesNet->size();
}

template<typename T_DATA> INLINE
void MCBayesNetGenerator<T_DATA>::__chooseclosenodes(gum::NodeId &i,gum::NodeId &j) {
    srand(time(NULL)+ c++);
    gum::Idx temp = rand() % __bayesNet->size();
    gum::Size co = 0;
    if(__bayesNet->dag().parents(temp).size()){ 
      j = temp;
      NodeSetIterator it = __bayesNet->dag().parents(j).begin();
      co = rand() % __bayesNet->dag().parents(j).size();
      while(co--){
      ++it;
      }
    i = *it;
    }
    else if(__bayesNet->dag().children(temp).size()){ 
      i = temp; 
       NodeSetIterator it = __bayesNet->dag().children(i).begin();
      co = rand() % __bayesNet->dag().children(i).size();
      while(co--){
      ++it;
      }
      
      j = *it;
    }
    else GUM_ERROR( FatalError, "Sorry Misconstruction BN isolated node." )
  
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__createTree(gum::Size BNSize) {
    static Idx n = 0;
    srand(time(NULL) + n);
        gum::Idx nb_mod = (__max_modality == 2) ? 2 : 2 + rand() % ( __max_modality - 1);
        std::stringstream strBuff;
        strBuff << "n_1_" <<n++;
          NodeId root = __bayesNet->addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod));
    Size maxNodes = BNSize - 1;
    gum::Size SubG = 0;
    while (maxNodes) {
        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        gum::NodeId rootS = __createPartTree(SubG);
        __bayesNet->insertArc( root,rootS);
        }
}

template<typename T_DATA>
NodeId MCBayesNetGenerator<T_DATA>::__createPartTree(gum::Size BNSize) {
    static Idx n = 0;
    srand(time(NULL) + n);
        gum::Idx nb_mod = (__max_modality == 2) ? 2 : 2 + rand() % ( __max_modality - 1);
        std::stringstream strBuff;
        strBuff << "n_2_" <<n++;
          NodeId root = __bayesNet->addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod));
    Size maxNodes = BNSize - 1;
    gum::Size SubG = 0;
    while (maxNodes) {

        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        gum::NodeId rootS = __createPartTree(SubG);
        __bayesNet->insertArc( root,rootS);
        }
        return root;
}

template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__transformPoly(gum::Idx nbiter){
  while(nbiter--){
    gum::NodeId i,j;
    __chooseclosenodes(i,j);
    if (__bayesNet->dag().parents(i).size() + 1 <= __max_parents){
    __bayesNet->eraseArc(i,j);
    __bayesNet->insertArc(j,i);
    }
    else nbiter++;
 
    
  }
}


template<typename T_DATA>
void MCBayesNetGenerator<T_DATA>::__createDAG(gum::Size BNSize, gum::Size iniRoot) {
    static Idx n = 0;
    srand(time(NULL) + n);
    gum::Size nbRoot = (BNSize <= iniRoot ?(rand() % min(BNSize , __max_parents) ) + 1: (rand() %  __max_parents) + 1 );
    std::vector<gum::NodeId> roots;
    for (Idx r = 0 ; r < nbRoot; r++) {
        gum::Idx nb_mod = (__max_modality == 2) ? 2 : 2 + rand() % ( __max_modality - 1);
        std::stringstream strBuff;
        strBuff << "n" <<n++<<"_"<<r;
        roots.push_back(__bayesNet->addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod)));
    }
    Size maxNodes = BNSize - min( nbRoot , BNSize);
    gum::Size SubG = 0;
    while (maxNodes) {

        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        std::vector<gum::NodeId> * rootS = __createPartDAG(SubG, nbRoot);
        for (std::vector<gum::NodeId>::iterator it = roots.begin();it != roots.end(); ++it) {
            gum::Idx theroot = rand() % rootS->size() ;

            __bayesNet->insertArc( *it, (*rootS)[theroot]);
        }
        if (rootS->size() == SubG) {
            for (std::vector<gum::NodeId>::iterator it = rootS->begin();it != rootS->end(); ++it) {
                if (!__bayesNet->dag().parents(*it).size()) {
                    gum::Idx theroot = rand() % roots.size();
                    __bayesNet->insertArc(roots[theroot], *it);
                }
            }
        }
        delete rootS;
    }
    if (!__isPolytree()) {std::cout << "it is not a polytree" << std::endl;GUM_ERROR( FatalError, "Sorry Misconstruction BN is no Polytree." );}
}

template<typename T_DATA>
std::vector<gum::NodeId> * MCBayesNetGenerator<T_DATA>::__createPartDAG(gum::Size BNSize, gum::Size iniRoot) {
    static Idx n = 0;
    srand(time(NULL) + n);
    gum::Size nbRoot = (BNSize <= iniRoot ?(rand() % min(__max_parents, BNSize)) + 1: (rand() % __max_parents) + 1 );//################
    std::vector<gum::NodeId> * roots = new std::vector<gum::NodeId> ;
    for (Idx r = 0 ; r < nbRoot; r++) {
        gum::Idx nb_mod = (__max_modality == 2) ? 2 : 2 + rand() % ( __max_modality - 1);
        std::stringstream strBuff;
        strBuff << "nA" <<n++<<"_"<<r;
        roots->push_back(__bayesNet->addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod)));
    }
    Size maxNodes = BNSize - min( nbRoot , BNSize);
    gum::Size SubG = 0;
    while (maxNodes) {
        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        std::vector<gum::NodeId> * rootS = __createPartDAG(SubG, nbRoot);
        for (std::vector<gum::NodeId>::iterator it = roots->begin();it != roots->end(); ++it) {
            gum::Idx theroot = rand() % rootS->size() ;
            __bayesNet->insertArc( *it, (*rootS)[theroot]); 
        }
        if (rootS->size() == SubG) {
            for (std::vector<gum::NodeId>::iterator it = rootS->begin();it != rootS->end(); ++it) {
                if (!__bayesNet->dag().parents(*it).size()) {
                    gum::Idx theroot = rand() % roots->size();
                    __bayesNet->insertArc((*roots)[theroot], *it);
                }
            }
        }
        delete rootS;
    }
    return roots;
}


template<typename T_DATA>
bool MCBayesNetGenerator<T_DATA>::__isPolytree() {
    const gum::DAG __dag = __bayesNet->dag();
    return __bayesNet->size() - 1 == __bayesNet->nbrArcs();
}

template<typename T_DATA>
bool MCBayesNetGenerator<T_DATA>::__connect(const gum::NodeId i, const gum::NodeId j) {
    NodeSet excluded;
    return __connect(i,j,excluded);
}

template<typename T_DATA>
bool MCBayesNetGenerator<T_DATA>::__connect(const gum::NodeId i, const gum::NodeId j, gum::NodeSet &excluded) {
    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(i,j) || __dag.existsArc(j,i))
        return true;
    else {
        excluded.insert(i);
        const gum::NodeSet set_parent = __dag.parents(i);

        for (gum::NodeSetIterator par = set_parent.begin(); par != set_parent.end(); ++par) {
            if (!excluded.exists(*par)&&__connect( *par,j,excluded))
                return true;
        }
        const gum::NodeSet  set_children = __dag.children(i);
        for (gum::NodeSetIterator par = set_children.begin(); par != set_children.end(); ++par) {
            if (!excluded.exists(*par)&& __connect(*par,j,excluded))
                return true;
        }
        return false;
    }
}

template<typename T_DATA>
bool MCBayesNetGenerator<T_DATA>::__directedPath(const gum::NodeId tail, const gum::NodeId head) {

    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(tail,head))
        return true;
    else {
        const gum::NodeSet  set_children = __dag.children(tail);
        for (gum::NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node) {
            if (__directedPath(*node,head))
                return true;
        }
        return false;
    }
}

template<typename T_DATA>
bool MCBayesNetGenerator<T_DATA>::__directedPath(const gum::NodeId tail, const gum::NodeId head, gum::NodeSet &excluded) {

    const DAG __dag = __bayesNet->dag();
    if (__dag.existsArc(tail,head))
        return true;
    else {
        const gum::NodeSet  set_children = __dag.children(tail);
        excluded.insert(tail);
        for (gum::NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node) {
            if (!excluded.exists(*node)&&__directedPath(*node,head,excluded))
                return true;
        }
        return false;
    }
}


} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

