#include <agrum/core/exceptions.h>
#include <agrum/CN/CredalNet.h>

namespace gum {

  template< typename GUM_SCALAR >
  CredalNet< GUM_SCALAR >::CredalNet ( const std::string &src_min_num, const std::string &src_max_den ) {
    try {
      __initParams();
      __initCNNets ( src_min_num, src_max_den );
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      throw ( err );
    }

    GUM_CONSTRUCTOR ( CredalNet );
  }

  template< typename GUM_SCALAR >
  CredalNet< GUM_SCALAR >::CredalNet ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den ) {
    try {
      __initParams();
      __initCNNets ( src_min_num, src_max_den );
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      throw ( err );
    }

    GUM_CONSTRUCTOR ( CredalNet );
  }

  template< typename GUM_SCALAR >
  CredalNet< GUM_SCALAR >::~CredalNet() {
    if ( __current_bn != NULL )
      delete __current_bn;

    if ( __credalNet_current_cpt != NULL )
      delete __credalNet_current_cpt;

    if ( __current_nodeType != NULL )
      delete __current_nodeType;

    GUM_DESTRUCTOR ( CredalNet );
  }

  // from BNs with numerators & denominators or cpts & denominators to credal
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::bnToCredal ( GUM_SCALAR beta, bool rational ) {
    double epsi_min = 1;
    double epsi_max = 0;
    double epsi_moy = 0;
    double epsi_den = 0;

    for ( gum::DAG::NodeIterator node_idIt = __src_bn.beginNodes(); node_idIt != __src_bn.endNodes(); ++node_idIt ) {
      int CASES = 0;

      const gum::Potential< GUM_SCALAR > * const potential ( &__src_bn.cpt ( *node_idIt ) );

      gum::Potential< GUM_SCALAR > * const potential_min ( const_cast< gum::Potential< GUM_SCALAR > * const > ( &__src_bn_min.cpt ( *node_idIt ) ) );
      gum::Potential< GUM_SCALAR > * const potential_max ( const_cast< gum::Potential< GUM_SCALAR > * const > ( &__src_bn_max.cpt ( *node_idIt ) ) );

      int var_dSize = __src_bn.variable ( *node_idIt ).domainSize();
      int entry_size = potential->domainSize() / var_dSize;

      gum::Instantiation ins ( potential );
      gum::Instantiation ins_min ( potential_min );
      gum::Instantiation ins_max ( potential_max );

      ins.setFirst(); ins_min.setFirst(); ins_max.setFirst();

      //GUM_SCALAR val_preced = 0.;
      //bool uniform;
      //std::cout << __src_bn.variable ( *node_idIt ).name() << std::endl;
      std::string var_name = __src_bn.variable ( *node_idIt ).name();
      var_name = var_name.substr ( 0, 2 );

      for ( int entry = 0; entry < entry_size; entry++ ) {
        //uniform = true;
        //int ins_incr = 0;
        //int positive_elem = 0;

        std::vector< GUM_SCALAR > vertex ( var_dSize );
        //GUM_SCALAR den = 1;

        //if(rational)
        GUM_SCALAR den = potential_max->get ( ins_max );
        CASES += den;

        GUM_SCALAR epsilon;

        if ( beta == 0 )
          epsilon = 0;
        else if ( den == 0 || beta == 1 )
          epsilon = 1;
        else
          epsilon = pow ( beta, log ( den + 1 ) );

        epsi_moy += epsilon;
        epsi_den += 1;

        if ( epsilon > epsi_max )
          epsi_max = epsilon;

        if ( epsilon < epsi_min )
          epsi_min = epsilon;

        GUM_SCALAR min, max;
        int nbm = 0;

        for ( int modality = 0; modality < var_dSize; modality++ ) {
          vertex[modality] = potential->get ( ins );

          if ( vertex[modality] > 0 )
            nbm++;

          ++ins;
        }

        //ins-=var_dSize;

        for ( int modality = 0; modality < var_dSize; modality++ ) {
          if ( vertex[modality] > 0 && nbm > 1 ) {
            min = ( 1. - epsilon ) * vertex[modality];
            max = min + epsilon;
          } else if ( vertex[modality] == 0 && nbm > 1 ) {
            min = 0.;
            max = 0.;
          } else if ( vertex[modality] > 0 && nbm <= 1 ) {
            min = vertex[modality];
            max = min;
          } else if ( vertex[modality] == 0 && nbm <= 1 ) {
            min = vertex[modality];
            max = min;
          }



          /*

                    if(den == 0) {
                       vertex[modality] = potential->get(ins);
                      //vertex[modality] = 1. / var_dSize;
                      if(rational)
                        vertex[modality] = 1. / var_dSize;
                      else
                        vertex[modality] = potential->get(ins);

                      if(beta == 0) {
                        min = vertex[modality];
                        max = vertex[modality];
                      }
                      else {
                      if(modality == var_dSize - 1) {
                        if(can_modify) {
                          min = 0.;
                          max = 1.;
                        }
                        else
                        {
                          min =  vertex[modality];
                          max = min;

                        }

                      } else {
                        min = 0.;
                        max = 1.;
                        can_modify = true;
                      }

                      //}
                    }
                    else {
                      vertex[modality] = potential->get(ins);
                      if(vertex[modality] > 0) {
                        if(modality == var_dSize - 1)
                        min = (1. - epsilon) * vertex[modality];
                        if(rational) {
                          max = min + epsilon * den;
                          min = min / den;
                          max = max / den;
                          vertex[modality] /= den;
                        }
                        else
                          max = min + epsilon;
                          can_modify = true;
                      }
                      // p(x=mod|.) = 0
                      else {
                        min = 0.;
                        max = 0.;
                      }
                    }
                */
          potential_min->set ( ins_min, min );
          potential_max->set ( ins_max, max );

          /*++ins; */++ins_min; ++ins_max;
        } // end of : for each modality

        /*
        for(int modality = 0; modality < var_dSize; modality++) {
          vertex[modality] = potential->get(ins);

          if(val_preced != 0 && fabs(vertex[modality] - val_preced) > __epsRedund)
            uniform = false;
          else if(vertex[modality] > 0) {
            val_preced = vertex[modality];
            positive_elem++;
          }

          if(modality + 1 != var_dSize || entry + 1 != entry_size) {
            ++ins; ++ins_min; ++ins_max;
            ins_incr++;
          }
        } // end of : for each modality
        */

        // check sum is 1
        double sum = 0.;

        for ( typename std::vector< GUM_SCALAR >::const_iterator modality = vertex.begin(); modality != vertex.end(); ++modality ) {
          sum += *modality;
        }

        if ( fabs ( 1. - sum ) > __epsRedund ) {
          ins -= var_dSize ;
          GUM_ERROR ( CPTNoSumTo1 , __src_bn.variable ( *node_idIt ).name() << "(" << __epsRedund << ")" << " " << entry << std::endl << vertex << std::endl << ins << std::endl );
        }

        // end check sum is 1

        /*
        if(uniform) {
          ins_min -= ins_incr;
          ins_max -= ins_incr;

          for(int modality = 0; modality < var_dSize; modality++) {
            GUM_SCALAR min = 0.;
            GUM_SCALAR max = 1.;

            if(vertex[modality] > 0) {
              potential_min->set(ins_min, min);
              potential_max->set(ins_max, max);
            }

            ++ins_min; ++ins_max;

          } // end of : for each modality
        } // end of : if uniform
        */

      } // end of : for each entry

      //std::cout << "cas : " << CASES << std::endl;
    } // end of : for each variable

    __epsilonMin = epsi_min;
    __epsilonMax = epsi_max;
    __epsilonMoy = epsi_moy;
    //std::cout << "epsilon min : " << epsi_min << std::endl;
    //std::cout << "epsilon max : " << epsi_max << std::endl;
    //std::cout << "epsilon moyen : " << epsi_moy / epsi_den << std::endl;

    __intervalToCredal();
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__intervalToCredal() {
    if ( ! __credalNet_src_cpt.empty() )
      __credalNet_src_cpt.clear();

    __credalNet_src_cpt.resize ( __src_bn.size() );

    for ( gum::DAG::NodeIterator node_idIt = __src_bn.beginNodes(); node_idIt != __src_bn.endNodes(); ++node_idIt ) {
      const gum::Potential< GUM_SCALAR > * const potential_min ( &__src_bn_min.cpt ( *node_idIt ) );
      const gum::Potential< GUM_SCALAR > * const potential_max ( &__src_bn_max.cpt ( *node_idIt ) );

      int var_dSize = __src_bn.variable ( *node_idIt ).domainSize();
      int entry_size = potential_min->domainSize() / var_dSize;

      std::vector< std::vector< std::vector< GUM_SCALAR > > > var_cpt ( entry_size );

      gum::Instantiation ins_min ( potential_min );
      gum::Instantiation ins_max ( potential_max );

      ins_min.setFirst();
      ins_max.setFirst();

      std::vector< GUM_SCALAR > lower ( var_dSize );
      std::vector< GUM_SCALAR > upper ( var_dSize );

      // use iterator
      for ( int entry = 0; entry < entry_size; entry++ ) {
        for ( int modality = 0; modality < var_dSize; modality++, ++ins_min, ++ins_max ) {
          lower[modality] = potential_min->get ( ins_min );
          upper[modality] = potential_max->get ( ins_max );
        }

        bool all_equals = true;
        std::vector< std::vector< GUM_SCALAR > > vertices;

        for ( int modality = 0; modality < var_dSize; modality++ ) {
          if ( fabs ( upper[modality] - lower[modality] ) < 1e-4 )
            continue;

          all_equals = false;
          std::vector< GUM_SCALAR > vertex ( var_dSize );
          vertex[modality] = upper[modality];

          for ( int mod = 0; mod < var_dSize; mod++ ) {
            if ( modality != mod )
              vertex[mod] = lower[mod];
          }

          double total = 0;

          for ( Size i = 0; i < vertex.size(); i++ )
            total += vertex[i];

          if ( fabs ( total - 1. ) > 1e-4 )
            std::cout << "vertex does not sum to one " << vertex << std::endl;

          vertices.push_back ( vertex );
        }

        if ( all_equals ) {
          std::vector< GUM_SCALAR > vertex ( var_dSize );

          for ( int modality = 0; modality < var_dSize; modality++ )
            vertex[modality] = lower[modality];

          double total = 0.;

          for ( Size i = 0; i < vertex.size(); i++ )
            total += vertex[i];

          if ( fabs ( total - 1. ) > 1e-4 )
            std::cout << "vertex does not sum to one " << vertex << std::endl;

          vertices.push_back ( vertex );
        }

        var_cpt[entry] = vertices;
      }

      __credalNet_src_cpt.insert ( *node_idIt, var_cpt );

    } // end of : for each variable (node)

    // get precise/credal/vacuous status of each variable
    __sort_varType();
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::intervalToCredal ( const int s ) {
    if ( ! __credalNet_src_cpt.empty() )
      __credalNet_src_cpt.clear();

    __credalNet_src_cpt.resize ( __src_bn.size() );

    for ( gum::DAG::NodeIterator node_idIt = __src_bn.beginNodes(); node_idIt != __src_bn.endNodes(); ++node_idIt ) {
      const gum::Potential< GUM_SCALAR > * const potential_min ( &__src_bn_min.cpt ( *node_idIt ) );
      const gum::Potential< GUM_SCALAR > * const potential_max ( &__src_bn_max.cpt ( *node_idIt ) );

      int var_dSize = __src_bn.variable ( *node_idIt ).domainSize();
      int entry_size = potential_min->domainSize() / var_dSize;

      std::vector< std::vector< std::vector< GUM_SCALAR > > > var_cpt ( entry_size );

      gum::Instantiation ins_min ( potential_min );
      gum::Instantiation ins_max ( potential_max );

      ins_min.setFirst();
      ins_max.setFirst();

      // use iterator
      for ( int entry = 0; entry < entry_size; entry++ ) {
        std::vector< std::vector< GUM_SCALAR > > vertices;
        std::vector< GUM_SCALAR > vertex ( var_dSize ); // if not interval

        std::vector< std::vector< GUM_SCALAR > > inequalities ( var_dSize * 2, std::vector< GUM_SCALAR > ( var_dSize + 1 + ( ( s > 0 ) ? 1 : 0 ) , 0 ) );

        std::vector< GUM_SCALAR > sum_ineq1 ( var_dSize + 1 + ( ( s > 0 ) ? 1 : 0 ), -1 );
        std::vector< GUM_SCALAR > sum_ineq2 ( var_dSize + 1 + ( ( s > 0 ) ? 1 : 0 ), 1 );
        sum_ineq1[0] = 1;
        sum_ineq1[ ( s > 0 ) ? 1 : 0] = 1;
        sum_ineq2[0] = -1;

        bool isInterval = false;

        // use iterator
        if ( s > 0 ) {
          isInterval = true;

          for ( int modality = 0; modality < var_dSize; modality++ ) {
            inequalities[modality * 2][0] = - potential_min->get ( ins_min );
            inequalities[modality * 2 + 1][0] = - inequalities[modality * 2][0] + s;
            inequalities[modality * 2][1] = potential_max->get ( ins_max ) + s;
            inequalities[modality * 2 + 1][1] = inequalities[modality * 2][1];
            inequalities[modality * 2][modality + 2] = 1;
            inequalities[modality * 2 + 1][modality + 2] = -1;

            ++ins_min;
          }

          for ( int modality = 0; modality < var_dSize; modality++ )
            ++ins_max;
        } else {
          for ( int modality = 0; modality < var_dSize; modality++ ) {
            inequalities[modality * 2][0] = - potential_min->get ( ins_min );
            inequalities[modality * 2 + 1][0] = potential_max->get ( ins_max );
            inequalities[modality * 2][modality + 1] = 1;
            inequalities[modality * 2 + 1][modality + 1] = -1;

            vertex[modality] = inequalities[modality * 2 + 1][0];

            if ( !isInterval && ( - inequalities[modality * 2][0] != inequalities[modality * 2 + 1][0] ) )
              isInterval = true;

            ++ins_min;
            ++ins_max;
          }
        }

        inequalities.push_back ( sum_ineq1 );
        inequalities.push_back ( sum_ineq2 );

        if ( ! isInterval ) {
          vertices.push_back ( vertex );
        } else {
          try {
            //std::cout << __src_bn.variable(*node_idIt) << " pconf : " << entry << " ineq : \n" << inequalities << std::endl;
            __H2V ( inequalities, vertices, ( ( s > 0 ) ? true : false ) );
            //std::cout << std::endl << vertices << std::endl;
          } catch ( const std::exception &err ) {
            std::cout << err.what() << std::endl;
            throw;
          }

          // permute first & second vertex to show L2U DTS error
          // to be removed once all is well
          /*if(vertices.size() > 1 && entry == 0) {
          std::vector< GUM_SCALAR > tmp = vertices[0];
          vertices[0] = vertices[1];
          vertices[1] = tmp;
          }*/
          // end of permute (to be removed)

        } // end of : is interval

        var_cpt[entry] = vertices;

      } // end of : for each entry

      __credalNet_src_cpt.insert ( *node_idIt, var_cpt );
      //std::cout << __src_bn.variable(*node_idIt).name() << std::endl;
      //std::cout << var_cpt << std::endl;

    } // end of : for each variable (node)

    // get precise/credal/vacuous status of each variable
    __sort_varType();

  }

  /**
   * to call after bnToCredal( GUM_SCALAR beta, bool rational )
   * save a BN with lower probabilities and a BN with upper ones
   */
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::saveBNsMinMax( const std::string & min_path, const std::string & max_path ) const {
    gum::BIFWriter< GUM_SCALAR > writer;

    std::string minfilename = min_path;//"min.bif";
    std::string maxfilename = max_path;//"max.bif";
    std::ofstream min_file ( minfilename.c_str() , std::ios::out | std::ios::trunc );
    std::ofstream max_file ( maxfilename.c_str(), std::ios::out | std::ios::trunc );

    if ( ! min_file.good() )
      GUM_ERROR ( IOError, "bnToCredal() : could not open stream : min_file : " << minfilename );

    if ( ! max_file.good() ) {
      min_file.close();
      GUM_ERROR ( IOError, "bnToCredal() : could not open stream : min_file : " << maxfilename );
    }

    try {
      writer.write ( min_file, __src_bn_min );
      writer.write ( max_file, __src_bn_max );
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      min_file.close();
      max_file.close();
      throw ( err );
    }

    min_file.close();
    max_file.close();

  }



  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::dts() {
    // don't forget to delete the old one (__current), if necessary at the end
    gum::BayesNet< GUM_SCALAR > * __dts_bn = new gum::BayesNet< GUM_SCALAR >();

    __bnCopy ( *__dts_bn );
    // delete old one too
    typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_dts_cpt = new typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes();

    // delete old one too
    typename gum::Property< nodeType >::onNodes *__dts_nodeType = new typename gum::Property< nodeType >::onNodes();

    const gum::BayesNet< GUM_SCALAR > * __current_bn;
    const typename gum::Property< nodeType >::onNodes *__current_nodeType;
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;

    // does it mean something to do the dts on the current bn and not the src bn ?
    if ( this->__current_bn == NULL )
      __current_bn = & this->__src_bn;
    else
      __current_bn = this->__current_bn;

    // same thing here
    if ( this->__credalNet_current_cpt == NULL )
      __credalNet_current_cpt = & this->__credalNet_src_cpt;
    else
      __credalNet_current_cpt = this->__credalNet_current_cpt;

    if ( this->__current_nodeType == NULL )
      __current_nodeType = & this->__nodeType;
    else
      __current_nodeType = this->__current_nodeType;

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      int var_dSize = __current_bn->variable ( *node_idIt ).domainSize();

      // CREDAL variable, add decision node
      if ( var_dSize > 2 && ( *__current_nodeType ) [*node_idIt] == CREDAL ) {
        int dNode_card = __find_dNode_card ( ( *__credalNet_current_cpt ) [*node_idIt] );
        int nb_bits, new_card;
        __superiorPow ( dNode_card, nb_bits, new_card );

        std::string dNode_bit_name;

        __dts_bn->beginTopologyTransformation();

        // erase arcs to put d node bits first in cpt
        for ( gum::Sequence< const gum::DiscreteVariable * >::const_iterator parent_idIt = __current_bn->cpt ( *node_idIt ).begin(); parent_idIt != __current_bn->cpt ( *node_idIt ).end(); ++parent_idIt )
          if ( *node_idIt != __current_bn->nodeId ( **parent_idIt ) )
            __dts_bn->eraseArc ( __current_bn->nodeId ( **parent_idIt ), *node_idIt );

        // create all D node bits
        for ( int bit = 0; bit < nb_bits; bit++ ) {
          dNode_bit_name = "D - " + __current_bn->variable ( *node_idIt ).name() + " - bit - ";
          std::stringstream ss;
          ss << bit;
          dNode_bit_name += ss.str();

          gum::LabelizedVariable dNode_bit ( dNode_bit_name, "node " + dNode_bit_name, 2 );
          gum::NodeId dBit_iD = __dts_bn->add ( dNode_bit );

          // create d node bit cpt
          std::vector< std::vector< std::vector< GUM_SCALAR > > > dNode_bit_cpt ( 1, std::vector< std::vector< GUM_SCALAR > > ( 2, std::vector< GUM_SCALAR > ( 2, 0 ) ) );
          dNode_bit_cpt[0][0][0] = 1;
          dNode_bit_cpt[0][1][1] = 1;

          __credalNet_dts_cpt->insert ( dBit_iD, dNode_bit_cpt );
          __dts_bn->insertArc ( dBit_iD, *node_idIt );
          __dts_nodeType->insert ( dBit_iD, DNODE );
        } // end of : for each d node bit

        // add parents back
        for ( gum::Sequence< const gum::DiscreteVariable * >::const_iterator parent_idIt = __current_bn->cpt ( *node_idIt ).begin(); parent_idIt != __current_bn->cpt ( *node_idIt ).end(); ++parent_idIt )
          if ( *node_idIt != __current_bn->nodeId ( **parent_idIt ) )
            __dts_bn->insertArc ( __current_bn->nodeId ( **parent_idIt ), *node_idIt );

        __dts_bn->endTopologyTransformation();

        // reform cpt of variable according to dNode bits
        gum::Potential< GUM_SCALAR > const *potential ( &__dts_bn->cpt ( *node_idIt ) );

        int entry_size = potential->domainSize() / var_dSize;
        std::vector< std::vector < std::vector< GUM_SCALAR > > > var_cpt ( entry_size );

        int id_vertex = 0;
        int old_entry = 0;

        for ( int entry = 0; entry < entry_size; entry++ ) {
          std::vector< std::vector< GUM_SCALAR > > vertices ( 1, std::vector< GUM_SCALAR > ( var_dSize ) );

          if ( id_vertex < ( *__credalNet_current_cpt ) [*node_idIt][old_entry].size() )
            vertices[0] = ( *__credalNet_current_cpt ) [*node_idIt][old_entry][id_vertex];
          else
            vertices[0] = ( *__credalNet_current_cpt ) [*node_idIt][old_entry][0];

          id_vertex++;

          if ( id_vertex == new_card ) {
            old_entry++;
            id_vertex = 0;
          }

          var_cpt[entry] = vertices;
        } // end of : for each entry in new potential

        __credalNet_dts_cpt->insert ( *node_idIt, var_cpt );
        __dts_nodeType->insert ( *node_idIt, PRECISE );

      } // end of : if card > 2 && CREDAL
      else {
        __credalNet_dts_cpt->insert ( *node_idIt, ( *__credalNet_current_cpt ) [*node_idIt] );

        if ( ( *__current_nodeType ) [*node_idIt] == VACUOUS )
          __dts_nodeType->insert ( *node_idIt, VACUOUS );
        else if ( ( *__current_nodeType ) [*node_idIt] == CREDAL )
          __dts_nodeType->insert ( *node_idIt, CREDAL );
        else if ( ( *__current_nodeType ) [*node_idIt] == PRECISE )
          __dts_nodeType->insert ( *node_idIt, PRECISE );
      }

    } // end of : for each original variable

    if ( this->__current_bn != NULL )
      delete this->__current_bn;

    this->__current_bn = __dts_bn;

    if ( this->__credalNet_current_cpt != NULL )
      delete this->__credalNet_current_cpt;

    this->__credalNet_current_cpt = __credalNet_dts_cpt;

    if ( this->__current_nodeType != NULL )
      delete this->__current_nodeType;

    this->__current_nodeType = __dts_nodeType;

  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::bin() {
    // don't forget to delete the old one (__current), if necessary at the end
    gum::BayesNet< GUM_SCALAR > * __bin_bn = new gum::BayesNet< GUM_SCALAR >();

    __bnCopy ( *__bin_bn );
    // delete old one too
    typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_bin_cpt = new typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes();

    // delete old one too
    typename gum::Property< nodeType >::onNodes *__bin_nodeType = new typename gum::Property< nodeType >::onNodes();

    const gum::BayesNet< GUM_SCALAR > * __current_bn;
    const typename gum::Property< nodeType >::onNodes *__current_nodeType;
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;

    if ( this->__current_bn == NULL )
      __current_bn = & this->__src_bn;
    else
      __current_bn = this->__current_bn;

    if ( this->__credalNet_current_cpt == NULL )
      __credalNet_current_cpt = & this->__credalNet_src_cpt;
    else
      __credalNet_current_cpt = this->__credalNet_current_cpt;

    if ( this->__current_nodeType == NULL )
      __current_nodeType = & this->__nodeType;
    else
      __current_nodeType = this->__current_nodeType;

    if ( ! __var_bits.empty() )
      __var_bits.clear();

    __bin_bn->beginTopologyTransformation();

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      int nb_bits, new_card;
      int var_dSize = __current_bn->variable ( *node_idIt ).domainSize();

      if ( var_dSize != 2 ) {
        __superiorPow ( var_dSize, nb_bits, new_card );

        std::string bit_name;
        std::vector< gum::NodeId > bits ( nb_bits );

        for ( int bit = 0; bit < nb_bits; bit++ ) {
          bit_name = __current_bn->variable ( *node_idIt ).name() + " - bit - ";
          std::stringstream ss;
          ss << bit;
          bit_name += ss.str();

          gum::LabelizedVariable var_bit ( bit_name, "node " + bit_name, 2 );
          gum::NodeId iD = __bin_bn->add ( var_bit );

          bits[bit] = iD;
        } // end of : for each bit

        __var_bits.insert ( *node_idIt, bits );

      } // end of : if variable is not binary
      else {
        std::string bit_name = __current_bn->variable ( *node_idIt ).name();
        gum::LabelizedVariable var_bit ( bit_name, "node " + bit_name, 2 );
        gum::NodeId iD = __bin_bn->add ( var_bit );

        __var_bits.insert ( *node_idIt, std::vector< gum::NodeId > ( 1, iD ) );
      }

    } // end of : for each original variable

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      gum::NodeSet parents = __current_bn->dag().parents ( *node_idIt );

      if ( ! parents.empty() ) {
        for ( gum::Sequence< const gum::DiscreteVariable * >::const_iterator parent_idIt = __current_bn->cpt ( *node_idIt ).begin(); parent_idIt != __current_bn->cpt ( *node_idIt ).end(); ++parent_idIt ) {

          if ( __current_bn->nodeId ( **parent_idIt ) != *node_idIt ) {
            for ( int parent_bit = 0; parent_bit < __var_bits[__current_bn->nodeId ( **parent_idIt )].size(); parent_bit++ )
              for ( int var_bit = 0; var_bit < __var_bits[*node_idIt].size(); var_bit++ )
                __bin_bn->insertArc ( __var_bits[__current_bn->nodeId ( **parent_idIt )][parent_bit], __var_bits[*node_idIt][var_bit] );
          }

        } // end of : for each parent
      } // end of : if parents

      // arcs with one's bits
      /*
      for(int bit_c = 1; bit_c < __var_bits[*node_idIt].size(); bit_c++)
      for(int bit_p = 0; bit_p < bit_c; bit_p++)
      __bin_bn->insertArc(__var_bits[*node_idIt][bit_p], var_bits[*node_idIt][bit_c]);
      */

    } // end of : for each original variable

    __bin_bn->endTopologyTransformation();

    // binarize cpt
    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      for ( int bit = 0; bit < __var_bits[*node_idIt].size(); bit++ ) {
        gum::Potential< GUM_SCALAR > const *potential ( &__bin_bn->cpt ( __var_bits[*node_idIt][bit] ) );

        gum::Instantiation ins ( potential );
        ins.setFirst();

        int entry_size = potential->domainSize() / 2;//__bin_bn->variable(__var_bits[*node_idIt][bit]).domainSize();
        std::vector< std::vector< std::vector< GUM_SCALAR > > > var_cpt ( entry_size );

        int old_entry = 0;



      } // end of : for each original variable bits
    } // end of : for each original variable

    if ( this->__current_bn != NULL )
      delete this->__current_bn;

    this->__current_bn = __bin_bn;

    if ( this->__credalNet_current_cpt != NULL )
      delete this->__credalNet_current_cpt;

    this->__credalNet_current_cpt = __credalNet_bin_cpt;

    if ( this->__current_nodeType != NULL )
      delete this->__current_nodeType;

    this->__current_nodeType = __bin_nodeType;

  }

  template< typename GUM_SCALAR >
  const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes &CredalNet< GUM_SCALAR >::credalNet_cpt() const {
    if ( this->__credalNet_current_cpt != NULL )
      return *this->__credalNet_current_cpt;

    return this->__credalNet_src_cpt;
  }

  template< typename GUM_SCALAR >
  typename CredalNet< GUM_SCALAR >::nodeType CredalNet< GUM_SCALAR >::getNodeType ( const gum::NodeId &id ) const {
    if ( this->__current_nodeType != NULL )
      return ( * ( this->__current_nodeType ) ) [id];

    return this->__nodeType[id];
  }



  ///////////////////////////////// test decimal -> fractional algorithms
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::testFrac() const {
    int64_t num, den;
    double num1 = 0.;

    double num2 = 1.;

    __farley ( num, den, num1, __denMax );

    std::cout << num << " / " << den << std::endl;
    __farley ( num, den, num2, __denMax );

    std::cout  << num << " / " << den << std::endl;




    /*
    srand(NULL);
    int64_t num, den;
    int iter = 10;
    //int PRECISION = 10000;
    while(iter > 0)
    {
    double prob = rand()/(double)RAND_MAX;
    __frac(num, den, prob);
    std::cout << " num : " << prob << " frac : " << num << " / " << den << " val : " << (double) num/den << std::endl;
    __farley(num,den,prob,__denMax);
    std::cout << "farley : " << num << " / " << den << " val : " << (double)num/den << std::endl;

    __fracC(num, den, (int)(prob*__precisionC), __deltaC, __precisionC);
    std::cout << "other num : "<< (int)(__precision*prob)*10 << " other frac : " << num << " / " << den << " = " << num/(double)den << std::endl;
        iter--;
      }*/
  }
  //////////////////////////////// end of test decimal -> fractional
  //
  // test pow

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::testPow() const {
    int x = 0;
    __int2Pow ( x );
    std::cout << x << std::endl;
  }

  // end of test pow
  
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::intPow ( const int &base, int &exponent ) const {
    __intPow(base,exponent);
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::int2Pow ( int &exponent ) const {
    __int2Pow(exponent);
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::superiorPow ( const int &card, int &num_bits, int &new_card ) const {
    __superiorPow(card,num_bits,new_card);
  }

  // only if CN is binary !!
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::computeCPTMinMax() {
    binCptMin.resize(__src_bn.size());
    binCptMax.resize(__src_bn.size());
    for ( gum::DAG::NodeIterator node_idIt = __src_bn.beginNodes(); node_idIt != __src_bn.endNodes(); ++node_idIt ) {
      std::cout << __src_bn.variable(*node_idIt).name() << std::endl;
      int pConf = credalNet_cpt()[*node_idIt].size();
      std::vector< GUM_SCALAR > min(pConf);
      std::vector< GUM_SCALAR > max(pConf);
      for( int pconf = 0; pconf < pConf; pconf++) {
        GUM_SCALAR v1, v2;
        v1 = credalNet_cpt()[*node_idIt][pconf][0][1];
        if(__credalNet_src_cpt[*node_idIt][pconf].size() > 1)
          v2 = credalNet_cpt()[*node_idIt][pconf][1][1];
        else
          v2 = v1;
        std::cout << "pconf : " << pconf << std::endl;
        std::cout << credalNet_cpt()[*node_idIt][pconf] << std::endl;
        std::cout << "v1, v2 : " << v1 << ", " << v2 << std::endl;
        GUM_SCALAR delta = v1 - v2;
        min[pconf] = (delta >= 0) ? v2 : v1;
        max[pconf] = (delta >= 0) ? v1 : v2;
      }
      binCptMin[*node_idIt] = min;
      binCptMax[*node_idIt] = max;
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< GUM_SCALAR > > & CredalNet< GUM_SCALAR >::get_CPT_min() const {
    return binCptMin;
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< GUM_SCALAR > > & CredalNet< GUM_SCALAR >::get_CPT_max() const {
    return binCptMax;
  }

  template< typename GUM_SCALAR >
  const double & CredalNet< GUM_SCALAR >::getEpsilonMin() const {
    return __epsilonMin;
  }

  template< typename GUM_SCALAR >
  const double & CredalNet< GUM_SCALAR >::getEpsilonMax() const {
    return __epsilonMax;
  }

  template< typename GUM_SCALAR >
  const double & CredalNet< GUM_SCALAR >::getEpsilonMoy() const {
    return __epsilonMoy;
  }


  template< typename GUM_SCALAR >
  std::string CredalNet< GUM_SCALAR >::toString() const {
    std::stringstream output;
    const gum::BayesNet< GUM_SCALAR > * __current_bn;
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;

    if ( this->__current_bn == NULL )
      __current_bn = & this->__src_bn;
    else
      __current_bn = this->__current_bn;

    if ( this->__credalNet_current_cpt == NULL )
      __credalNet_current_cpt = & this->__credalNet_src_cpt;
    else
      __credalNet_current_cpt = this->__credalNet_current_cpt;

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      const gum::Potential< GUM_SCALAR > * potential ( &__current_bn->cpt ( *node_idIt ) );
      int pconfs = potential->domainSize() / __current_bn->variable ( *node_idIt ).domainSize();

      output << "\n" << __current_bn->variable ( *node_idIt ) << "\n";

      gum::Instantiation ins ( potential );
      ins.forgetMaster();
      ins.erase ( __current_bn->variable ( *node_idIt ) );
      ins.setFirst();

      for ( int pconf = 0; pconf < pconfs; pconf++ ) {
        output << ins << " : ";
        output << ( *__credalNet_current_cpt ) [*node_idIt][pconf] << std::endl;

        if ( pconf < pconfs - 1 )
          ++ins;
      }
    }

    output << "\n";

    return output.str();
  }

  template< typename GUM_SCALAR >
  const gum::BayesNet< GUM_SCALAR > & CredalNet< GUM_SCALAR >::current_bn() const {
    if ( __current_bn != NULL )
      return *__current_bn;

    return __src_bn;
  }

  template< typename GUM_SCALAR >
  const gum::BayesNet< GUM_SCALAR > & CredalNet< GUM_SCALAR >::src_bn() const {
    return __src_bn;
  }



  /////////// protected stuff //////////

  /////////// private stuff ////////////


  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__initParams() {
    GUM_TRACE ( "===================================================" );
    GUM_TRACE ( "===================================================" );
    __epsilonMin = 0;
    __epsilonMax = 0;
    __epsilonMoy = 0;

    __epsRedund = 1e-4;

    // farley algorithm
    __epsF = 1e-6;
    __denMax = 1e6; // beware LRS

    // continued fractions, beware LRS
    // decimal paces (__epsC * __precisionC == 1)
    __precisionC = 1e6;
    __deltaC = 5;

    // old custom algorithm
    __precision = 1e6; // beware LRS

    __current_bn = NULL;
    __credalNet_current_cpt =  NULL;
    __current_nodeType = NULL;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__initCNNets ( const std::string &src_min_num, const std::string &src_max_den ) {
    gum::BIFReader< GUM_SCALAR > reader ( &__src_bn, src_min_num );
    std::string other;

    if ( src_max_den.compare ( "" ) != 0 )
      other = src_max_den;
    else
      other = src_min_num;

    gum::BIFReader< GUM_SCALAR > reader_min ( &__src_bn_min, src_min_num );
    gum::BIFReader< GUM_SCALAR > reader_max ( &__src_bn_max, other );

    try {
      reader.proceed();
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      throw ( err );
    }

    try {
      reader_min.proceed();
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      throw ( err );
    }

    try {
      reader_max.proceed();
    } catch ( gum::Exception &err ) {
      GUM_SHOWERROR ( err );
      throw ( err );
    }

  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__initCNNets ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den ) {
    __src_bn = src_min_num;
    __src_bn_min = src_min_num;

    if ( src_max_den.size() > 0 )
      __src_bn_max = src_max_den;
    else
      __src_bn_max = src_min_num;
  }

  template< typename GUM_SCALAR >
  int CredalNet< GUM_SCALAR >::__find_dNode_card ( const std::vector< std::vector< std::vector< GUM_SCALAR > > > & var_cpt ) const {
    int vertices_size = 0;

    for ( typename std::vector< std::vector< std::vector< GUM_SCALAR > > >::const_iterator entry = var_cpt.begin(); entry != var_cpt.end(); ++entry ) {
      if ( entry->size() > vertices_size )
        vertices_size = entry->size();
    }

    return vertices_size;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__bnCopy ( gum::BayesNet< GUM_SCALAR > & dest ) {
    const gum::BayesNet< GUM_SCALAR > * __current_bn;

    if ( this->__current_bn == NULL )
      __current_bn = & this->__src_bn;
    else
      __current_bn = this->__current_bn;

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt )
      dest.add ( __current_bn->variable ( *node_idIt ) );

    dest.beginTopologyTransformation();

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {
      for ( gum::Sequence< const gum::DiscreteVariable * >::const_iterator parent_idIt = __current_bn->cpt ( *node_idIt ).begin(); parent_idIt != __current_bn->cpt ( *node_idIt ).end(); ++parent_idIt ) {
        if ( __current_bn->nodeId ( **parent_idIt ) != *node_idIt )
          dest.insertArc ( __current_bn->nodeId ( **parent_idIt ), *node_idIt );
      } // end of : for each parent in order of appearence
    } // end of : for each variable

    dest.endTopologyTransformation();
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__intPow ( const int &base, int &exponent ) const {
    if ( exponent == 0 ) {
      exponent = 1;
      return;
    }

    int out = base;

    for ( int i = 1; i < exponent; i++ )
      out *= base;

    exponent = out;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__int2Pow ( int &exponent ) const {
    int base = 2;
    base <<= ( exponent - 1 );
    exponent = base;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__superiorPow ( const int &card, int &num_bits, int &new_card ) const {
    if ( card <= 0 ) {
      num_bits = 0;
      new_card = 1;
      return;
    }

    num_bits = 1;
    new_card = 2;

    while ( new_card < card ) {
      new_card *= 2;
      num_bits++;
    }
  }


  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__H2V ( const std::vector< std::vector< GUM_SCALAR > > & h_rep, std::vector< std::vector< GUM_SCALAR > > & v_rep, const bool rationals ) const {
    // write H rep file
    int64_t num, den;

    char * inefile = tmpnam(NULL); // generate unique file name, we need to add .ine or .ext for lrs to know which input it is (Hrep to Vrep or Vrep to Hrep)
    std::string sinefile(inefile);
    sinefile += ".ine";

    std::ofstream h_file ( sinefile.c_str(), std::ios::out | std::ios::trunc );

    if ( ! h_file.good() )
      GUM_ERROR ( IOError, "__H2V : could not open lrs input file : " << inefile );

    h_file << "H - representation\n";
    h_file << "begin\n";
    h_file << h_rep.size() << ' ' << h_rep[0].size() - ( rationals ? 1 : 0 )  << " rational\n";

    if ( rationals ) {
      for ( typename std::vector< std::vector< GUM_SCALAR > >::const_iterator it = h_rep.begin(); it != h_rep.end(); ++it ) {
        h_file << *it->begin() << '/' << * ( ++it->begin() ) << ' ';

        for ( typename std::vector< GUM_SCALAR >::const_iterator it2 = ++ ( ++ ( it->begin() ) ); it2 != it->end(); ++it2 ) {
          h_file << *it2 << '/' << 1 << ' ';
        }

        h_file << '\n';
      }
    } else {
      for ( typename std::vector< std::vector< GUM_SCALAR > >::const_iterator it = h_rep.begin(); it != h_rep.end(); ++it ) {
        for ( typename std::vector< GUM_SCALAR >::const_iterator it2 = it->begin(); it2 != it->end(); ++it2 ) {
          // get integer fraction from decimal value
          // smaller numerator & denominator is farley
          __farley ( num, den, ((*it2 > 0) ? *it2 : - *it2), __denMax );
          h_file << ((*it2 > 0) ? num : -num) << '/' << den << ' ';
        }

        h_file << '\n';
      }
    }

    h_file << "end\n";
    h_file.close();

    // call lrs
    // lrs arguments
    char *args[3];

    std::string soft_name = "lrs";
    std::string extfile(inefile);
    extfile += ".ext";
    
    args[0] = new char[soft_name.size()];
    args[1] = new char[sinefile.size()];
    args[2] = new char[extfile.size()];

    strcpy ( args[0], soft_name.c_str() );
    strcpy ( args[1], sinefile.c_str() );
    strcpy ( args[2], extfile.c_str() );


    // cout to null not working in agrum, why ?
    //
    // doesn't matter, use temporary file (not working with TestSuite either)
    //
 /*   char * lrs_outputs = tmpnam(NULL); 
    std::string lrslog(lrs_outputs);
    lrslog += ".lrslog";

    std::ofstream l_file ( lrslog.c_str(), std::ios::out | std::ios::trunc );

    if ( ! l_file.good() )
      GUM_ERROR ( IOError, "__H2V : could not open lrs log file : " << lrslog );

    std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    std::cout.rdbuf(l_file.rdbuf()); //redirect cout to tmp file

    lrs_main ( 3, args );

    std::cout.rdbuf(coutbuf); //restore standard output again
    
    // delete file
    if( std::remove(lrslog.c_str()) != 0)
      GUM_ERROR(IOError, "error removing : " + lrslog);
*/
    /////
    /////
    /////


    // standard cout to null (avoid lrs flooding)
    int old_cout, new_cout;
    fflush ( stdout );
    old_cout = dup ( 1 );
    
    new_cout = open ( "/dev/null", O_WRONLY );
    dup2 ( new_cout, 1 );
    close ( new_cout );

    lrs_main ( 3, args );
    
    // restore standard cout
    fflush ( stdout );
    dup2 ( old_cout, 1 );
    close ( old_cout );


    delete args[2]; delete args[1]; delete args[0];

    // read V rep file
    std::ifstream v_file ( extfile.c_str() /*extfilename.c_str()*/, std::ios::in );

    if ( ! v_file.good() )
      GUM_ERROR ( IOError, "__H2V() : could not open lrs ouput file : " );

    std::string line, tmp;
    char *cstr, * p;
    GUM_SCALAR probability;

    size_t pos, end_pos;
    bool keep_going = true;
    int vertices = 0;

    std::vector< GUM_SCALAR > vertex;

    v_file.ignore ( 256, 'l' );

    while ( v_file.good() && keep_going ) {
      getline ( v_file, line );

      if ( line.size() == 0 ) continue;
      else if ( line.compare ( "end" ) == 0 ) {
        keep_going = false;
        getline ( v_file, line );
        pos = line.find ( "vertices = " );
        end_pos = line.find ( "rays", pos + 9 );
        vertices = atoi ( line.substr ( pos + 9, end_pos - pos - 9 ).c_str() );
        break;
      } else if ( line[1] != '1' ) {
        GUM_ERROR ( IOError, "__H2V() : reading something other than a vertex from lrs output file : " );
      }

      line = line.substr ( 2 );
      cstr = new char[line.size() + 1];
      strcpy ( cstr, line.c_str() );

      p = strtok ( cstr, " " );

      while ( p != NULL ) {
        tmp = p;

        if ( tmp.compare ( "1" ) == 0 || tmp.compare ( "0" ) == 0 )
          probability = atof ( tmp.c_str() );
        else {
          pos = tmp.find ( "/" );
          probability = atof ( tmp.substr ( 0, pos ).c_str() ) / atof ( tmp.substr ( pos + 1, tmp.size() ).c_str() );
        }

        vertex.push_back ( probability );
        p = strtok ( NULL, " " );
      } // end of : for all tokens

      delete[] p;
      delete[] cstr;

      bool is_redund = false;

      #pragma omp parallel
      {
        int this_thread = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        size_t begin_pos = ( this_thread + 0 ) * v_rep.size() / num_threads;
        size_t end_pos = ( this_thread + 1 ) * v_rep.size() / num_threads;

        for ( size_t p = begin_pos; p < end_pos; p++ ) {
          #pragma omp flush(is_redund)

          if ( is_redund ) break;

          bool thread_redund = true;

          for ( int modality = 0; modality < vertex.size(); modality++ ) {
            if ( fabs ( vertex[modality] - v_rep[p][modality] ) > __epsRedund ) {
              thread_redund = false;
              break;
            }
          }

          if ( thread_redund ) {
            is_redund = true;
            #pragma omp flush(is_redund)
          }
        } // end of : each thread for
      } // end of : parallel

      if ( ! is_redund )
        v_rep.push_back ( vertex );

      vertex.clear();

    } // end of : file

    v_file.close();

    if( std::remove(sinefile.c_str()) != 0)
      GUM_ERROR(IOError, "error removing : " + sinefile);
    if( std::remove(extfile.c_str()) != 0)
      GUM_ERROR(IOError, "error removing : " + extfile);
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__sort_varType() {
    typename gum::Property< nodeType >::onNodes *__current_nodeType;
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;

    const gum::BayesNet< GUM_SCALAR > * __current_bn;

    if ( this->__current_bn == NULL )
      __current_bn = & this->__src_bn;
    else
      __current_bn = this->__current_bn;

    if ( this->__credalNet_current_cpt == NULL )
      __credalNet_current_cpt = & this->__credalNet_src_cpt;
    else
      __credalNet_current_cpt = this->__credalNet_current_cpt;

    if ( this->__current_nodeType == NULL )
      __current_nodeType = & this->__nodeType;
    else
      __current_nodeType = this->__current_nodeType;

    if ( ! __current_nodeType->empty() )
      __current_nodeType->clear();

    for ( gum::DAG::NodeIterator node_idIt = __current_bn->beginNodes(); node_idIt != __current_bn->endNodes(); ++node_idIt ) {

      bool precise = true, vacuous = true;

      for ( typename std::vector< std::vector< std::vector< GUM_SCALAR > > >::const_iterator entry = ( *__credalNet_current_cpt ) [*node_idIt].begin(); entry != ( *__credalNet_current_cpt ) [*node_idIt].end(); ++entry ) {

        int vertices = entry->size();
        int var_dSize = ( *entry ) [0].size();

        if ( precise && vertices > 1 )
          precise = false;

        if ( vacuous && vertices == var_dSize ) {
          std::vector< bool > elem ( var_dSize, false );

          for ( typename std::vector< std::vector< GUM_SCALAR > >::const_iterator vertex = entry->begin(); vertex != entry->end(); ++vertex ) {
            for ( typename std::vector< GUM_SCALAR >::const_iterator probability = vertex->begin(); probability != vertex->end(); ++probability ) {
              if ( *probability == 1 ) {
                elem[probability - vertex->begin()] = true;
                break;
              }
            } // end of : for each modality

            break; // not vacuous
          } // end of : for each vertex

          for ( std::vector< bool >::const_iterator probability = elem.begin(); probability != elem.end(); ++probability )
            if ( *probability == false )
              vacuous = false;

        } // end of : if vertices == dSize

        if ( vacuous == false && precise == false ) {
          __current_nodeType->insert ( *node_idIt, CREDAL );
          break;
        }

      } // end of : for each parents entry

      if ( vacuous )
        __current_nodeType->insert ( *node_idIt, VACUOUS );
      else if ( precise )
        __current_nodeType->insert ( *node_idIt, PRECISE );

    } // end of : for each variable
  }

  //////////////////////////////// farley algorithm
  // does not work if number = 0 and / or 1 ?
  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__farley ( int64_t &numerator, int64_t &denominator, const GUM_SCALAR &number, const int &den_max ) const {

    if ( number < 0 && fabs ( fabs ( number ) - 1 ) < __epsF ) {
      numerator = -1;
      denominator = 1;
      return;
    } else if ( fabs ( number - 1. ) < __epsF ) {
      numerator = 1;
      denominator = 1;
      return;
    } else if ( number < __epsF ) {
      numerator = 0;
      denominator = 1;
      return;
    }

    int a ( 0 ), b ( 1 ), c ( 1 ), d ( 1 );
    double mediant ( 0.0F );

    while ( b <= den_max && d <= den_max ) {
      mediant = ( double ) ( a + c ) / ( double ) ( b + d );

      if ( fabs ( number - mediant ) < __epsF ) {
        if ( b + d <= den_max ) {
          numerator = a + c;
          denominator = b + d;
          return;
        } else if ( d > b ) {
          numerator = c;
          denominator = d;
          return;
        } else {
          numerator = a;
          denominator = b;
          return;
        }
      } else if ( number > mediant ) {
        a = a + c;
        b = b + d;
      } else {
        c = a + c;
        d = b + d;
      }
    }

    if ( b > den_max ) {
      numerator = c;
      denominator = d;
      return;
    } else {
      numerator = a;
      denominator = b;
      return;
    }
  }

  ////////////////////////// end of farley algorithm

  ////////////////////////// continued fractions

  template< typename GUM_SCALAR >
  bool CredalNet< GUM_SCALAR >::__less ( const double &a, const double &b, const int &c, const int &d ) const {
    return ( a * d < b * c );
  }

  template< typename GUM_SCALAR >
  bool CredalNet< GUM_SCALAR >::__leq ( const double &a, const double &b, const int &c, const int &d ) const {
    return ( a *d <= b * c );
  }

  template< typename GUM_SCALAR >
  int CredalNet< GUM_SCALAR >::__matches ( const double &a, const double &b, const int &alpha_num, const int &d_num, const int &denum ) const {
    if ( __leq ( a, b, alpha_num - d_num, denum ) ) return 0;

    if ( __less ( a, b, alpha_num + d_num, denum ) ) return 1;

    return 0;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__find_exact_left ( const double &p_a, const double &q_a, const double &p_b, const double &q_b, int64_t &num, int64_t &den, const int &alpha_num, const int &d_num, const int &denum ) const {
    double k_num = denum * p_b - ( alpha_num + d_num ) * q_b;
    double k_denum = ( alpha_num + d_num ) * q_a - denum * p_a;
    double k = ( ( double ) k_num / k_denum ) + 1;
    num = p_b + k * p_a;
    den = q_b + k * q_a;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__find_exact_right ( const double &p_a, const double &q_a, const double &p_b, const double &q_b, int64_t &num, int64_t &den, const int &alpha_num, const int &d_num, const int &denum ) const {
    double k_num = -denum * p_b - ( alpha_num - d_num ) * q_b;
    double k_denum = - ( alpha_num - d_num ) * q_a + denum * p_a;
    double k = ( ( double ) k_num / k_denum ) + 1;
    num = p_b + k * p_a;
    den = q_b + k * q_a;
  }

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__fracC ( int64_t &numerator, int64_t &denominator, const int &alpha_num, const int &d_num, const int &denum ) const {
    double p_a = 0;
    double q_a = 1;
    double p_b = 1;
    double q_b = 1;

    double x, x_num, x_denum, new_p_a, new_q_a, new_p_b, new_q_b;
    int aa, bb;

    while ( true ) {
      x_num = denum * p_b - alpha_num * q_b;
      x_denum = -denum * p_a + alpha_num * q_a;
      x = ( int ) ( ( x_num + x_denum - 1 ) / x_denum );

      aa = __matches ( p_b + x * p_a, q_b + x * q_a, alpha_num, d_num, denum );
      bb = __matches ( p_b + ( x - 1 ) * p_a, q_b + ( x - 1 ) * q_a, alpha_num, d_num, denum );

      if ( aa || bb ) {
        __find_exact_left ( p_a, q_a, p_b, q_b, numerator, denominator, alpha_num, d_num, denum );
        break;
      }

      new_p_a = p_b + ( x - 1 ) * p_a;
      new_q_a = q_b + ( x - 1 ) * q_a;
      new_p_b = p_b + x * p_a;
      new_q_b = q_b + x * q_a;

      p_a = new_p_a;
      p_b = new_p_b;
      q_a = new_q_a;
      q_b = new_q_b;

      x_num = alpha_num * q_b - denum * p_b;
      x_denum = -alpha_num * q_a + denum * p_a;
      x = ( int ) ( ( x_num + x_denum - 1 ) / x_denum );

      aa = __matches ( p_b + x * p_a, q_b + x * q_a, alpha_num, d_num, denum );
      bb = __matches ( p_b + ( x - 1 ) * p_a, q_b + ( x - 1 ) * q_a, alpha_num, d_num, denum );

      if ( aa || bb ) {
        __find_exact_right ( p_a, q_a, p_b, q_b, numerator, denominator, alpha_num, d_num, denum );
        break;
      }

      new_p_a = p_b + ( x - 1 ) * p_a;
      new_q_a = q_b + ( x - 1 ) * q_a;
      new_p_b = p_b + x * p_a;
      new_q_b = q_b + x * q_a;

      p_a = new_p_a;
      p_b = new_p_b;
      q_a = new_q_a;
      q_b = new_q_b;
    }
  }

  //////////////////////////// end of continued fractions

  //////////////////////////// custom decimal -> fractional

  template< typename GUM_SCALAR >
  void CredalNet< GUM_SCALAR >::__frac ( int64_t &numerator, int64_t &denominator, const GUM_SCALAR &number ) const {
    double l = log10 ( abs ( number ) );

    int d1 = l + 1.000001;
    int d2 = ( d1 <= 0 ) ? 4 : 4 - d1;

    double dd = pow ( 10, d2 );
    int64_t di = dd;

    if ( di < dd )
      di++;

    double nd = number * di;
    int64_t ni = nd;

    if ( ni < nd )
      ni++;

    numerator = ni;
    denominator = di;

    int64_t a ( numerator ), b ( denominator ), t;

    while ( b != 0 ) {
      t = b;
      b = a % b;
      a = t;
    }

    numerator /= a;
    denominator /= a;
  }

  //////////////////////////// end of custom decimal -> fractional

} // namespace cn
