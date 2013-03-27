//#include <agrum/CN/LoopyPropagation.h>
namespace gum {


template<typename GUM_SCALAR>
const gum::BayesNet<GUM_SCALAR>& LoopyPropagation<GUM_SCALAR>::bn() const
{
  return *(this->bnet);
}

template<typename GUM_SCALAR>
const gum::BayesNet<GUM_SCALAR>& LoopyPropagation<GUM_SCALAR>::bn_org() const
{
  return cn->src_bn();
}
/*
template<typename GUM_SCALAR>
const gum::BayesNet<GUM_SCALAR>& LoopyPropagation<GUM_SCALAR>::bn_min() const
{
  return *((*(this->cn)).get_BN_min());
}

template<typename GUM_SCALAR>
const gum::BayesNet<GUM_SCALAR>& LoopyPropagation<GUM_SCALAR>::bn_max() const
{
  return *((*(this->cn)).get_BN_max());
}
*/
template<typename GUM_SCALAR>
const std::vector< std::vector<GUM_SCALAR> >& LoopyPropagation<GUM_SCALAR>::get_CPT_min() const
{
  return cn->get_CPT_min();
}

template<typename GUM_SCALAR>
const std::vector< std::vector<GUM_SCALAR> >& LoopyPropagation<GUM_SCALAR>::get_CPT_max() const
{
  return cn->get_CPT_max();
}

template<typename GUM_SCALAR>
const gum::CredalNet<GUM_SCALAR>& LoopyPropagation<GUM_SCALAR>::get_cn() const
{
  return *(this->cn);
}

template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::saveInference(const std::string &path) 
{
  //std::cout << "\n\n\t\t SAVING Inference results\n\n" << std::endl;
  std::string path_name = path.substr(0,path.size()-4);
  path_name = path_name + ".res";
  
  
  std::ofstream res(path_name.c_str(), std::ios::out | std::ios::trunc);
  
  
  if(!res.good())
  {
    GUM_ERROR ( NotFound, "LoopyPropagation<GUM_SCALAR>::saveInference(std::string & path) : could not open file : " + path_name )
    //std::cout << "LoopyPropagation<GUM_SCALAR>::saveInference(std::string & path) : impossible d'ouvrir le fichier res" << std::endl;
    //return;
  }
  
  std::string ext = path.substr(path.size()-3,path.size());
  if(std::strcmp(ext.c_str(),"evi") == 0)
  {
    std::ifstream evi(path.c_str(), std::ios::in);
    std::string ligne;
    if(!evi.good()) 
    {
      GUM_ERROR ( NotFound, "LoopyPropagation<GUM_SCALAR>::saveInference(std::string & path) : could not open file : " + ext )
      //std::cout << "fichier evidence introuvable" << std::endl;
      //return;
    }
    while(evi.good())
    {
      getline(evi, ligne);
      res << ligne << "\n";
    }
    evi.close();
  }  
  
  res << "[RESULTATS]" << "\n";
   
  for(DAG::NodeIterator it = this->bn().beginNodes(); it != this->bn().endNodes(); ++it)
  {	
    // calcul distri posteriori
    
    GUM_SCALAR msg_p_min = 1.0;
    GUM_SCALAR msg_p_max = 0.0;
    
    // cas evidence, calcul immediat
    if( this->_evidence.exists(*it) )
    {
      if( this->_evidence[*it][1] == (GUM_SCALAR) 0. )
	msg_p_min = (GUM_SCALAR) 0.;
      else if( this->_evidence[*it][1] == (GUM_SCALAR) 1. )
	msg_p_min = (GUM_SCALAR) 1.;
      
      msg_p_max = msg_p_min;
    }
    // sinon depuis node P et node L
    else
    {
	GUM_SCALAR min = this->_NodesP_min[*it];
	GUM_SCALAR max;
	
	if( this->_NodesP_max.exists(*it) )
	  max = this->_NodesP_max[*it];
	else
	  max = min;
	
	GUM_SCALAR lmin = this->_NodesL_min[*it];
	GUM_SCALAR lmax;
	
	if( this->_NodesL_max.exists(*it) )
	  lmax = this->_NodesL_max[*it];
	else
	  lmax = lmin;
      	
	// cas limites sur min
	/*if(min == (GUM_SCALAR) 0. && lmin == _INF)
	  std::cout << "proba ERR : pi = 0, l = inf" << std::endl;*/
	if( min == _INF && lmin == (GUM_SCALAR) 0. )
	  std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
	
	if ( /*min != (GUM_SCALAR) 0. &&*/ lmin == _INF  ) // cas infini
	  msg_p_min = (GUM_SCALAR) 1.;
	else if( min == (GUM_SCALAR) 0. || lmin == (GUM_SCALAR) 0.)
	  msg_p_min = (GUM_SCALAR) 0;
	else
	  msg_p_min = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / min - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmin ));
	
	// cas limites sur max	
	/*if(max == (GUM_SCALAR) 0. && lmax == _INF)
	  std::cout << "proba ERR : pi = 0, l = inf" << std::endl;*/
	if( max == _INF && lmax == (GUM_SCALAR) 0. )
	  std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
	
	if ( /*max != (GUM_SCALAR) 0. &&*/ lmax == _INF  ) // cas infini
	  msg_p_max = (GUM_SCALAR) 1.;
	else if( max == (GUM_SCALAR) 0. || lmax == (GUM_SCALAR) 0.)
	  msg_p_max = (GUM_SCALAR) 0;
	else
	  msg_p_max = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / max - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmax ));
    }
    
    if(msg_p_min != msg_p_min && msg_p_max == msg_p_max)
	{
	  msg_p_min = msg_p_max;
	  /*std::cout << std::endl;
	  std::cout << "msg_p_min is NaN" << std::endl;*/
	}
	if(msg_p_max != msg_p_max && msg_p_min == msg_p_min)
	{
	  msg_p_max = msg_p_min;
	  /*std::cout << std::endl;
	  std::cout << "msg_p_max is NaN" << std::endl;*/
	  
	}
	if(msg_p_max != msg_p_max && msg_p_min != msg_p_min)
	{
	  std::cout << std::endl;
	  std::cout << "pas de proba calculable (verifier observations)" << std::endl;
	}
    
    res << "P(" << (this->bn()).variable(*it).name() << " | e) = ";//<< std::endl;
    
    if( this->_evidence.exists(*it) )
      res << "(observe)" << std::endl;
    else
      res << std::endl;
    
    res << "\t\t" << (this->bn()).variable(*it).label(0) << "  [ " << (GUM_SCALAR) 1. - msg_p_max;
    
    if( msg_p_min != msg_p_max )
      res << ", " << (GUM_SCALAR) 1. - msg_p_min <<" ] | ";
    else
      res<<" ] | ";    
       
    res << (this->bn()).variable(*it).label(1) << "  [ "<< msg_p_min;
    if( msg_p_min != msg_p_max )
      res << ", " << msg_p_max <<" ]"<<std::endl;
    else
      res<<" ]"<<std::endl;
  }
  
  res.close();
}


/**
* pour les fonctions suivantes, les GUM_SCALAR min/max doivent etre initialises
* (min a 1 et max a 0) pour comparer avec les resultats intermediaires
*/

/**
* une fois les cpts marginalises sur X et Ui, on calcul le min/max,
*/
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_compute_ext(GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, GUM_SCALAR &num_min, GUM_SCALAR &num_max, GUM_SCALAR &den_min, GUM_SCALAR &den_max)
{
  GUM_SCALAR old_msg_min = msg_l_min;
  GUM_SCALAR old_msg_max = msg_l_max;
      
  GUM_SCALAR num_min_tmp;
  GUM_SCALAR den_min_tmp;
  GUM_SCALAR num_max_tmp;
  GUM_SCALAR den_max_tmp;
  
  GUM_SCALAR res_min = 1.0, res_max = 0.0;
  
  for(unsigned int i = 0; i < lx.size(); i++)
  {
    bool non_defini_min = false;
    bool non_defini_max = false;
    // cas infini
    if( lx[i] == _INF /*< (GUM_SCALAR) 0.*/)
    {
      num_min_tmp = num_min /*- (GUM_SCALAR) 1.*/;
      den_min_tmp = den_max /*- (GUM_SCALAR) 1.*/;
      num_max_tmp = num_max /*- (GUM_SCALAR) 1.*/;
      den_max_tmp = den_min /*- (GUM_SCALAR) 1.*/;
    }
    else if( lx[i] == (GUM_SCALAR) 1. )
    {
      num_min_tmp = (GUM_SCALAR) 1.;
      den_min_tmp = (GUM_SCALAR) 1.;
      num_max_tmp = (GUM_SCALAR) 1.;	
      den_max_tmp = (GUM_SCALAR) 1.;
    }
    else if( lx[i] > (GUM_SCALAR) 1. )
    {	
      num_min_tmp = num_min + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      den_min_tmp = den_max + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      num_max_tmp = num_max + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      den_max_tmp = den_min + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
    }    
    else if( lx[i] < (GUM_SCALAR) 1. )
    {
      num_min_tmp = num_max + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      den_min_tmp = den_min + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      num_max_tmp = num_min + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
      den_max_tmp = den_max + (GUM_SCALAR)1. / (lx[i] - (GUM_SCALAR)1.);
    }
    
    //std::cout << "LAMBDA" << std::endl;    
    //std::cout <<"num_min : "<< num_min << "num_max : " << num_max << " den min " << den_min << " den max " << den_max << std::endl;
    //std::cout << num_min_tmp << "/" <<den_min_tmp << " " <<num_max_tmp << "/" <<den_max_tmp << std::endl;
          
    if(den_min_tmp == (GUM_SCALAR) 0. && num_min_tmp == (GUM_SCALAR) 0.)
    {
      non_defini_min = true;
      //res_min = (GUM_SCALAR) 1.;
     /* std::cout << std::endl;
      std::cout << "ERR MSG L min non defini 0/0, msg actuel : "<< msg_l_min<<", "<<msg_l_max;
      std::cout << " lx[i] : " << lx[i]<< " lx "<<lx<<std::endl;*/
      //break;
    }
    else if(den_min_tmp == (GUM_SCALAR) 0. && num_min_tmp != (GUM_SCALAR) 0.)
      res_min = _INF;//(GUM_SCALAR) -1.;
    else if (den_min_tmp != _INF || num_min_tmp != _INF)
      res_min = num_min_tmp / den_min_tmp;
    
    if(den_max_tmp == (GUM_SCALAR) 0. && num_max_tmp == (GUM_SCALAR) 0.)
    {
      non_defini_max = true;
      //res_max = 1;
    /*  std::cout << std::endl;
      std::cout << "ERR MSG L max non defini 0/0 ";
      std::cout << " lx[i] : " << lx[i]<< " lx "<<lx<<std::endl;*/
      //break;
    }
    else if(den_max_tmp == (GUM_SCALAR) 0. && num_max_tmp != (GUM_SCALAR) 0.)
      res_max = _INF;//(GUM_SCALAR) -1.;
    else if (den_max_tmp != _INF || num_max_tmp != _INF)
      res_max = num_max_tmp / den_max_tmp;
    
    if(non_defini_max && non_defini_min)
    {
      continue;
    }
    else if(non_defini_min && !non_defini_max)
    {
      //std::cout << "msg : "<<_INF<<std::endl;
      res_min = res_max;
    }
    else if(non_defini_max && !non_defini_min)
    {
      //std::cout << "msg : "<<res_min<<std::endl;
      res_max = res_min;
    }
    
    if(res_min <= (GUM_SCALAR) 0.)
      res_min = (GUM_SCALAR) 0.;
    if(res_max <= (GUM_SCALAR) 0.)
      res_max = (GUM_SCALAR) 0.;
    
    /*
    std::cout << "res min / max" << std::endl;
    std::cout << res_min << " | " << res_max << std::endl;
    
    std::cout << "msg min / max" << std::endl;
    std::cout << msg_l_min << " | "<< msg_l_max << std::endl;*/
    
    //std::cout << res_min << " " << res_max<<std::endl;
    
    if(msg_l_min == msg_l_max && msg_l_min == (GUM_SCALAR) -2.)
    {
	msg_l_min = res_min;
	msg_l_max = res_max;
    }
    
    if( (res_max > msg_l_max ))//&& msg_l_max > (GUM_SCALAR) 0.) || res_max == _INF;/*(GUM_SCALAR) -1.*/)
      msg_l_max = res_max;
    
    if( (res_min < msg_l_min ))//&& res_min != (GUM_SCALAR) -1.) || msg_l_min == _INF;/*(GUM_SCALAR) -1.*/)
      msg_l_min = res_min;
    
  }  
  
 /* if(non_defini_min && non_defini_max)
  {
    msg_l_min = old_msg_min;
    msg_l_max = old_msg_max;    
  }*/
  
}

