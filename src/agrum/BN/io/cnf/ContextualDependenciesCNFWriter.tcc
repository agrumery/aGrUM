/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#include <agrum/BN/io/cnf/ContextualDependenciesCNFWriter.h>

namespace gum {

  /* ============================================================================ */
  /* ===                           GUM_BN_WRITER                              === */
  /* ============================================================================ */
  // Default constructor.
  template<typename GUM_SCALAR, template <class> class IApproximationPolicy> INLINE
  ContextualDependenciesCNFWriter<GUM_SCALAR, IApproximationPolicy>::ContextualDependenciesCNFWriter() {
    GUM_CONSTRUCTOR ( ContextualDependenciesCNFWriter );
  }

  // Default destructor.
  template<typename GUM_SCALAR, template <class> class IApproximationPolicy> INLINE
  ContextualDependenciesCNFWriter<GUM_SCALAR, IApproximationPolicy>::~ContextualDependenciesCNFWriter() {
    GUM_DESTRUCTOR ( ContextualDependenciesCNFWriter );
  }

  //
  // Writes a Bayesian Network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian Network writen in output.
  // @throws Raised if an I/O error occurs.
  template<typename GUM_SCALAR, template <class> class IApproximationPolicy> INLINE
  void
  ContextualDependenciesCNFWriter<GUM_SCALAR, IApproximationPolicy>::write ( std::ostream& output, const IBayesNet<GUM_SCALAR>& bn ) {
    Instantiation Order;

    for ( gum::Sequence<gum::NodeId>::iterator it = bn.topologicalOrder().begin(); it != bn.topologicalOrder().end(); ++it )
      Order.add ( bn.variable ( *it ) );

    if ( ! output.good() )
      GUM_ERROR ( IOError, "Stream states flags are not all unset." );

    std::ofstream outputvar ( "provisoire.var", std::ios_base::trunc );

    std::stringstream strfile, strfile2;

    if ( ! outputvar.good() )
      GUM_ERROR ( IOError, "Stream states flags are not all unset." );


    Idx num = 0;
    Idx numvar = 0;
    Idx clause = 0;
    std::stringstream clausstr, clausstr2;
    gum::HashTable<std::string, Idx> vartable ;
    gum::HashTable<std::string, Idx> protable ;
    gum::HashTable<const gum::DiscreteVariable*, gum::HashTable<std::string, gum::Sequence< gum::Sequence<gum::Instantiation* >* >* >* > cptparamval;

    for ( const auto iter : bn.nodes() ) {
      std::stringstream str0 ;
      const DiscreteVariable* var = &bn.variable ( iter );

      for ( Idx i = 0; i < bn.variable ( iter ).domainSize(); i++ ) {
        std::stringstream stri ;
        stri << var->name() << "_" << var->label ( i );
        vartable.insert ( stri.str(), ++num );
        strfile << num << "::" << stri.str() << "\n";
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n"; clause++; numvar++;
      clausstr2 << str0.str();
      const Potential<GUM_SCALAR>& cpt = bn.cpt ( iter );
      Instantiation inst ( cpt ); inst.forgetMaster();
      inst.reorder ( Order );
      cptparamval.insert ( var, new gum::HashTable<std::string, gum::Sequence< gum::Sequence<gum::Instantiation* >* >* >() );

      for ( inst.setFirst(); ! inst.end(); ++inst ) {
        if ( this->fromExact ( cpt[inst] ) != 1 ) {
          std::stringstream strk;
          strk << this->fromExact ( cpt[inst] );
          std::string valp = strk.str();

          if ( ! ( cptparamval[var] )->exists ( valp ) ) {
            ( cptparamval[var] )->insert ( valp, new gum::Sequence<gum::Sequence<gum::Instantiation*>*>() );

            ( * ( cptparamval[var] ) ) [valp]->insert ( new gum::Sequence<gum::Instantiation*> );

            if ( this->fromExact ( cpt[inst] ) ) {
              std::stringstream strinst;
              strinst << var->name();
              strinst << "_val=" << this->fromExact ( cpt[inst] );

              if ( !protable.exists ( strinst.str() ) ) {
                protable.insert ( strinst.str() , ++num );
                strfile << num << "::" <<  strinst.str()  << "\n";
              }
            }
          }

          ( * ( cptparamval[var] ) ) [valp]->front()->insert ( new gum::Instantiation ( inst ) );
        }
      }
    }

    std::stringstream str2;

    while ( !cptparamval.empty() ) {
      gum::HashTable<const DiscreteVariable*, gum::HashTable<std::string, gum::Sequence<gum::Sequence<gum::Instantiation*>*>*>*>::iterator_safe itvar = cptparamval.beginSafe();


      while ( ! ( itvar.val() )->empty() ) {
        gum::HashTable<std::string, gum::Sequence<gum::Sequence<gum::Instantiation*>*>*>::iterator_safe itpvall = ( itvar.val() )->beginSafe();

        for ( gum::Sequence<gum::Sequence<gum::Instantiation*>*>::iterator itpv = ( itpvall.val() )->begin(); itpv != ( itpvall.val() )->end(); ++itpv ) {
          gum::Idx linecount = 0;
          gum::HashTable<std::string, gum::HashTable<const  gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>*> orderStruct; // set sizeof Hashtable

          for ( gum::Sequence<gum::Instantiation*>::iterator itseqv = ( *itpv )->begin(); itseqv != ( *itpv )->end(); ++itseqv , linecount++ ) {
            if ( ( *itseqv )->nbrDim() > 1 ) {
              for ( Idx iInst = 0; iInst < ( *itseqv )->nbrDim(); iInst++ ) {
                gum::Instantiation instpro ( **itseqv , false );
                instpro.reorder ( Order );
                const gum::DiscreteVariable* var = & ( ( *itseqv )->variable ( iInst ) );
                instpro.erase ( *var ); // reorder instance to optimize make sure key unicity.

                if ( !orderStruct.exists ( instpro.toString() ) ) {
                  orderStruct.insert ( instpro.toString(),
                                       new gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>() );
                }

                if ( !orderStruct[instpro.toString()]->exists ( var ) ) {
                  orderStruct[instpro.toString()]->insert ( var,
                      new std::pair<gum::Set<Idx>*, gum::Set<Idx>*> ( new gum::Set<Idx>, new gum::Set<Idx> ( ( *itseqv )->variable ( iInst ).domainSize() ) ) ); // set sizeof Hashtable
                }

                gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>* orderStruct2 = orderStruct[instpro.toString()];
                ( *orderStruct2 ) [var]->first->insert ( linecount );
                ( *orderStruct2 ) [var]->second->insert ( ( *itseqv )->val ( iInst ) );
              }
            }
          }

          gum::Set<gum::Idx> elimination;
          gum::Sequence<gum::Instantiation*>* newSeq = 0;

          for ( gum::HashTable<std::string, gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>*>::iterator_safe it = orderStruct.beginSafe();
                it != orderStruct.endSafe(); ++it ) {
            bool added = false;

            for ( gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>::iterator_safe it2 = ( it.val() )->beginSafe();
                  it2 != ( it.val() )->endSafe(); ++it2 ) {
              if ( ( it2.val() )->second->size() == ( it2.key() )->domainSize() ) {
                if ( !newSeq ) newSeq =  new gum::Sequence<gum::Instantiation*>();

                if ( !added ) {
                  added = true;
                  newSeq->insert ( new gum::Instantiation ( * ( ( **itpv ) [ ( * ( ( it2.val() )->first->beginSafe() ) )] ) , false ) );
                  newSeq->back()->erase ( * ( it2.key() ) );
                }

                elimination = elimination + * ( ( it2.val() )->first );
              }
            }
          }

          if ( newSeq ) {
            ( itpvall.val() )->insert ( newSeq );

            for ( int itelem = ( *itpv )->size() - 1; itelem >= 0; itelem-- ) {
              if ( elimination.exists ( ( gum::Idx ) itelem ) ) {
                delete ( ( **itpv ) [itelem] );
                ( *itpv )->erase ( ( **itpv ) [itelem] );
              }
            }
          }

          while ( !orderStruct.empty() ) {
            while ( ! ( orderStruct.beginSafe().val() )->empty() ) {
              delete orderStruct.beginSafe().val()->beginSafe().val()->first;
              delete orderStruct.beginSafe().val()->beginSafe().val()->second;
              ( orderStruct.beginSafe().val () )->erase ( ( orderStruct.beginSafe().val() )->beginSafe() );
            }

            delete orderStruct.beginSafe().val();
            orderStruct.erase ( orderStruct.beginSafe() );
          }
        }

        while ( ! ( itpvall.val() )->empty() ) {
          gum::Sequence<gum::Sequence<gum::Instantiation*>*>::iterator   itpv = ( itpvall.val() )->begin();

          while ( ! ( *itpv )->empty() ) {
            gum::Sequence<gum::Instantiation*>::iterator   itseqv = ( *itpv )->begin();

            for ( Idx i = 0; i < ( *itseqv )->nbrDim() ; i++ ) {
              std::stringstream str;
              str << ( *itseqv )->variable ( i ).name() << "_" << ( *itseqv )->val ( ( *itseqv )->variable ( i ) );
              str2 << "-" << vartable[str.str()] << " ";
            }

            if ( itpvall.key().compare ( "0" ) != 0 && itpvall.key().compare ( "0.0" ) != 0 ) {
              std::stringstream strinst;
              strinst << itvar.key()->name();
              strinst << "_val=" << itpvall.key();
              str2 << protable[strinst.str()];
            }

            str2  << " 0\n"; clause++;
            delete ( *itseqv );
            ( *itpv )->erase ( itseqv );
          }

          delete ( *itpv );
          ( itpvall.val() )->erase ( itpv );
        }

        delete ( itpvall.val() );
        ( itvar.val() )->erase ( itpvall );
      }

      delete ( itvar.val() );
      cptparamval.erase ( itvar );
    }

    clausstr << str2.str();

    output << "p cnf " << num << " " << clause << "\neclauses " << numvar << "\n" << clausstr.str() <<  clausstr2.str()  << std::endl;
    output.flush();
    outputvar << strfile.str() ;
    outputvar.flush();
    outputvar.close();

    if ( outputvar.fail() )
      GUM_ERROR ( IOError, "Writting in the ostream failed." );
  }

  // Writes a Bayesian Network in the referenced file using the BN format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian Network.
  // @param bn The Bayesian Network writed in the file.
  // @throws Raised if an I/O error occurs.
  template<typename GUM_SCALAR, template <class> class IApproximationPolicy> INLINE
  void
  ContextualDependenciesCNFWriter<GUM_SCALAR, IApproximationPolicy>::write ( std::string filePath, const IBayesNet<GUM_SCALAR>& bn ) {
    std::ofstream output ( filePath.c_str(), std::ios_base::trunc );
    std::ofstream outputvar ( ( filePath + ".var" ).c_str(), std::ios_base::trunc );

    if ( ! output.good() )
      GUM_ERROR ( IOError, "Stream states flags are not all unset." );

    std::stringstream strfile, strfile2;

    if ( ! outputvar.good() )
      GUM_ERROR ( IOError, "Stream states flags are not all unset." );

    Idx num = 0;
    Idx numvar = 0;
    Idx clause = 0;
    std::stringstream clausstr, clausstr2;
    gum::HashTable<std::string, Idx> vartable ;
    gum::HashTable<std::string, Idx> protable ;
    gum::HashTable<const gum::DiscreteVariable*, gum::HashTable<std::string, gum::Sequence< gum::Sequence<gum::Instantiation* >* >* >* > cptparamval;



    Instantiation Order;

    for ( gum::Sequence<gum::NodeId>::iterator it = bn.topologicalOrder().begin(); it != bn.topologicalOrder().end(); ++it )
      Order.add ( bn.variable ( *it ) );

    for ( const auto iter : bn.nodes() ) {
      std::stringstream str0 ;
      const DiscreteVariable* var = &bn.variable ( iter );

      for ( Idx i = 0; i < bn.variable ( iter ).domainSize(); i++ ) {
        std::stringstream stri ;
        stri << var->name() << "_" << var->label ( i );
        vartable.insert ( stri.str(), ++num );
        strfile << num << "::" << stri.str() << "\n";
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n"; clause++; numvar++;
      clausstr2 << str0.str();
      const Potential<GUM_SCALAR>& cpt = bn.cpt ( iter );
      Instantiation inst ( cpt ); inst.forgetMaster();
      inst.reorder ( Order );
      cptparamval.insert ( var, new gum::HashTable<std::string, gum::Sequence< gum::Sequence<gum::Instantiation* >* >* >() );

      for ( inst.setFirst(); ! inst.end(); ++inst ) {
        if ( this->fromExact ( cpt[inst] ) != 1 ) {
          std::stringstream strk;
          strk << this->fromExact ( cpt[inst] );
          std::string valp = strk.str();

          if ( ! ( cptparamval[var] )->exists ( valp ) ) {
            ( cptparamval[var] )->insert ( valp, new gum::Sequence<gum::Sequence<gum::Instantiation*>*>() ); //remember to verify protocole for param = to 1

            ( * ( cptparamval[var] ) ) [valp]->insert ( new gum::Sequence<gum::Instantiation*> );

            if ( this->fromExact ( cpt[inst] ) ) {
              std::stringstream strinst;
              strinst << var->name();
              strinst << "_val=" << this->fromExact ( cpt[inst] );

              if ( !protable.exists ( strinst.str() ) ) {
                protable.insert ( strinst.str() , ++num );
                strfile << num << "::" <<  strinst.str()  << "\n";
              }
            }
          }

          ( * ( cptparamval[var] ) ) [valp]->front()->insert ( new gum::Instantiation ( inst ) );
        }
      }
    }

    std::stringstream str2;

    while ( !cptparamval.empty() ) {
      gum::HashTable<const DiscreteVariable*, gum::HashTable<std::string, gum::Sequence<gum::Sequence<gum::Instantiation*>*>*>*>::iterator_safe itvar = cptparamval.beginSafe();

      while ( ! ( itvar.val() )->empty() ) {
        gum::HashTable<std::string, gum::Sequence<gum::Sequence<gum::Instantiation*>*>*>::iterator_safe itpvall = ( itvar.val() )->beginSafe();

        for ( gum::Sequence<gum::Sequence<gum::Instantiation*>*>::iterator itpv = ( itpvall.val() )->begin(); itpv != ( itpvall.val() )->end(); ++itpv ) {
          gum::Idx linecount = 0;
          gum::HashTable<std::string, gum::HashTable<const  gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>*> orderStruct; // set sizeof Hashtable

          gum::Set<gum::Idx> elimination;
          gum::HashTable<std::string, gum::Instantiation*> newSeqpre ;

          for ( gum::Sequence<gum::Instantiation*>::iterator itseqv = ( *itpv )->begin(); itseqv != ( *itpv )->end(); ++itseqv , linecount++ ) {

            if ( ( *itseqv )->nbrDim() > 1 ) {
              for ( Idx iInst = 0; iInst < ( *itseqv )->nbrDim(); iInst++ ) {
                gum::Instantiation* instpro = new gum::Instantiation ( **itseqv , false );
                const gum::DiscreteVariable* var = & ( ( *itseqv )->variable ( iInst ) );
                instpro->erase ( *var );
                instpro->reorder ( Order );

                if ( !orderStruct.exists ( instpro->toString() ) && !newSeqpre.exists ( instpro->toString() ) ) {
                  orderStruct.insert ( instpro->toString(),
                                       new gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>() );

                }

                if ( orderStruct.exists ( instpro->toString() ) && !orderStruct[instpro->toString()]->exists ( var ) ) {


                  orderStruct[instpro->toString()]->insert ( var,
                      new std::pair<gum::Set<Idx>*, gum::Set<Idx>*> ( new gum::Set<Idx>, new gum::Set<Idx> ( ( *itseqv )->variable ( iInst ).domainSize() ) ) ); // set sizeof Hashtable


                }

                if ( orderStruct.exists ( instpro->toString() ) && !newSeqpre.exists ( instpro->toString() ) ) {
                  gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>* orderStruct2 = orderStruct[instpro->toString()];
                  ( *orderStruct2 ) [var]->first->insert ( linecount );
                  ( *orderStruct2 ) [var]->second->insert ( ( *itseqv )->val ( iInst ) );

                  if ( ( *orderStruct2 ) [var]->second->size() == var->domainSize() ) {
                    newSeqpre.insert ( instpro->toString(), instpro );

                    while ( orderStruct2->size() ) {
                      gum::HashTable<const gum::DiscreteVariable* , std::pair<gum::Set<Idx>*, gum::Set<Idx>*>*>::iterator_safe itb = orderStruct2->beginSafe();
                      elimination = elimination + * ( ( itb.val() )->first );
                      delete ( ( itb.val() )->first );
                      delete ( ( itb.val() )->second );
                      delete ( itb.val() );
                      orderStruct2->erase ( itb.key() );
                    }

                    delete orderStruct2;
                    orderStruct.erase ( instpro->toString() );
                  } else delete instpro;
                } else if ( newSeqpre.exists ( instpro->toString() ) ) {
                  elimination.insert ( linecount );
                  delete instpro;
                }

              }
            }
          }

          gum::Sequence<gum::Instantiation*>* newSeq = 0;

          if ( newSeqpre.size() ) newSeq = new gum::Sequence<gum::Instantiation*>();

          while ( newSeqpre.size() ) {
            gum::HashTable<std::string, gum::Instantiation*>::iterator_safe ith = newSeqpre.beginSafe();
            newSeq->insert ( ith.val() );
            newSeqpre.erase ( ith.key() );
          }

          if ( newSeq ) {
            ( itpvall.val() )->insert ( newSeq );

            for ( int itelem = ( *itpv )->size() - 1; itelem >= 0; itelem-- ) {
              if ( elimination.exists ( ( gum::Idx ) itelem ) ) {
                delete ( ( **itpv ) [itelem] );
                ( *itpv )->erase ( ( **itpv ) [itelem] );
              }
            }
          }

          while ( !orderStruct.empty() ) {
            while ( ! ( orderStruct.beginSafe().val() )->empty() ) {
              delete orderStruct.beginSafe().val()->beginSafe().val()->first;
              delete orderStruct.beginSafe().val()->beginSafe().val()->second;
              ( orderStruct.beginSafe().val() )->erase ( orderStruct.beginSafe().val()->beginSafe() );
            }

            delete orderStruct.beginSafe().val();
            orderStruct.erase ( orderStruct.beginSafe() );
          }
        }


        while ( ! ( itpvall.val() )->empty() ) {
          gum::Sequence<gum::Sequence<gum::Instantiation*>*>::iterator   itpv = ( itpvall.val() )->begin();

          while ( ! ( *itpv )->empty() ) {
            gum::Sequence<gum::Instantiation*>::iterator   itseqv = ( *itpv )->begin();

            for ( Idx i = 0; i < ( *itseqv )->nbrDim() ; i++ ) {
              std::stringstream str;
              str << ( *itseqv )->variable ( i ).name() << "_" << ( *itseqv )->val ( ( *itseqv )->variable ( i ) );
              str2 << "-" << vartable[str.str()] << " ";
            }

            if ( itpvall.key().compare ( "0" ) != 0 && itpvall.key().compare ( "0.0" ) != 0 ) {
              std::stringstream strinst;
              strinst << itvar.key()->name();
              strinst << "_val=" << itpvall.key();
              str2 << protable[strinst.str()];
            }

            str2  << " 0\n"; clause++;
            delete ( *itseqv );
            ( *itpv )->erase ( itseqv );
          }

          delete ( *itpv );
          ( itpvall.val() )->erase ( itpv );
        }

        delete ( itpvall.val() );
        ( itvar.val() )->erase ( itpvall );
      }

      delete ( itvar.val() );
      cptparamval.erase ( itvar );
    }


    clausstr << str2.str();

    output << "p cnf " << num << " " << clause << "\neclauses " << numvar << "\n" << clausstr.str() <<  clausstr2.str()  << std::endl;
    output.flush();
    outputvar << strfile.str() ;
    outputvar.flush();

    outputvar.close();
    output.close();

    if ( outputvar.fail() )
      GUM_ERROR ( IOError, "Writting in the ostream failed." );

    if ( output.fail() )
      GUM_ERROR ( IOError, "Writting in the ostream failed." );

  }

} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
