/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN & Ariele-Paolo MAESANO   *
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
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 *
 */
// ============================================================================
#include <agrum/BN/generator/MCBayesNetGenerator.h>

// ============================================================================


template<typename T_DATA>
gum::Size getMaxModality(gum::BayesNet<T_DATA> & bayesNet){
  gum::Size maxMod = 0;
  gum::DAG::NodeIterator node = bayesNet.beginNodes();
        while (node != bayesNet.endNodes()){
          if (maxMod < bayesNet.variable(*node).domainSize() )
            maxMod = bayesNet.variable(*node).domainSize();
         ++node; 
        }
        return maxMod;
}

namespace gum {

// Default constructor.
// Use the DefaultCPTGenerator for generating the BNs CPT.
template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::MCBayesNetGenerator( Size nbrNodes, Size maxArcs, Idx maxModality, Size iteration ,Idx p,Idx q): 
AbstractBayesNetGenerator<T_DATA,ICPTGenerator>(nbrNodes, maxArcs ,maxModality),_bayesNettemp(),_hashMarginal(){
  if(p+q > 100) GUM_ERROR(OperationNotAllowed,"the sum of the probabilities p and q must be at most equal to 100");
    _c = 0;
    _iteration =iteration;
    _p = p;
    _q = q;
    _disturbing = false;

    GUM_CONSTRUCTOR(MCBayesNetGenerator);
}

template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::MCBayesNetGenerator( BayesNet<T_DATA> bayesNet, Size iteration ,Idx p,Idx q): 
AbstractBayesNetGenerator<T_DATA,ICPTGenerator>(bayesNet.size(), (Size) (bayesNet.nbrArcs() * 1.1) ,getMaxModality(bayesNet)),_bayesNettemp(),_hashMarginal(){

    _c = 0;
    _iteration =iteration;
    _p = p;
    _q = q;
    _disturbing = false;

    GUM_CONSTRUCTOR(MCBayesNetGenerator);
}
// Destructor.
template<typename T_DATA, template<class> class ICPTGenerator , template <class> class ICPTDisturber> 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::~MCBayesNetGenerator() { 
    GUM_DESTRUCTOR(MCBayesNetGenerator);

}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::generateBN(BayesNet<T_DATA> & bayesNet){


  Idx iteration = _iteration;
    
   AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = bayesNet;
    __createTree(AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_nbrNodes);
    __transformPoly(AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_nbrNodes/2);
    _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet ;
    __PMMx_poly();

   AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::fillCPT();
    _iteration = iteration;

    bayesNet = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet;

}


// density represent de
template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::disturbBN(BayesNet<T_DATA> & bayesNetinit, Size iteration) {// insert option for the variation
 _disturbing = true;
   Size iter = _iteration;
   if(iteration) _iteration = iteration;
    AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = bayesNetinit;
    if(__checkConditions()){ 
      LazyPropagation<T_DATA> inf(bayesNetinit);
      inf.makeInference();
      for (DAG::NodeIterator it = bayesNetinit.beginNodes(); it != bayesNetinit.endNodes(); ++it){
       Potential<T_DATA> * pottemp = new Potential<T_DATA>();
       pottemp->copy(inf.marginal(*it));
       _hashMarginal.insert(*it, pottemp); 
        
      }
      _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet ;
      if(__isPolytree())__PMMx_poly();
      else __PMMx_multi();
      
      bayesNetinit = (AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet); 
      
      while(_hashMarginal.size()){ 
          delete (*(_hashMarginal.begin()));
         _hashMarginal.erase(_hashMarginal.begin());
         
       }

    }
    else { 
      std::cout << AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.toDot()  << std::endl;
      GUM_ERROR( OperationNotAllowed, "BN is not valid cause it does not respect constraint " );
    }
     _iteration = iter;
    _disturbing = false;

}

template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
 bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__checkConditions(){

        return AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_maxArcs >= AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().sizeArcs();    
   
}

//main algorithme for moving between state of the BayesNet according on the nature of the topology polytree or multi-connected

template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__PMMx_poly() {
    if (!_iteration--)
        return;

    srand(time(NULL) + _c++);
    Idx per = rand() % 100;
    if (per < _p) {
        __AorR();
        if (__checkConditions()) {_bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet; __PMMx_multi();}
        else {

          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = _bayesNettemp ; 
          __PMMx_poly();}
    }
    else {
        if (per < _p + _q) {
            __AR();
             if (!__checkConditions()) {

               AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet =_bayesNettemp;}
             else _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet;
            __PMMx_poly();
        }
        else {
            __jump_poly();
            if  (__checkConditions()){
             _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet ;
              __PMMx_multi();
              
            }
            else {
             AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet =   _bayesNettemp;
             __PMMx_poly();
              
            }
        }
    }
  
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__PMMx_multi() {

    if (!_iteration--)
        return;

    srand(time(NULL) + _c++);
    Idx per = rand() % 100;
    if (per < _p + _q) {
        __AorR();
          if (__checkConditions()) {
           if (__isPolytree()) {
                if (per < _p)  {_bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet ; __PMMx_poly(); }
               else {AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = _bayesNettemp; __PMMx_multi();}
           }
           else {_bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet; __PMMx_multi();}
        }
        else {
         
          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = _bayesNettemp; 
          __PMMx_multi();
      }
    }
        else {  __jump_multi();
             if (__checkConditions()) {
               _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet;
               if(__isPolytree())
                 __PMMx_poly(); 
               else 
                 __PMMx_multi();//TODO verification required
               
            }
             else {
               AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = _bayesNettemp ;
               __PMMx_multi();
            }
        }
        
}



template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__AorR() {
    srand(time(NULL) + _c++);
    NodeId i,j;
    __chooseNodes(i,j);
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(i,j)) {
        __eraseArc(i,  j);

        return;  
    }
    else __insertArc( i,  j);
        
    }


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__AR() {

    srand(time(NULL) + _c++);
    NodeId i, j,head,tail;
    __chooseNodes(i,j);
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(i,j) || __dag.existsArc(j,i)) {
        return;
    }
    else {
        Idx per = rand() % 100;
        if (per < 50) {
            head = i;
            tail = j;
        }
        else {
          head = j;
        tail = i;
        }


        for (NodeSetIterator it = __dag.parents(j).begin(); it != __dag.parents(j).end(); ++it) {
          NodeSet excluded;
          excluded.insert(j);
         if(__connect(*it, i, excluded)){
           std::string nameit = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.variable(*it).name();
          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.eraseArc(*it,j);//TODO reflect 
          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(head,tail);return;
         }
        }

         for (NodeSetIterator it = __dag.children(j).begin(); it != __dag.children(j).end(); ++it) {
           NodeSet excluded;
           excluded.insert(j);
         if(__connect(*it, i, excluded)){ 
           std::string nameit = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.variable(*it).name();
          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.eraseArc(j, *it);
          AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(head,tail);return;
         }
        }
    }
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__jump_poly() {
    srand(time(NULL) + _c++);
    NodeId i,j;
    __chooseNodes(i,j);
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (!__dag.existsArc(i,j))
        __insertArc(i, j);
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__jump_multi() {
    srand(time(NULL)+ _c++);
    NodeId i,j;
    __chooseNodes(i,j);
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(i,j)){
        __eraseArc(i, j);

}
}




template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__insertArc(NodeId i, NodeId j) {
  if(__directedPath(j,i)) return;
  if (_disturbing) { 
      Potential<T_DATA> potj;
      potj = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.cpt(j);
      AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(i,j);

      disturbAugmCPT(i,j, AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet, potj,(T_DATA) 0.5);
    }
    else AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(i,j);
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__eraseArc(NodeId i, NodeId j, bool mustbeconnex) {
  if (_disturbing) {
    const BayesNet<T_DATA> bayesNet( AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet);
     Potential<T_DATA> potj;
     potj.copy(AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.cpt(j));
       AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.eraseArc(i,j);
       
       if(__connect(i,j) || ! mustbeconnex){
       Potential<T_DATA> marg;
       marg = (*_hashMarginal[i]);
      
       disturbReducCPT(i,j, AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet, potj, marg);
      }
       else AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(i,j);
    }
    else { AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.eraseArc(i,j);

      
    if(!__connect(i,j) && mustbeconnex){

        AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(i,j);

    }
    
    }
}





template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber >  INLINE
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__chooseNodes(NodeId &i,NodeId &j) {
    srand(time(NULL)+ _c++);
    i = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.size();
    j = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.size();
    while (i==j)
        j = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.size();
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber>
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__chooseCloseNodes(NodeId &i,NodeId &j) {
  srand(time(NULL)+ _c++);
    Idx temp = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.size();
    Size co = 0;
    if (AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().parents(temp).size()) {
        j = temp;
        NodeSetIterator it = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().parents(j).begin();
        co = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().parents(j).size();
        while (co--) {
            ++it;
        }
        i = *it;
    }
    else if (AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().children(temp).size()) {
        i = temp;
        NodeSetIterator it = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().children(i).begin();
        co = rand() % AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag().children(i).size();
        while (co--) {
            ++it;
        }

        j = *it;
    }
    else GUM_ERROR( FatalError, "Sorry Misconstructed BN because of isolated node." )

    }

    
template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__createTree(Size BNSize) {
    static Idx n = 0;
    srand(time(NULL) + n);
    Idx nb_mod = 2 + rand() % ( AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_maxModality - 1);
    std::stringstream strBuff;
    strBuff << "n_1_" <<n++;
    NodeId root = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod));
    Size maxNodes = BNSize - 1;
    Size SubG = 0;
    while (maxNodes) {
        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        NodeId rootS = __createPartTree(SubG);
        AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc( root,rootS);
    }
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
NodeId MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__createPartTree(Size BNSize) {
    static Idx n = 0;
    srand(time(NULL) + n);
    Idx nb_mod = 2 + rand() % ( AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_maxModality - 1);
    std::stringstream strBuff;
    strBuff << "n_2_" <<n++;
    NodeId root = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.addVariable(LabelizedVariable(strBuff.str(), "" , nb_mod));
    Size maxNodes = BNSize - 1;
    Size SubG = 0;
    while (maxNodes) {
        SubG = (rand() % (maxNodes)) + 1 ;
        maxNodes = maxNodes - SubG;
        NodeId rootS = __createPartTree(SubG);
        AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc( root,rootS);
    }
    return root;
}


// Allow to invert maximum nbiter arc to use from polytree only
template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__transformPoly(Idx nbiter) {
    while (nbiter--) {
        NodeId i,j;
        __chooseCloseNodes(i,j);
        _bayesNettemp = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet;
        __eraseArc(i,  j, false);
            AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.insertArc(j,i);
            if (!__checkConditions())
            AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet = _bayesNettemp; 
    }
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__isPolytree() {
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    return AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.size() - 1 == AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.nbrArcs();
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__connect(const NodeId i, const NodeId j) {
  const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
  if (__dag.existsArc(i,j) || __dag.existsArc(j,i))
        return true;
    else {

    NodeSet excluded;  
    excluded.insert(i);
        const NodeSet set_parent = __dag.parents(i);
        for (NodeSetIterator par = set_parent.begin(); par != set_parent.end(); ++par) {
            if (!excluded.exists(*par)&&__connect( *par,j,excluded))
                return true;
        }
        const NodeSet  set_children = __dag.children(i);
        for (NodeSetIterator par = set_children.begin(); par != set_children.end(); ++par) {
            if (!excluded.exists(*par)&& __connect(*par,j,excluded))
                return true;
        }
        return false;
    }

}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__connect(const NodeId i, const NodeId j, NodeSet &excluded) {
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(i,j) || __dag.existsArc(j,i))
        return true;
    else {
        excluded.insert(i);
        const NodeSet set_parent = __dag.parents(i);
        for (NodeSetIterator par = set_parent.begin(); par != set_parent.end(); ++par) {
            if (!excluded.exists(*par)&&__connect( *par,j,excluded))
                return true;
        }
        const NodeSet  set_children = __dag.children(i);
        for (NodeSetIterator par = set_children.begin(); par != set_children.end(); ++par) {
            if (!excluded.exists(*par)&& __connect(*par,j,excluded))
                return true;
        }
        return false;
    }
}


template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber > 
bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__directedPath(const NodeId tail, const NodeId head) {
    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(tail,head))
        return true;
    else {
      NodeSet excluded;
      excluded.insert(tail);
        const NodeSet  set_children = __dag.children(tail);
        
        for (NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node) {
            if (__directedPath(*node,head,excluded))
                return true;
        }
        return false;
    }
}

template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> 
bool MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__directedPath(const NodeId tail, const NodeId head, NodeSet &excluded) {

    const DAG __dag = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.dag();
    if (__dag.existsArc(tail,head))
        return true;
    else {
        const NodeSet  set_children = __dag.children(tail);
        excluded.insert(tail);
        for (NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node) {
            if (!excluded.exists(*node)&&__directedPath(*node,head,excluded))
                return true;
        }
        return false;
    }
}



      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      Size MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::iteration() const{
       return _iteration; 
      }
      
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      Idx MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::p() const{
        return _p;
      }
      
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      Idx MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::q() const{
        return _q;
      }
      
  
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::setIteration(Size iteration){
        _iteration = iteration;
      }
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::setP(Idx p){
       _p = p ;
       if(p+_q > 100) GUM_ERROR(OperationNotAllowed,"the sum of the probabilities p and q must be at most equal to 100");
      }
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      void MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::setQ(Idx q){
       _q = q;
       if(_p+q > 100) GUM_ERROR(OperationNotAllowed,"the sum of the probabilities p and q must be at most equal to 100");
      }


} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