/**
* extremes pour une combinaison des parents, message vers parent
*/
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_compute_ext(std::vector< std::vector<GUM_SCALAR> > &combi_msg_p, const gum::NodeId &id, GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, const gum::Idx &pos)
{
 /* GUM_SCALAR min = msg_l_min;
  GUM_SCALAR max = msg_l_max;*/
  
  GUM_SCALAR num_min = (GUM_SCALAR)0.;
  GUM_SCALAR num_max = (GUM_SCALAR)0.;
  GUM_SCALAR den_min = (GUM_SCALAR)0.;
  GUM_SCALAR den_max = (GUM_SCALAR)0.;
  
  int taille = combi_msg_p.size();
  
  std::vector< typename std::vector<GUM_SCALAR>::iterator > it(taille);
  for(int i = 0; i < taille; i++)
    it[i] = combi_msg_p[i].begin();
  
  int pas = 2; //= (int)pow(2,(int)pos);
  int pp = pos;
  cn->intPow(pas, pp); // beware : pp is modified, not pas
  
  /*int pas = pos;
  int_2_pow(pas);*/
  
  int combi_den = 0;
  int combi_num = pp;//pas;
  
  //std::cout << "COMPUTE EXT" << std::endl;
  //std::cout << "msg_p " << combi_msg_p << std::endl;
  //std::cout << "cpt : " << get_CPT_min()[id] << std::endl;
  
  // marginalisation
  while(it[taille-1] != combi_msg_p[taille-1].end())
  {
    GUM_SCALAR prod = (GUM_SCALAR)1.;
    //std::cout << "prod : ";
    for(int k = 0; k < taille; k++)
    {
      //std::cout << *it[k] << " ";
      prod *= *it[k];
    }
    //std::cout << std::endl;
    //std::cout << "den : " << (get_CPT_min()[id][combi_den]) << std::endl;
    den_min += (GUM_SCALAR) (get_CPT_min()[id][combi_den] * prod);
    den_max += (GUM_SCALAR) (get_CPT_max()[id][combi_den] * prod);
    
    //std::cout << "num : " << (get_CPT_min()[id][combi_num]) << std::endl;
    num_min += (GUM_SCALAR) (get_CPT_min()[id][combi_num] * prod);
    num_max += (GUM_SCALAR) (get_CPT_max()[id][combi_num] * prod);
    
    combi_den++;
    combi_num++;
    
    if( combi_den % pp/*pas*/ == 0)
    {
      combi_den += pp;//pas;
      combi_num += pp;//pas;
    }
        
    // incrementation de 1
    ++it[0];
    for(int i = 0; (i<taille-1)&&(it[i] == combi_msg_p[i].end()); ++i)
    {
      it[i] = combi_msg_p[i].begin();
      ++it[i+1];
    }
  }
    
  _compute_ext(msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max);
  /*std::cout << "retour 1 " << std::endl;
  std::cout << min << " | " << max << std::endl;
  msg_l_min = min;
  msg_l_max = max;*/
}

/**
* extremes pour une combinaison des parents, message vers enfant
* marginalisation cpts
*/
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_compute_ext(std::vector< std::vector<GUM_SCALAR> > &combi_msg_p, const gum::NodeId &id, GUM_SCALAR &msg_p_min, GUM_SCALAR &msg_p_max)
{  
  GUM_SCALAR min = (GUM_SCALAR) 0.;
  GUM_SCALAR max = (GUM_SCALAR) 0.;
  
  int taille = combi_msg_p.size();
  
  std::vector< typename std::vector<GUM_SCALAR>::iterator > it(taille);
  for(int i = 0; i < taille; i++)
    it[i] = combi_msg_p[i].begin();
  
  int combi = 0;
  while(it[taille-1] != combi_msg_p[taille-1].end())
  {
    GUM_SCALAR prod = (GUM_SCALAR)1.;
    
    for(int k = 0; k < taille; k++)
      prod *= *it[k];
    
    min += (get_CPT_min()[id][combi] * prod);	// marg min
    max += (get_CPT_max()[id][combi++] * prod); // marg max
    
    //combi++;
        
    // incrementation de 1
    ++it[0];
    for(int i = 0; (i<taille-1)&&(it[i] == combi_msg_p[i].end()); ++i)
    {
      it[i] = combi_msg_p[i].begin();
      ++it[i+1];
    }
  }
  
  if(min < msg_p_min) msg_p_min = min;
  if(max > msg_p_max) msg_p_max = max;
}

/**
* enumere combinaisons messages parents, pour message vers enfant
*/
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_enum_combi(std::vector< std::vector< std::vector<GUM_SCALAR> > > &msgs_p, const gum::NodeId &id, GUM_SCALAR &msg_p_min, GUM_SCALAR &msg_p_max)
{
  int taille = msgs_p.size();
  //si LX fournis verifier X = 1 pour min/max => msg_l = 1, return
  
  // ou autre fonction ?
  
  // noeud source
  if(taille == 0)
  {
    msg_p_min = (GUM_SCALAR) get_CPT_min()[id][0];
    msg_p_max = (GUM_SCALAR) get_CPT_max()[id][0];
    
    return;
  }

  unsigned int msgPerm = 1;
  #pragma omp parallel
  {
    GUM_SCALAR msg_pmin = msg_p_min;//_INF;
    GUM_SCALAR msg_pmax = msg_p_max;//0;
    std::vector< std::vector<GUM_SCALAR> > combi_msg_p(taille);

    unsigned int confs = 1;
    #pragma omp for
    for(int i = 0; i < taille; i++)
      confs *= msgs_p[i].size();

    #pragma omp atomic
    msgPerm *= confs;
    #pragma omp barrier
    #pragma omp flush(msgPerm)

    #pragma omp for
    for ( unsigned int j = 0; j < msgPerm; j++ ) {
      // get jth msg :
      unsigned int jvalue = j;
      for( int i = 0; i < taille; i++ ) {
        if ( msgs_p[i].size() == 2 ) {
          combi_msg_p[i] = ( jvalue & 1 ) ? msgs_p[i][1] : msgs_p[i][0];
          jvalue /= 2;
        }
        else
          combi_msg_p[i] = msgs_p[i][0];
      }
     
      _compute_ext(combi_msg_p, id, msg_pmin, msg_pmax);

      combi_msg_p.clear();   
    }

    // since min is _INF and max is 0 at init, there is no issue having more threads here than during for loop
    #pragma omp critical(msgpminmax)
    {
      #pragma omp flush(msg_p_min)
      #pragma omp flush(msg_p_max)
      if ( msg_p_min > msg_pmin )
        msg_p_min = msg_pmin;
      if ( msg_p_max < msg_pmax )
        msg_p_max = msg_pmax;
    }
  }
  return;

  // sinon enumerer les combi de list
  std::vector< typename std::vector< std::vector<GUM_SCALAR> >::iterator > it(taille);
  
  for(int i = 0; i < taille; i++)
    it[i] = ((msgs_p[i])).begin();
   
  while(it[0] != ((msgs_p[0])).end())
  {
    std::vector< std::vector<GUM_SCALAR> > combi_msg_p(taille);
    for(int i = 0; i < taille; i++)
      combi_msg_p[i] = *it[i];
    
    /*
    std::stringstream ss;
    ss << cpt;
    std::stringstream ss2;
    ss2 <<  (int)pow(2,taille);
    
    std::string aff = " config message P : " + ss.str() + " / " + ss2.str()+"          ";
    std::cout << aff;//<<"\r";  
    for(int k = 0; k < aff.size(); k++)
      std::cout << "\b";
    std::cout.flush();
    */
    _compute_ext(combi_msg_p, id, msg_p_min, msg_p_max);
    //cpt++;
    
    if(msg_p_min == (GUM_SCALAR)0. && msg_p_max == (GUM_SCALAR)1.)
      return;
    
    combi_msg_p.clear();

    ++it[taille-1];
    for(int i = taille-1; (i>0)&&(it[i] == ((msgs_p[i])).end()); --i)
    {
      it[i] = ((msgs_p[i])).begin();
      ++it[i-1];
    }
  }
}

/**
* comme precedemment mais pour message parent, vraisemblance prise en compte
*/

template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_enum_combi(std::vector< std::vector< std::vector<GUM_SCALAR> > > &msgs_p, const gum::NodeId &id, GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, const gum::Idx &pos)
{
  int taille = msgs_p.size();
  /*
  GUM_SCALAR min = msg_l_min;
  GUM_SCALAR max = msg_l_max;*/
  
  // un seul parent, celui qui recoit le message
  if( taille == 0 )
  {
    GUM_SCALAR num_min = (GUM_SCALAR)get_CPT_min()[id][1];
    GUM_SCALAR num_max = (GUM_SCALAR)get_CPT_max()[id][1];
    GUM_SCALAR den_min = (GUM_SCALAR)get_CPT_min()[id][0];
    GUM_SCALAR den_max = (GUM_SCALAR)get_CPT_max()[id][0];
        
    _compute_ext(msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max);
    /*
    msg_l_min = min;
    msg_l_max = max;*/
    return;
  }

  /////////////////////////////////////////////////////////

  unsigned int msgPerm = 1;
  #pragma omp parallel
  {
    // -2 by default ?!
    GUM_SCALAR msg_lmin = msg_l_min;//_INF;
    GUM_SCALAR msg_lmax = msg_l_max;//0;
    std::vector< std::vector<GUM_SCALAR> > combi_msg_p(taille);

    unsigned int confs = 1;
    #pragma omp for
    for(int i = 0; i < taille; i++)
      confs *= msgs_p[i].size();

    #pragma omp atomic
    msgPerm *= confs;
    #pragma omp barrier
    #pragma omp flush(msgPerm)

    // direct binary representation of config, no need for iterators
    #pragma omp for
    for ( unsigned int j = 0; j < msgPerm; j++ ) {
      // get jth msg :
      unsigned int jvalue = j;
      for( int i = 0; i < taille; i++ ) {
        if ( msgs_p[i].size() == 2 ) {
          combi_msg_p[i] = ( jvalue & 1 ) ? msgs_p[i][1] : msgs_p[i][0];
          jvalue /= 2;
        }
        else
          combi_msg_p[i] = msgs_p[i][0];
      }
     
      _compute_ext(combi_msg_p, id, msg_lmin, msg_lmax, lx, pos);

      combi_msg_p.clear();
    }

    // there may be more threads here than in the for loop, therefor positive test is NECESSARY (init is -2)
    #pragma omp critical(msglminmax)
    {
      #pragma omp flush(msg_l_min)
      #pragma omp flush(msg_l_max)
      if ( ( msg_l_min > msg_lmin || msg_l_min == -2 ) && msg_lmin > 0 )
        msg_l_min = msg_lmin;
      if ( ( msg_l_max < msg_lmax || msg_l_max == -2 ) && msg_lmax > 0 )
        msg_l_max = msg_lmax;
    }

  }

  return;

  ///////////////////////////////////////////////// old version ///
  
  // sinon enumerer les combi de msgs_p
  std::vector< typename std::vector< std::vector<GUM_SCALAR> >::iterator > it(taille);
  
  for(int i = 0; i < taille; i++)
    it[i] = ((msgs_p[i])).begin();
  
  //std::stringstream ss2;
  //ss2 <<  (int)pow(2,taille);
  
  int cpt = 1;
  while(it[0] != ((msgs_p[0])).end())
  {
    std::vector< std::vector<GUM_SCALAR> > combi_msg_p(taille);
    for(int i = 0; i < taille; i++)
      combi_msg_p[i] = *it[i];
    /*
    std::stringstream ss;
    ss << cpt;
    
    
    std::string aff = " config message L : " + ss.str() + " / " + ss2.str() +"          ";
    std::cout << aff;//<<"\r";  
    for(int k = 0; k < aff.size(); k++)
      std::cout << "\b";
    std::cout.flush();
    */
    _compute_ext(combi_msg_p, id, msg_l_min, msg_l_max, lx, pos);
    /*
    msg_l_min = min;
    msg_l_max = max;
    std::cout << "apres return "<< msg_l_min << " | " << msg_l_max << std::endl; */
    
    cpt++;
    
    if(msg_l_min == (GUM_SCALAR)0. && msg_l_max == _INF /*msg_l_max == (GUM_SCALAR)-1.*/)
      return;
    
    combi_msg_p.clear();
    
    ++it[taille-1];
    for(int i = taille-1; (i>0)&&(it[i] == ((msgs_p[i])).end()); --i)
    {
      it[i] = ((msgs_p[i])).begin();
      ++it[i-1];
    }
  }
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			make inference 			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::makeInference()
{
	if(this->_InferenceUpToDate) return;
	
	//std::cout << "\n\n\t\tINIT\n" << std::endl;
	
	//_cleanInferenceData();

	_initialize();
	
	this->initApproximationScheme();
	
	//std::cout << "\n\n\n\t\tInference\n" << std::endl;
	
	switch(__inferenceType)
	{
		case ordered:
			_makeInferenceByOrderedArcs();
			break;
			
		case randomOrder:
			_makeInferenceByRandomOrder();
			break;
			
		//case randomEvaluation:
			//_makeInferenceByRandomEvaluation();
			//break;
	}

  _updateMarginals();
	
	/* 
	 * next to ordered, randomOrder, randomEvaluation
	 * it would be possible to add some more inference orders:
	 * 
	 * cocktail - pass the network in topological order and then in reversed topological order.
	 * 
	 * trampStyle: all messages from one node and then skip to a nighbours and the same....
	 * 
	 * 
	 */
	
	//_refreshLMsPIs();
	//this->_calculateEpsilon();
	
	this->_InferenceUpToDate = true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 		clean inference data		////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_cleanInferenceData() 
{
  //this->eraseAllEvidence();
	//this->_invalidateMarginals();
	this->eraseAllEvidence();	
	//this->_oldMarginalMin.clear(); this->_oldMarginalMax.clear();
	this->_ArcsL_min.clear(); this->_ArcsL_max.clear();
	this->_ArcsP_min.clear(); this->_ArcsP_max.clear();
	this->_NodesL_min.clear(); this->_NodesL_max.clear();
	this->_NodesP_min.clear(); this->_NodesP_max.clear();
	
	this->_InferenceUpToDate = false;
	
	if(_msg_l_sent.size() > 0)
	{
	  for(DAG::NodeIterator it = bn().beginNodes(); it != bn().endNodes(); ++it)
	    delete _msg_l_sent[*it];
	}
	_msg_l_sent.clear();
	_update_l.clear();
	_update_p.clear();
	//this->_evidence.clear();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 		insert evidence	////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
template< typename GUM_SCALAR >
void gum::LoopyPropagation< GUM_SCALAR >::insertEvidence( const std::string & path ) {
  InferenceEngine< GUM_SCALAR >::insertEvidence( path );
  _insertEvidence();
}

template< typename GUM_SCALAR >
void gum::LoopyPropagation< GUM_SCALAR >::insertEvidence( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap ) {
  InferenceEngine< GUM_SCALAR >::insertEvidence( eviMap );
  _insertEvidence();
}

template< typename GUM_SCALAR >
void gum::LoopyPropagation< GUM_SCALAR >::_insertEvidence() {
  _cleanInferenceData();

  if ( this->_evidence.size() == 0 )
    return;

  for ( typename gum::Property< std::vector< GUM_SCALAR > >::onNodes::const_iterator it = this->_evidence.begin(); it != this->_evidence.end(); ++it ) {
    bool hardEvidence = false;
    int modal = -1; // should not stay as such
    for( unsigned int mod = 0; mod < it->size(); mod++) {
      if(hardEvidence && (*it)[mod] > 0) {
        GUM_ERROR ( OperationNotAllowed, "void gum::LoopyPropagation< GUM_SCALAR >::_insertEvidence() : not reading hard evidence : " << *it );
      }
      else if(!hardEvidence && (*it)[mod] > 0) {
        hardEvidence = true;
        modal = mod;
      }
    }
    std::vector< GUM_SCALAR > modals(2);
    modals[0] = (modal == 0) ? 1 : 0;
    modals[1] = modal;

    //this->_evidence.insert(it.key(), modal);
    this->_evidence.insert ( it.key(), modals );
  }
}*/

template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::insertEvidence_old(const std::string &path)
{   
  /*for(int i = 0; i < this->bn_org().size(); i++)
    std::cout << this->bn_org().variable(i).name() << std::endl;*/
  
	_cleanInferenceData();
	//eraseAllEvidence();
	
	//Lpmsg Infos;
	
	//std::cout << " Parsing fichier evidence" << std::endl;
	
	std::ifstream evi(path.c_str(), std::ios::in);
	
	if(!evi.good())
	{
    GUM_ERROR ( NotFound, "LoopyPropagation<GUM_SCALAR>::insertEvidence_old(const std::string &path) : could not open evidence file : " + path );
	  //std::cout << " Fichier evidence introuvable, pas d'evidences"<< std::endl;
	  //return;
	}
	std::string ligne, tmp;
	int var_add = -1; // shoud not stay as such
	gum::NodeId var_tmp;
	char *cstr, *p;
	GUM_SCALAR proba;
	
	std::vector< std::vector<gum::NodeId> > bits;// = *((get_cn()).get_BITS());
  bits.resize(this->bn().size());
  for(unsigned int var = 0; var < this->bn().size(); var++) {
    bits[var] = std::vector<gum::NodeId>(1,(gum::NodeId)var);
  }

	
	while(evi.good() && std::strcmp(ligne.c_str(),"[EVIDENCE]") != 0)
	    getline(evi, ligne);
	
	while( evi.good() )
	{
	  getline(evi, ligne);
	 
	  if( std::strcmp(ligne.c_str(),"[QUERY]") == 0) break;
	  if(ligne.size() == 0 ) continue;
	  
	  cstr = new char [ligne.size()+1];
	  strcpy(cstr, ligne.c_str());
	  
	  std::vector< gum::NodeId > list_lx;
	  // = new gum::Potential<GUM_SCALAR>();
	  
	  // tokens
	  p = strtok (cstr, " ");
	  bool var = true;
	  while(p!=NULL)
	  {
	    tmp = p;
	    if(var)
	    {
	      for(unsigned int i = 0; i < this->bn_org().size(); i++)
	      {
		if(tmp.compare(this->bn_org().variable(i).name()) == 0)
		{
		  var_add = i;
		  //std::cout << "\tevidence " << tmp;
		  //var_tmp = this->bn_org().variable(i);
		  //std::cout << bits << std::endl;
		  for(unsigned int b = 0; b < bits[i].size(); b++)
		  {
		    
		    list_lx.push_back(this->bn().nodeId(this->bn().variable(bits[i][b])));
		    //msg * mon_msg = new msg();;
		    //msg * lx = new msg();this->_calculateEpsilon
		    //gum::Potential<GUM_SCALAR> * ptr = new gum::Potential<GUM_SCALAR>();
		    //*ptr << this->bn().variable(bits[i][b]);
		    //(*mon_msg).push_back(ptr);
		    //list_lx.push_back(mon_msg);
		  }
		  var = false;
		  break;
		  /*var_tmp =  this->bn_org().nodeId( this->bn_org().variable(i));
		  
		  std::cout << var_tmp << std::endl;
		  std::cout << this->bn_org().variable(i).name() << std::endl;*/
		}
	      }
	      // pour les autres evidences (variable indic, etc)
	      for(unsigned int i = 0; i < this->bn().size(); i++)
	      {
		if(tmp.compare(this->bn().variable(i).name()) == 0)
		{
		  //std::cout << "\tevidence " << tmp << " = ";
		  p=strtok(NULL," ");
		  tmp = p;
		  proba = atof(tmp.c_str());
		  //std::cout << proba << std::endl;
		  this->_evidence.insert(this->bn().nodeId(this->bn().variable(i)), std::vector< GUM_SCALAR > (2, proba) ); // bit = 1
		    
		  break;
		}
	      }
	      
	    }
	    else
	    {
	      //std::vector<GUM_SCALAR> t(this->bn_org().variable(var_add).domainSize(),(GUM_SCALAR)0);
	      
	      proba = atof(tmp.c_str());
	      int val = 0;
	      
	      if(proba + 1 > this->bn_org().variable(var_add).domainSize()) 
	      {
		std::cout << " : cardinalite incompatible avec evidence !" << std::endl;
		var = true;
		continue;
	      }
	      
	      //std::cout << " = " << proba << std::endl;
	      for(int i = list_lx.size() - 1; i >= 0; i--)
	      {
		//std::vector<GUM_SCALAR> t(2,0);		
		
		int puiss = (int)pow(2,i);
		//val += (int)pow(2,i);
		if(val + puiss <= proba)
		{
		  val += puiss;
		  this->_evidence.insert(this->bn().nodeId(this->bn().variable( bits[var_add][i])), std::vector< GUM_SCALAR > (2, (GUM_SCALAR)1.) ); // bit = 1
		}
		else
		  this->_evidence.insert(this->bn().nodeId(this->bn().variable( bits[var_add][i])), std::vector< GUM_SCALAR > (2, (GUM_SCALAR)0.) ); // bit = 0
		
		//std::cout << "\t\tbit "<< i <<" = " << this->_evidence[this->bn().nodeId(this->bn().variable( bits[var_add][i]))] << std::endl;
		//(*((*(list_lx[i]))[0])).fillWith(t);
		//std::cout << "\t\ttable cree : " << (*((*(list_lx[i]))[0])) << std::endl;
		
		//Infos.insert(list_lx[i]);
	      }
	      
	      //t[proba] = 1;
	      //gum::easyFill(*ptr, (int)this->bn_org().variableFromName(var_tmp).domainSize(), t);
	      //(*ptr).fillWith(t);
	      //(*lx).push_back(ptr);
	      
	    }
	    p=strtok(NULL," ");
	  } // fin lecture ligne
	  delete[] p;
	  delete[] cstr;
	  
	  
	}
	//std::cout << " Parsing termine" << std::endl;
	evi.close();	
	
	//this->insertEvidence(Infos);
	
	/*
	for(typename Lpmsg::const_iterator it = a_evidences.begin(); it != a_evidences.end(); ++it)
	{
	  if( ((*(*it))[0])->nbrDim() != 1)
	    GUM_ERROR(OperationNotAllowed, "Dimension of at least one evidence is different to 1. " );
	  
	  NodeId iN = this->bn().nodeId( ((*(*it))[0])->variable(0) );
		if(this->_evidence.exists(iN) ) this->_evidence.erase(iN);
		this->_evidence.insert(iN, *it);
	  */
	  
	  /*
		if((*it)->nbrDim() != 1) GUM_ERROR(OperationNotAllowed, "Dimension of at least one evidence is different to 1. " );
		 
		NodeId iN = this->bn().nodeId( (*it)->variable(0) );
		if(this->_evidence.exists(iN) ) this->_evidence.erase(iN);
		this->_evidence.insert(iN, *it);
		*/
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			initialize	 			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_initialize()
{
  gum::DAG graphe = this->bn().dag();
  	const Sequence<NodeId> & topoNodes = this->bn().topologicalOrder();
	for(Sequence<NodeId>::const_iterator it = topoNodes.begin(); it != topoNodes.end(); ++it)
	{
    // msg from X to Y comment
		std::cout << "\t"<< (this->bn()).variable(*it).name() << "   ";std::cout.flush();// << std::endl;
    
    
		
		
		////////////////////////////////
		_update_p.insert(*it,false);
		_update_l.insert(*it,false);
				
		gum::NodeSet * _parents = new gum::NodeSet();
		_msg_l_sent.insert(*it, _parents);
		
		
	      // accelerer init pour evidences
		if(this->_evidence.exists(*it))
		{
      std::cout << " evi : ";
		  active_nodes_set.insert(*it);
		  _update_l.set(*it,true);
		  _update_p.set(*it,true);
		  if(this->_evidence[*it][1] == (GUM_SCALAR) 1.)
		  {
        std::cout << " = 1 ";
		    this->_NodesL_min.insert(*it, _INF/*(GUM_SCALAR) -1.*/);
		    //this->_NodesL_min.insert(*it, (GUM_SCALAR) 1.);
		    
		    
		    this->_NodesP_min.insert(*it, (GUM_SCALAR) 1.);
		  }
		  else if (this->_evidence[*it][1] == (GUM_SCALAR) 0.)
		  {
        std::cout << " = 0 ";
		    this->_NodesL_min.insert(*it, this->_evidence[*it][1]); // soit 0
		    this->_NodesP_min.insert(*it, (GUM_SCALAR) 0.);
		  }
		  
      std::vector<GUM_SCALAR> marg(2);
      marg[1] = this->_NodesP_min[*it];
      marg[0] = 1 - marg[1];
      std::cout << marg << std::endl;
		  this->_oldMarginalMin.insert( *it, marg );
      this->_oldMarginalMax.insert( *it, marg );
		  // msg from X to Y comment
      //std::cout<<"\r";std::cout.flush();
		  continue;
		}
		
		
		gum::NodeSet _par = graphe.parents(*it);
		gum::NodeSet _enf = graphe.children(*it);
		
		if(_par.size()==0)
		{
		  active_nodes_set.insert(*it);
		  _update_p.set(*it,true);
		  _update_l.set(*it,true);
		}
		// variables indicatrices seulement, prendre une couche avant
		// ou alors verifier sur l'ancien DAG (sans indicatrices)
		if(_enf.size()==0) 
		{
		  active_nodes_set.insert(*it);  
		  _update_p.set(*it,true);
		  _update_l.set(*it,true);
		}
		
		
		/**
		* messages and so parents need to be read in order of appearance
		* use potentials instead of dag
		*/		
		
		const gum::Potential<GUM_SCALAR> * parents = &bn().cpt(*it);
		
		std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
		std::vector< std::vector<GUM_SCALAR> > msg_p;
		std::vector<GUM_SCALAR> distri(2);
		
		// +1 from start to avoid counting itself
		for(gum::Sequence<const gum::DiscreteVariable*>::const_iterator jt = ++((*parents).begin()); jt != (*parents).end(); ++jt)
		{
		  // compute probability distribution to avoid doing it multiple times (at each combination of messages)
		  distri[1] = this->_NodesP_min[bn().nodeId(**jt)];
		  distri[0] = (GUM_SCALAR)1. - distri[1];
		  msg_p.push_back(distri);
		  
		  if( (this->_NodesP_max).exists(bn().nodeId(**jt)) )
		  {    
		    distri[1] = this->_NodesP_max[bn().nodeId(**jt)];
		    distri[0] = (GUM_SCALAR)1. - distri[1];
		    msg_p.push_back(distri);
		  }
		  msgs_p.push_back(msg_p);
		  msg_p.clear();
		}
		
			
		GUM_SCALAR msg_p_min = (GUM_SCALAR) 1.;
		GUM_SCALAR msg_p_max = (GUM_SCALAR) 0.;
		
		_enum_combi(msgs_p, *it, msg_p_min, msg_p_max);
		/* msg from X to Y comment
     std::cout<<"\r";std::cout.flush();
     */
		//std::cout << msg_p_min << " | " << msg_p_max << std::endl;
		
		if(msg_p_min <= (GUM_SCALAR) 0.)
		  msg_p_min =  (GUM_SCALAR) 0.;
		if(msg_p_max <= (GUM_SCALAR) 0.)
		  msg_p_max =  (GUM_SCALAR) 0.;
		
		
		(this->_NodesP_min).insert(*it, msg_p_min);
    std::vector<GUM_SCALAR> marg(2);
    marg[1] = msg_p_min;
    marg[0] = 1 - msg_p_min;

		this->_oldMarginalMin.insert( *it, marg );
		std::cout << "\n" << marg << std::endl;

		
		if(msg_p_min != msg_p_max)
		{
    marg[1] = msg_p_max;
    marg[0] = 1 - msg_p_max;
		 (this->_NodesP_max).insert(*it, msg_p_max);
    }
		this->_oldMarginalMax.insert(*it, marg );

    std::cout << marg << std::endl;


		/*
		if(this->_evidence.exists(*it))
		{
		  if(this->_evidence[*it][1] == (GUM_SCALAR) 1.)
		    this->_NodesL_min.insert(*it, _INF);
		  else
		    this->_NodesL_min.insert(*it, this->_evidence[*it][1]);
		}
		else*/
		  this->_NodesL_min.insert(*it, (GUM_SCALAR) 1.);
		
		//std::cout << this->_NodesL_min[*it] << std::endl;
	    
	}
	
	for(DAG::ArcIterator it = this->bn().beginArcs(); it != this->bn().endArcs(); ++it) 
	{		
		this->_ArcsP_min.insert(*it, this->_NodesP_min[it->tail()]);
		if( (this->_NodesP_max).exists(it->tail()) )
		  this->_ArcsP_max.insert(*it, this->_NodesP_max[it->tail()]);
		  
		this->_ArcsL_min.insert(*it, this->_NodesL_min[it->tail()]);
	}
  // msg from X to Y comment
	std::cout << std::endl;

	//std::cout << "                   " << std::endl;
	//std::cout << " DONE " << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 	make inference by ordered arcs	////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_makeInferenceByOrderedArcs()
{	
	gum::DAG graphe = this->bn().dag();
	unsigned i = 0; GUM_SCALAR eps; //should be changed to the burn in option from new ApproximationScheme.
	int cpt = 1; 
	do	
	{
    // msg from X to Y comment
	  std::cout << "\t\tIteration : " << i << std::endl;
		std::cout << std::endl;
		
		int taille = active_nodes_set.size();
		cpt = 0;
		for(gum::NodeSetIterator it = active_nodes_set.begin(); it != active_nodes_set.end(); ++it)
		{
		  gum::NodeSet _enfants = graphe.children(*it);
		  int cpt_in = 1;
		  // msg from X to Y comment
      int taille_in = _enfants.size();
		  for(gum::NodeSetIterator jt = _enfants.begin(); jt != _enfants.end(); ++jt)
		  {		    
		    // msg from X to Y comment
        std::cout << "\tnoeud "<<cpt<<" / "<< taille<<" "<<this->bn().variable(*it).name()<<" message vers enfant "<< cpt_in << " / " << taille_in <<" " << this->bn().variable(*jt).name() << "        ";
		    std::cout <<"\r"; 
		    //std::cout<<std::endl;
		    std::cout.flush();
        
		    _msgP(*it,*jt);
		    //actifs_suivants.insert(*jt);
		    cpt_in++;
		  }
      cpt += cpt_in;
		  cpt_in = 1;
		  gum::NodeSet _parents = graphe.parents(*it);
		  // msg from X to Y comment
      taille_in = _parents.size();
		  for(gum::NodeSetIterator kt = _parents.begin(); kt != _parents.end(); ++kt)
		  {
		    // msg from X to Y comment
        std::cout << "\tnoeud "<<cpt<<" / "<< taille<<" "<<this->bn().variable(*it).name()<<" message vers parent "<< cpt_in << " / " << taille_in <<" " << this->bn().variable(*kt).name() << "        ";;
		    std::cout <<"\r"; 
		    //std::cout<<std::endl;
		    std::cout.flush();
		    
		    _msgL(*it,*kt);
		    //actifs_suivants.insert(*kt);
		    cpt_in++;
		  }

		  cpt+=cpt_in;
      
      /*if( ! (this->continueApproximationScheme(eps, false)) )
        return;*/
		}
    eps = this->_calculateEpsilon();
    
    std::cout << "epsilon = " << eps << std::endl;
    this->updateApproximationScheme(cpt);
  
		active_nodes_set.clear();
		active_nodes_set = next_active_nodes_set;
		next_active_nodes_set.clear();
		i++;
		// msg from X to Y comment
    std::cout << std::endl;
		// msg from X to Y comment
    		
	} while( this->continueApproximationScheme ( eps, false, false ) && active_nodes_set.size() > 0 );
	std::cout << "nb noeuds suivants : " << active_nodes_set.size() << std::endl;
  for(gum::NodeSetIterator it = active_nodes_set.begin(); it != active_nodes_set.end(); ++it)
		{
      std::cout << this->bn().variable(*it).name() << std::endl;
    }

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 	make inference by random order	////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_makeInferenceByRandomOrder()
{	
	unsigned int nbrArcs = this->bn().dag().sizeArcs();
	 
	std::vector<cArcP> seq;
	seq.reserve(nbrArcs);
	for(DAG::ArcIterator it = this->bn().beginArcs(); it != this->bn().endArcs(); ++it)
		seq.push_back(&(*it));
	
	unsigned i = 0; GUM_SCALAR eps;
	do
	{
		/* msg from X to Y comment
    std::cout << "\t\tIteration : " << i << std::endl;
		std::cout << std::endl;*/
		int cpt = 1;
		for(unsigned j = 0; j < nbrArcs / 2; ++j)
		{
			unsigned w1 = intRand(nbrArcs), w2 = intRand(nbrArcs);
			if(w1==w2) continue;
			
			cArcP tmp = seq[w1];
			seq[w1] = seq[w2];
			seq[w2] = tmp;
		}
		
		for(std::vector<cArcP>::iterator it = seq.begin(); it != seq.end(); ++it)
		{
			/* msg from X to Y comment
      std::cout<< "\tArc " << cpt << " / " << this->bn().nbrArcs();//<< "\r";
			std::cout.flush();*/
			_msgP((*it)->tail(), (*it)->head() );
			_msgL((*it)->head(), (*it)->tail() );
			cpt++;
			/* msg from X to Y comment
      std::cout << "\r";std::cout.flush();
      */
		}
    /* msg from X to Y comment
		std::cout << std::endl;*/
		eps = this->_calculateEpsilon();
		
		/* msg from X to Y comment
    std::cout << "epsilon = " << eps << std::endl;
    */
		this->updateApproximationScheme(); ++i;
	} while( (i<0) || (this->continueApproximationScheme(eps, false)) );
	
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			_msgL 					////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_msgL(const NodeId Y, const NodeId X)
{
  //std::cout << "msg L de " << this->bn().variable(Y).name() << " vers " << this->bn().variable(X).name()<< std::endl;
  //GUM_TRACE("_msgL called " << Y << " -> " << X);
  NodeSet const & children = this->bn().dag().children(Y); 
  
  NodeSet const & _parents = this->bn().dag().parents(Y);
  
  const gum::Potential<GUM_SCALAR> * parents = &bn().cpt(Y);
  
  
  if( ( (children.size() + (*parents).nbrDim() - 1) == 1 ) and (! this->_evidence.exists(Y) ) )
  {
    // msg from X to Y comment
    std::cout << "sortie msgL de suite (dim)" << std::endl;
    
    //GUM_TRACE("_msgL terminates short"); 
    return;
  }
  
  
  bool update_l = _update_l[Y];
  bool update_p = _update_p[Y];
  
  
  
  
   ///////////////////////// ?????????????????? ///////////////
  if( ! update_p && ! update_l )
	{
	  //std::cout << (this->bn()).variable(Y).name() <<" MSG_L vers " <<  (this->bn()).variable(X).name()<< " saute (inchange)" << std::endl;
	  return;
	}
  
  
  
  (*(_msg_l_sent[Y])).insert(X);
  
  // pour le refresh LM/PI
  if( (*(_msg_l_sent[Y])).size() == _parents.size() )
  {
    //std::cout << "TOUS MSGS L envoyes" << std::endl;
	(*(_msg_l_sent[Y])).clear();
  	_update_l[Y] = false;
  }
  
 
  
  // ----------------------------------------------------------------- refresh LM_part
  
  
 /**
 * INITIALISER a -1 dans initialize et passer au message de suite si evidence
 * pas besoin d'actualiser node_L
 */
   /* if(this->_evidence.exists(Y) )
    {
      if( this->_evidence[Y] == (GUM_SCALAR) 0. )
	lmin = (GUM_SCALAR) 0.;
      else if ( this->_evidence[Y] == (GUM_SCALAR) 1. )
	lmin = (GUM_SCALAR) -1.;
      
      lmax = lmin;
    }
    
    else */
   
   
   
   if(update_l)
   {
   
   if ( ! children.empty() && !this->_evidence.exists(Y) ) 
    {
      
	GUM_SCALAR lmin = (GUM_SCALAR) 1.;
	GUM_SCALAR lmax = (GUM_SCALAR) 1.;
  
      for(NodeSet::const_iterator it = children.begin(); it != children.end(); ++it)
      {
	/*if( isDNode( (*(get_cn().get_CPT()))[*it] ) )
	  continue;*/
	
	/*if( lmin == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(Y, *it)] == _INF)
	{}
	else if( lmin == _INF && this->_ArcsL_min[Arc(Y, *it)] == (GUM_SCALAR) 0.)
	{}
	else*/
	  lmin *= this->_ArcsL_min[Arc(Y, *it)];
		  		  
	if( this->_ArcsL_max.exists(Arc(Y, *it)) )
	{
	  /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_max[Arc(Y, *it)] == _INF)
	  {}
	  else if( lmax == _INF && this->_ArcsL_max[Arc(Y, *it)] == (GUM_SCALAR) 0.)
	  {}
	  else*/
	    lmax *= this->_ArcsL_max[Arc(Y, *it)];
	}
	else
	{
	  /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(Y, *it)] == _INF)
	  {}
	  else if( lmax == _INF && this->_ArcsL_min[Arc(Y, *it)] == (GUM_SCALAR) 0.)
	  {}
	  else*/
	    lmax *= this->_ArcsL_min[Arc(Y, *it)];
	}
      }
      
      if(lmin != lmin && lmax == lmax){//std::cout <<"\n lmin non defini : 0*inf"<<std::endl;
	lmin = lmax;}
      if(lmax != lmax && lmin == lmin){//std::cout <<"\n lmax non defini : 0*inf"<<std::endl;
	lmax = lmin;}
      if(lmax != lmax && lmin != lmin)
	std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;
            
      if(lmin <= (GUM_SCALAR) 0.)
	lmin = (GUM_SCALAR) 0.;
      if(lmax <= (GUM_SCALAR) 0.)
	lmax = (GUM_SCALAR) 0.;
      
      /*if(lmin != this->_NodesL_min[Y])
	_update_l.set(Y,true);
      else if(this->_NodesL_max.exists(Y))
	if(lmax != this->_NodesL_max[Y])	
	  _update_l.set(Y,true);
      else
      {
	std::cout << "update noeud courant L false" << std::endl;
	_update_l.set(Y,false);
	
      }*/
      
      /*
      if(lmin == _INF)
    lmin = (GUM_SCALAR) 1.;
  else
    lmin = lmin / (1+lmin);
  
  if(lmax == _INF)
    lmax = (GUM_SCALAR) 1.;
  else
    lmax = lmax / (1+lmax);
      */
      
      
      // pas besoin de mettre a jour nodeL si evidence car jamais utilise si evidence
	
	this->_NodesL_min[Y] = lmin;
	if( lmin != lmax )
	{
	 /* if( this->_NodesL_max.exists(Y) )
	    this->_NodesL_max[Y] = lmax;
	  else*/
	    this->_NodesL_max.set(Y, lmax);
	}
	else
	  if( this->_NodesL_max.exists(Y) )
	    this->_NodesL_max.erase(Y);
	  
      //std::cout << "update noeud courant L false" << std::endl;
     // _update_l.set(Y,false);
    }
    /*else
    {std::cout << "update noeud courant L false" << std::endl;
      _update_l.set(Y,false);
    }*/
    
    
    
   } // fin si enfants ont change
        
    
    GUM_SCALAR lmin = this->_NodesL_min[Y];
    GUM_SCALAR lmax;
    if( this->_NodesL_max.exists(Y) )
      lmax =  this->_NodesL_max[Y];
    else
      lmax = lmin;
    
    /*
    this->_NodesL_min[Y] = lmin;
    if( lmin != lmax )
    {
      if( this->_NodesL_max.exists(Y) )
	this->_NodesL_max[Y] = lmax;
      else
	this->_NodesL_max.insert(Y, lmax);
    }
    else
      if( this->_NodesL_max.exists(Y) )
	this->_NodesL_max.erase(Y);
  */
  /**
  * pour les parents (hormis le noeud dest)
  * si lmin == lmax == 1  => variable barren, envoi 1 comme message aux parents
  */
  
  if( lmin == lmax && lmin == (GUM_SCALAR) 1. )
  {
    this->_ArcsL_min[Arc(X, Y)] = lmin;
    if( this->_ArcsL_max.exists(Arc(X, Y)) )
      this->_ArcsL_max.erase(Arc(X, Y));
    
    //std::cout << "MSG L inchange = 1" << std::endl;
    
    return;
  }
  
  
  //std::string var_name = this->bn().variable(Y).name();
  
  // comparer string inutile si isDnode verifie nb_dist = card && proba = 0 ou 1 ?
  /*if(isDNode( (*(get_cn().get_CPT()))[Y] ))
  {
    //std::cout << "DNODE" << std::endl;std::cout.flush();
    //std::cout << this->bn().variable(Y).name() << std::endl; std::cout.flush();
    GUM_SCALAR min, max;
    
    if(lmax != (GUM_SCALAR) 0.)
    {
      if(lmin < ((GUM_SCALAR)1. / lmax) )
	min = lmin;
      else
	min = ((GUM_SCALAR)1. / lmax);
    }
    else
      min = lmin;
    
   if(lmin != (GUM_SCALAR) 0.)
   {
     if(lmax < ((GUM_SCALAR)1./lmin) )
       max = (GUM_SCALAR) 1. / min;
     else
       max = lmax;
   }
   else
     max = _INF;
   
  if(min <= (GUM_SCALAR) 0.)
    min = (GUM_SCALAR) 0.;
  if(max <= (GUM_SCALAR) 0.)
    max = (GUM_SCALAR) 0.;
    
   bool update = false;
   if( min != this->_ArcsL_min[Arc(X, Y)] )
   {
     this->_ArcsL_min[Arc(X, Y)] = min;
     update = true;
   }
   
   if( this->_ArcsL_max.exists(Arc(X, Y)) )
   {
     if( max != this->_ArcsL_max[Arc(X, Y)] )
     {
       if( max != min)
	 this->_ArcsL_max[Arc(X, Y)] = max;
       else //if( max == min )
	 this->_ArcsL_max.erase(Arc(X, Y));
       
       update = true;
     }
   }
   else
   {
     if( max != min)
     {
       this->_ArcsL_max.insert(Arc(X, Y), max);
       update = true;
     }
   }
   
   if( update )
   {
    //std::cout<<std::endl;
     //std::cout << " D_NODE msg L : [" << min << ", " << max<<"]" << " lx : [" <<lmin<<", "<<lmax<<"]"<<std::endl;
     _update_l.set(X,true);
     next_active_nodes_set.insert(X);
   }
    
    return;
  }
  */
  
  
  
  // garder pour chaque noeud un table des parents maj, une fois tous maj, stop jusque notification msg L ou P
  
  if(  update_p || update_l )
  {
	std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
	std::vector< std::vector<GUM_SCALAR> > msg_p;
	std::vector<GUM_SCALAR> distri(2);
	
	gum::Idx pos;
		
	// +1 from start to avoid counting itself
	for(gum::Sequence<const gum::DiscreteVariable*>::const_iterator jt = ++((*parents).begin()); jt != (*parents).end(); ++jt)
	{
	  if( bn().nodeId(**jt) == X )
	  {
	    pos = (*parents).pos(**jt) - 1; // retirer la variable courante de la taille
	    continue;
	  }
	  
	  // compute probability distribution to avoid doing it multiple times (at each combination of messages)
	  distri[1] = this->_ArcsP_min[Arc(bn().nodeId(**jt),Y)];
	  distri[0] = (GUM_SCALAR)1. - distri[1];
	  msg_p.push_back(distri);
	  
	  if( (this->_ArcsP_max).exists(Arc(bn().nodeId(**jt),Y)) )
	  {
	    distri[1] = this->_ArcsP_max[Arc(bn().nodeId(**jt),Y)];
	    distri[0] = (GUM_SCALAR)1. - distri[1];
	    msg_p.push_back(distri);
	  }
	  msgs_p.push_back(msg_p);
	  msg_p.clear();
	}
	
	GUM_SCALAR min = (GUM_SCALAR) -2.;
	GUM_SCALAR max = (GUM_SCALAR) -2.;
	
	
	std::vector< GUM_SCALAR > lx;
	lx.push_back(lmin);
	
	if(lmin != lmax)
	  lx.push_back(lmax);
	
	//std::cout << lmin << std::endl;
  // on peut avoir -1 pour infini
  _enum_combi(msgs_p, Y, min, max, lx, pos);
  
  /*if ( min == _INF || max == 0 ) {
    if ( min != _INF ) max = min;
    else if ( max != 0 ) min = max;
    else {
      std::cout << std::endl;
      std::cout << "!!!! pas de message L calculable !!!!" << std::endl;
      return;
    }
  }*/

  if(min == (GUM_SCALAR) -2. || max == (GUM_SCALAR) -2.)
  {
    if(min != (GUM_SCALAR) -2.)
      max = min;
    else if(max != (GUM_SCALAR) -2.)
      min = max;
    else
    {
      std::cout << std::endl;
      std::cout << "!!!! pas de message L calculable !!!!" << std::endl;
      return;
    }
  }
    
  //std::cout << "msg L de " << this->bn().variable(Y).name() << " vers " << this->bn().variable(X).name()<< std::endl;
  
  if(min <= (GUM_SCALAR) 0.)
    min = (GUM_SCALAR) 0.;
  if(max <= (GUM_SCALAR) 0.)
    max = (GUM_SCALAR) 0.;
  
  
  
  // ?????????????????????????????
  //_update_p.set(Y,false);
  
  bool update = false;
   if( min != this->_ArcsL_min[Arc(X, Y)] )
   {
     this->_ArcsL_min[Arc(X, Y)] = min;
     update = true;
   }
   
   if( this->_ArcsL_max.exists(Arc(X, Y)) )
   {
     if( max != this->_ArcsL_max[Arc(X, Y)] )
     {
       if( max != min)
	 this->_ArcsL_max[Arc(X, Y)] = max;
       else //if( max == min )
	 this->_ArcsL_max.erase(Arc(X, Y));
       
       update = true;
     }
   }
   else
   {
     if( max != min)
     {
       this->_ArcsL_max.insert(Arc(X, Y), max);
       update = true;
     }
   }
   
   if( update )
   {
     std::cout << std::endl;
     std::cout << " msg L : [" << min << ", " << max<<"]" << " lx : [" <<lmin<<", "<<lmax<<"]"<<std::endl;
     _update_l.set(X,true);
     next_active_nodes_set.insert(X);
   }
  
  
	//GUM_TRACE("_msgL terminates long");*/
  
  
  } // fin si update_p ou update_l

  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			_msgP 					////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_msgP(const NodeId X, const NodeId demanding_child)
{
  //std::cout << "msg P de " << this->bn().variable(X).name() << " vers " << this->bn().variable(demanding_child).name()<< std::endl;
	//GUM_TRACE("_msgP called " << X << " -> " << demanding_child);
	NodeSet const & children = this->bn().dag().children(X);
	//NodeSet const & parents = this->bn().dag().parents(X);
	const gum::Potential<GUM_SCALAR> * parents = &bn().cpt(X);
		
	if( ( (children.size() + (*parents).nbrDim() - 1) == 1) and (! this->_evidence.exists(X) ))
	{
		//GUM_TRACE("_msgP terminates short");
		return;
	}
	
	// ------------------------------------------------------------------LM_part ---- from all children but one --- the lonely one will get the message :)
	
	// si evidence msg_p = 1 si X = 1, 0 sinon
	if(this->_evidence.exists(X) ) 
	{
	  this->_ArcsP_min[Arc(X, demanding_child)] = (GUM_SCALAR) this->_evidence[X][1];
	  //std::cout << "msg P :" << (GUM_SCALAR) this->_evidence[X] << std::endl;
	  if( this->_ArcsP_max.exists(Arc(X, demanding_child)) )
	    this->_ArcsP_max.erase(Arc(X, demanding_child));
	  
	  return; // pas de mise a jour de node P
	}
	
	
  bool update_l = _update_l[X];
  bool update_p = _update_p[X];
	
	/**
	* si pas actualise, meme message
	*/
	if( ! update_p && ! update_l )
	{
	   //std::cout << (this->bn()).variable(X).name() <<" MSG_P vers " <<  (this->bn()).variable(demanding_child).name()<< " saute (inchange)" << std::endl;
	  return;
	}
	
	
	// L_part
		  
	GUM_SCALAR lmin = (GUM_SCALAR) 1.;
	GUM_SCALAR lmax = (GUM_SCALAR) 1.;
	
	
	  // prendre node L et diviser par lmin et/ou lmax de demanding child ?
	for(NodeSet::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		  if( (*it) == demanding_child) continue;
		  
		  /*if( isDNode( (*(get_cn().get_CPT()))[*it] ) )
		      continue;*/
		  
		  /*if( lmin == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(X, *it)] == _INF)
		  {}
		  else if( lmin == _INF && this->_ArcsL_min[Arc(X, *it)] == (GUM_SCALAR) 0.)
		  {}
		  else*/
		    lmin *= this->_ArcsL_min[Arc(X, *it)];
		  		  
		  if( this->_ArcsL_max.exists(Arc(X, *it)) )
		  {
		    /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_max[Arc(X, *it)] == _INF)
		    {}
		    else if( lmax == _INF && this->_ArcsL_max[Arc(X, *it)] == (GUM_SCALAR) 0.)
		    {}
		    else*/
		      lmax *= this->_ArcsL_max[Arc(X, *it)];
		  }
		  else
		  {
		    /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(X, *it)] == _INF)
		    {}
		    else if( lmax == _INF && this->_ArcsL_min[Arc(X, *it)] == (GUM_SCALAR) 0.)
		    {}
		    else*/
		      lmax *= this->_ArcsL_min[Arc(X, *it)];
		  }
	}
	
      if(lmin != lmin && lmax == lmax){//std::cout <<"\n lmin non defini : 0*inf"<<std::endl;
	lmin = lmax;}
      if(lmax != lmax && lmin == lmin){//std::cout <<"\n lmax non defini : 0*inf"<<std::endl;
	lmax = lmin;}
      if(lmax != lmax && lmin != lmin)
	std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;
            
	
	if(lmin <= (GUM_SCALAR) 0.)
	  lmin = (GUM_SCALAR) 0.;
	if(lmax <= (GUM_SCALAR) 0.)
	  lmax = (GUM_SCALAR) 0.;
	
	/*
	if(lmin == _INF)
    lmin = (GUM_SCALAR) 1.;
  else
    lmin = lmin / (1+lmin);
  
  if(lmax == _INF)
    lmax = (GUM_SCALAR) 1.;
  else
    lmax = lmax / (1+lmax);*/
	/**
	* si lmin == lmax == 1  => variable barren, msg envoye est pi(x) 
	*/
		
	
	// ------------------------------------------------------------------ refresh PI_part 
	GUM_SCALAR min = _INF;//(GUM_SCALAR) 1.;
	GUM_SCALAR max = (GUM_SCALAR) 0.;
    
    if(update_p )
    {
	std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
	std::vector< std::vector<GUM_SCALAR> > msg_p;
	std::vector<GUM_SCALAR> distri(2);
		
	// +1 from start to avoid counting itself
	for(gum::Sequence<const gum::DiscreteVariable*>::const_iterator jt = ++((*parents).begin()); jt != (*parents).end(); ++jt)
	{
	  // compute probability distribution to avoid doing it multiple times (at each combination of messages)
	  distri[1] = this->_ArcsP_min[Arc(bn().nodeId(**jt), X)];
	  distri[0] = (GUM_SCALAR)1. - distri[1];
	  msg_p.push_back(distri);
	  
	  if( (this->_ArcsP_max).exists(Arc(bn().nodeId(**jt), X)) )
	  {    
	    distri[1] = this->_ArcsP_max[Arc(bn().nodeId(**jt), X)];
	    distri[0] = (GUM_SCALAR)1. - distri[1];
	    msg_p.push_back(distri);
	  }
	  msgs_p.push_back(msg_p);
	  msg_p.clear();
	}

  /*
  for(unsigned int i = 0; i < msgs_p.size(); i++ ) {
    for(unsigned int j = 0; j < msgs_p[i].size(); j++) {
      std::cout << msgs_p[i][j] << " ";
    }
    std::cout << std::endl;
  }*/

	_enum_combi(msgs_p, X, min, max);
			
	if(min <= (GUM_SCALAR) 0.)
	  min = (GUM_SCALAR) 0.;
	if(max <= (GUM_SCALAR) 0.)
	  max = (GUM_SCALAR) 0.;
	
	if(min == _INF || max == _INF)
	{std::cout << " ERREUR msg P min = max = INF " <<std::endl; std::cout.flush();}
	
	this->_NodesP_min[X] = min;
	
	if(min != max)
	{
	  /*if( this->_NodesP_max.exists(X) )
	    this->_NodesP_max[X] = max;
	  else
	    this->_NodesP_max.insert(X, max);*/
	  this->_NodesP_max.set(X, max);
	}
	else
	  if( this->_NodesP_max.exists(X) )
	    this->_NodesP_max.erase(X);
	  
	  _update_p.set(X,false);
	  
    } // fin si parents ont change
   else
    {
     //std::cout << (this->bn()).variable(X).name() <<" MSG_P vers " <<  (this->bn()).variable(demanding_child).name()<< " enum combi saute" << std::endl;
	  //GUM_SCALAR min, max;
	  min = this->_NodesP_min[X];
	  if( this->_NodesP_max.exists(X) )
	    max = this->_NodesP_max[X];
	  else
	    max = min;
    }
	 
	/**
	 * message a envoyer
	 */
  if(  update_p ||  update_l )
  {
	
	GUM_SCALAR msg_p_min;
	GUM_SCALAR msg_p_max;
	
	// cas limites sur min	
	/*if( min == (GUM_SCALAR) 0. && lmin == _INF)
	  std::cout << "MESSAGE P ERR : pi = 0, l = inf" << std::endl;*/
	if( min == _INF && lmin == (GUM_SCALAR) 0. )
	  std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;
		
	if (/* min != (GUM_SCALAR) 0. &&*/ lmin == _INF /*< (GUM_SCALAR) 0. */) // cas infini
	  msg_p_min = (GUM_SCALAR) 1.;
	else if( min == (GUM_SCALAR) 0. /*&& lmin != _INF*/ || lmin == (GUM_SCALAR) 0.)
	  msg_p_min = (GUM_SCALAR) 0;
	else
	  msg_p_min = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / min - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmin ));
		
	// cas limites sur max	
	/*if(max == (GUM_SCALAR) 0. && lmax == _INF)
	  std::cout << "MESSAGE P ERR : pi = 0, l = inf" << std::endl;*/
	if( max == _INF && lmax == (GUM_SCALAR) 0. )
	  std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;
	
	if ( /*max != (GUM_SCALAR) 0. &&*/ lmax == _INF /*< (GUM_SCALAR) 0. */) // cas infini
	  msg_p_max = (GUM_SCALAR) 1.;	
	else if( max == (GUM_SCALAR) 0./* && lmax != _INF*/ || lmax == (GUM_SCALAR) 0.)
	  msg_p_max = (GUM_SCALAR) 0;
	else
	  msg_p_max = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / max - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmax ));
	
	if(msg_p_min != msg_p_min && msg_p_max == msg_p_max)
	{
	  msg_p_min = msg_p_max;
	  std::cout << std::endl;
	  std::cout << "msg_p_min is NaN" << std::endl;
	}
	if(msg_p_max != msg_p_max && msg_p_min == msg_p_min)
	{
	  msg_p_max = msg_p_min;
	  std::cout << std::endl;
	  std::cout << "msg_p_max is NaN" << std::endl;
	  
	}
	if(msg_p_max != msg_p_max && msg_p_min != msg_p_min)
	{
	  std::cout << std::endl;
	  std::cout << "pas de message P calculable (verifier observations)" << std::endl;
	}
  
	if(msg_p_min <= (GUM_SCALAR) 0.)
	  msg_p_min = (GUM_SCALAR) 0.;
	if(msg_p_max <= (GUM_SCALAR) 0.)
	  msg_p_max = (GUM_SCALAR) 0.;
	
	
    bool update = false;
   if( msg_p_min != this->_ArcsP_min[Arc(X, demanding_child)] )
   {
     this->_ArcsP_min[Arc(X, demanding_child)] = msg_p_min;
     update = true;
   }
   
   if( this->_ArcsP_max.exists(Arc(X, demanding_child)) )
   {
     if( msg_p_max != this->_ArcsP_max[Arc(X, demanding_child)] )
     {
       if( msg_p_max != msg_p_min)
	 this->_ArcsP_max[Arc(X, demanding_child)] = msg_p_max;
       else //if( msg_p_max == msg_p_min )
	 this->_ArcsP_max.erase(Arc(X, demanding_child));
       
       update = true;
     }
   }
   else
   {
     if( msg_p_max != msg_p_min)
     {
       this->_ArcsP_max.insert(Arc(X, demanding_child), msg_p_max);
       update = true;
     }
   }
   
   if( update )
   {
     std::cout<<std::endl;
     std::cout << " msg P : [" << msg_p_min << ", " << msg_p_max<<"]" << " lx : [" <<lmin<<", "<<lmax<<"]"<<std::endl;
     _update_p.set(demanding_child,true);
     next_active_nodes_set.insert(demanding_child);
   }
	
	
	
  } // fin si enfants ou parents ont change
	
	
	
	//GUM_TRACE("_msgP terminates long");
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 		refresh LMs end PIs			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
void LoopyPropagation<GUM_SCALAR>::_refreshLMsPIs() 
{
  // msg from X to Y comment
  std::cout << "\n\n\t\trefresh LM & PI\n" << std::endl;
  
	for(DAG::NodeIterator itX = this->bn().beginNodes(); itX != this->bn().endNodes(); ++itX)
	{
	  // msg from X to Y comment
    std::cout << "                                          \r";std::cout.flush();
	  std::cout << "\t" << this->bn().variable(*itX).name()<<"      ";std::cout.flush();// << std::endl;
    
		NodeSet const & children = this->bn().dag().children(*itX);
		//NodeSet const & parents = this->bn().dag().parents(*itX);
		
		const gum::Potential<GUM_SCALAR> * parents = &bn().cpt(*itX);
	
	//std::cout << std::endl;
		
	if ( _update_l[*itX] )
	 {
		GUM_SCALAR lmin = (GUM_SCALAR) 1.;		  
		GUM_SCALAR lmax = (GUM_SCALAR) 1.;
		/*
		if(this->_evidence.exists(*itX))
		{
		  if( this->_evidence[*itX] == (GUM_SCALAR) 0.)
		    lmin = (GUM_SCALAR) 0.;    
		  else if ( this->_evidence[*itX] == (GUM_SCALAR) 1.)
		    lmin = (GUM_SCALAR) -1.;
		  
		  lmax = lmin;
		}
		
		else */if ( ! children.empty() && !this->_evidence.exists(*itX))
		{
		 //std::cout << "REFRESH L : " << bn().variable(*itX).name()<<std::endl;
		// L part
		  for(NodeSet::const_iterator it = children.begin(); it != children.end(); ++it)
		  {
		    /*if( isDNode( (*(get_cn().get_CPT()))[*it] ) )
		      continue;*/
		    
		    /*if( lmin == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(*itX, *it)] == _INF)
		    {}
		    else if( lmin == _INF && this->_ArcsL_min[Arc(*itX, *it)] == (GUM_SCALAR) 0.)
		    {}
		    else*/
		      lmin *= this->_ArcsL_min[Arc(*itX, *it)];
		    //std::cout << bn().variable(*it).name() <<" min " <<this->_ArcsL_min[Arc(*itX, *it)] << " ";
		    
				    
		    if( this->_ArcsL_max.exists(Arc(*itX, *it)) )
		    {
		      //std::cout << " max " << this->_ArcsL_max[Arc(*itX, *it)] << std::endl;
		      /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_max[Arc(*itX, *it)] == _INF)
		      {}
		      else if( lmax == _INF && this->_ArcsL_max[Arc(*itX, *it)] == (GUM_SCALAR) 0.)
		      {}
		      else*/
			lmax *= this->_ArcsL_max[Arc(*itX, *it)];
		    }
		    else
		    {
		      /*if( lmax == (GUM_SCALAR) 0. && this->_ArcsL_min[Arc(*itX, *it)] == _INF)
		      {}
		      else if( lmax == _INF && this->_ArcsL_min[Arc(*itX, *it)] == (GUM_SCALAR) 0.)
		      {}
		      else*/
			lmax *= this->_ArcsL_min[Arc(*itX, *it)];
		    }
		  }
		  
		 if(lmin != lmin && lmax == lmax){//std::cout <<"\n lmin non defini : 0*inf"<<std::endl;
	lmin = lmax;}
      if(lmax != lmax && lmin == lmin){//std::cout <<"\n lmax non defini : 0*inf"<<std::endl;
	lmax = lmin;}
      if(lmax != lmax && lmin != lmin)
	std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;
            
		  if(lmin <= (GUM_SCALAR) 0.)
		    lmin = (GUM_SCALAR) 0.;
		  if(lmax <= (GUM_SCALAR) 0.)
		    lmax = (GUM_SCALAR) 0.;
		  /*
		  if(lmin == _INF)
		    lmin = (GUM_SCALAR) 1.;
		  else
		    lmin = lmin / (1+lmin);
		  
		  if(lmax == _INF)
		    lmax = (GUM_SCALAR) 1.;
		  else
		    lmax = lmax / (1+lmax);
			*/	  
		  this->_NodesL_min[*itX] = lmin;
		  
		  if( lmin != lmax )
		  {
		    /*if( this->_NodesL_max.exists(*itX) )
		      this->_NodesL_max[*itX] = lmax;
		    else
		      this->_NodesL_max.insert(*itX, lmax);*/
		    this->_NodesL_max.set(*itX, lmax);
		    //std::cout << lmax << std::endl;
		  }
		  else
		    if( this->_NodesL_max.exists(*itX) )
		      this->_NodesL_max.erase(*itX);
		}
		
	 }
	if(_update_p[*itX])
	{
		// PI part
		   
		if( ((*parents).nbrDim() - 1) > 0  && !this->_evidence.exists(*itX) )
		{
		  //std::cout << "REFRESH P : " << bn().variable(*itX).name()<<std::endl;
		    std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
		    std::vector< std::vector<GUM_SCALAR> > msg_p;
		    std::vector<GUM_SCALAR> distri(2);
		    
		    // +1 from start to avoid counting itself
		    for(gum::Sequence<const gum::DiscreteVariable*>::const_iterator jt = ++((*parents).begin()); jt != (*parents).end(); ++jt)
		    {
		      // compute probability distribution to avoid doing it multiple times (at each combination of messages)
		      distri[1] = this->_ArcsP_min[Arc(bn().nodeId(**jt), *itX)];
		      distri[0] = (GUM_SCALAR)1. - distri[1];
		      msg_p.push_back(distri);
		      		      
		      if( (this->_ArcsP_max).exists(Arc(bn().nodeId(**jt), *itX)) )
		      {    
			distri[1] = this->_ArcsP_max[Arc(bn().nodeId(**jt), *itX)];
			distri[0] = (GUM_SCALAR)1. - distri[1];
			msg_p.push_back(distri);
		      }
		      msgs_p.push_back(msg_p);
		      msg_p.clear();
		    }
		    
		    GUM_SCALAR min = _INF;//(GUM_SCALAR) 1.;
		    GUM_SCALAR max = (GUM_SCALAR) 0.;
			    
		    _enum_combi(msgs_p, *itX, min, max);
				
		    
		  if(min <= (GUM_SCALAR) 0.)
		    min = (GUM_SCALAR) 0.;
		  if(max <= (GUM_SCALAR) 0.)
		    max = (GUM_SCALAR) 0.;
		    
		    this->_NodesP_min[*itX] = min;
		    
		    if(min != max)
		    {
		      /*if( this->_NodesP_max.exists(*itX) )
			this->_NodesP_max[*itX] = max;
		      else
			this->_NodesP_max.insert(*itX, max);*/
		      this->_NodesP_max.set(*itX, max);
		    }
		    else
		      if( this->_NodesP_max.exists(*itX) )
			this->_NodesP_max.erase(*itX);
		  _update_p[*itX] = false;
		}
		
		
	}
	
	
	// msg from X to Y comment
  std::cout << "\r";std::cout.flush();
	}
  // msg from X to Y comment
	std::cout<<std::endl;

}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// update marginals ///////////////////
////////////////////////////////////////////////////////////////////////////////

template< typename GUM_SCALAR >
void LoopyPropagation<GUM_SCALAR>::_updateMarginals() {
  for(DAG::NodeIterator it = this->bn().beginNodes(); it != this->bn().endNodes(); ++it)
  {	
    GUM_SCALAR msg_p_min = 1.0;
    GUM_SCALAR msg_p_max = 0.0;

    if( this->_evidence.exists(*it) )
    {
      if( this->_evidence[*it][1] == (GUM_SCALAR) 0. )
        msg_p_min = (GUM_SCALAR) 0.;
      else if( this->_evidence[*it][1] == (GUM_SCALAR) 1. )
        msg_p_min = (GUM_SCALAR) 1.;

      msg_p_max = msg_p_min;
    }
    else
    {
      GUM_SCALAR min = this->_NodesP_min[*it];
      GUM_SCALAR max;

      if( this->_NodesP_max.exists(*it) )
        max = this->_NodesP_max[*it];
      else
        max = min;

      GUM_SCALAR lmin = this->_NodesL_min[*it];
      GUM_SCALAR lmax;

      if( this->_NodesL_max.exists(*it) )
        lmax = this->_NodesL_max[*it];
      else
        lmax = lmin;

      if(min == _INF || max == _INF)
      {
        std::cout << " min ou max === _INF !!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
        std::cout.flush();
      }

      if( min == _INF && lmin == (GUM_SCALAR) 0. )
        std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;

      if ( lmin == _INF  ) 
        msg_p_min = (GUM_SCALAR) 1.;
      else if( min == (GUM_SCALAR) 0. || lmin == (GUM_SCALAR) 0.)
        msg_p_min = (GUM_SCALAR) 0;
      else
        msg_p_min = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / min - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmin ));

      if( max == _INF && lmax == (GUM_SCALAR) 0. )
        std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;

      if (  lmax == _INF  )        
        msg_p_max = (GUM_SCALAR) 1.;
      else if( max == (GUM_SCALAR) 0. || lmax == (GUM_SCALAR) 0.)
        msg_p_max = (GUM_SCALAR) 0;	 
      else
        msg_p_max = (GUM_SCALAR)1. / ( (GUM_SCALAR)1. + ( ((GUM_SCALAR)1. / max - (GUM_SCALAR)1.) * (GUM_SCALAR)1. / lmax ));
    }

    if(msg_p_min != msg_p_min && msg_p_max == msg_p_max)
    {
      msg_p_min = msg_p_max;
      std::cout << std::endl;
      std::cout << "msg_p_min is NaN" << std::endl;
    }
    if(msg_p_max != msg_p_max && msg_p_min == msg_p_min)
    {
      msg_p_max = msg_p_min;
      std::cout << std::endl;
      std::cout << "msg_p_max is NaN" << std::endl;

    }
    if(msg_p_max != msg_p_max && msg_p_min != msg_p_min)
    {
      std::cout << std::endl;
      std::cout << "pas de proba calculable (verifier observations)" << std::endl;
    }

    if(msg_p_min <= (GUM_SCALAR) 0.)
      msg_p_min = (GUM_SCALAR) 0.;
    if(msg_p_max <= (GUM_SCALAR) 0.)
      msg_p_max = (GUM_SCALAR) 0.;

    this->_marginalMin[*it][0] = 1 - msg_p_max;
    this->_marginalMax[*it][0] = 1 - msg_p_min;
    this->_marginalMin[*it][1] = msg_p_min;
    this->_marginalMax[*it][1] = msg_p_max;

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 		calculate epsilon			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
GUM_SCALAR LoopyPropagation<GUM_SCALAR>::_calculateEpsilon() // as a matter of fact, this is derivative of epsilon, the real epsilon can not be know
{
  //std::cout << "calcul epsilon, refresh PI, LM, calcul de P(x|e)" << std::endl;
  
  _refreshLMsPIs();
  _updateMarginals();

  return this->_computeEpsilon();
 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			CONSTRUCTOR 			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
LoopyPropagation<GUM_SCALAR>::LoopyPropagation(const CredalNet<GUM_SCALAR> &cn, const BayesNet<GUM_SCALAR>& bn) 
: CNInferenceEngine< GUM_SCALAR >::CNInferenceEngine ( cn ), _InferenceUpToDate(false), __inferenceType(ordered)
{
  if ( ! cn.isSeparatelySpecified() )
    GUM_ERROR (OperationNotAllowed, "LoopyPropagation is only available with separately specified nets");

	this->cn = &cn;
	this->bnet = &bn;
	//initRandom();
	GUM_CONSTRUCTOR(LoopyPropagation);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 			DESTRUCTOR	 			////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename GUM_SCALAR>
LoopyPropagation<GUM_SCALAR>::~LoopyPropagation()
{
	_cleanInferenceData();
	
	GUM_DESTRUCTOR(LoopyPropagation);
}


} // end of namespace gum






