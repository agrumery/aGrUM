#ifndef __LOOPY_PROPAGATION__H__
#define __LOOPY_PROPAGATION__H__

#include "CredalNet.h"
//#include "PearlPropagation.h"
#include <agrum/BN/algorithms/approximationScheme.h>
#include <agrum/core/sequence.h>
#include <cstdlib>
#include <ctime>
#include <limits>

#define _INF std::numeric_limits<GUM_SCALAR>::infinity()

//double _INF = std::numeric_limits<double>::infinity();

namespace gum
{
	template<typename GUM_SCALAR>
	class LoopyPropagation : public InferenceEngine<GUM_SCALAR>
	{
	public:
		//typedef std::vector<GUM_SCALAR> msg;
		typedef std::vector< gum::Potential<GUM_SCALAR>* > msg;
		
		//typedef typename Property< std::vector<GUM_SCALAR>* >::onNodes msg2;
		
		
		//typedef gum::List<const msg*> Lpmsg;
		typedef const gum::Arc* cArcP;
	
		enum InferenceType {ordered, randomOrder, randomEvaluation}; 
		
		const gum::BayesNet<GUM_SCALAR>& bn() const;
		const gum::BayesNet<GUM_SCALAR>& bn_org() const;
		/*const gum::BayesNet<GUM_SCALAR>& bn_min() const;
		const gum::BayesNet<GUM_SCALAR>& bn_max() const;*/
		const std::vector< std::vector<GUM_SCALAR> >& get_CPT_min() const;
		const std::vector< std::vector<GUM_SCALAR> >& get_CPT_max() const;
		const CredalNet<GUM_SCALAR>& get_cn() const;

		virtual void makeInference();
		
		void setInferenceType(InferenceType inft){__inferenceType = inft;}
		InferenceType inferenceType(){return __inferenceType;}
		
    // deprecated, use saveMarginals() from InferenceEngine instead if scripts are needed
    // this one is actually easier to read from file, but harder to parse
		void saveInference(const std::string &path);
		
		LoopyPropagation(const CredalNet<GUM_SCALAR> &cn, const BayesNet<GUM_SCALAR>& bn);
		~LoopyPropagation();
		
		
		std::vector< typename Property<GUM_SCALAR>::onNodes * > _oldMarginal_min_t;
		std::vector< typename Property<GUM_SCALAR>::onNodes * > _oldMarginal_max_t;
		
		typename std::vector<GUM_SCALAR> eps;
		
    void insertEvidence( const std::string & path );
    void insertEvidence( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap );
    // deprecated insertEvidence (should use IngerenceEngine instead, like above)
    // keep it since it will be usefull (little work needed) if DTS is back (GL2U)
		virtual void insertEvidence_old(const std::string &path);
		//virtual void eraseAllEvidence();
				
	protected:
    void _updateMarginals();
    void _insertEvidence();
    void virtual _initialize();
    void virtual _cleanInferenceData();
		void virtual _makeInferenceByOrderedArcs();
		void virtual _makeInferenceByRandomOrder();
		void virtual _makeInferenceByRandomEvaluation();
		
		/**
		* message vers les parents
		*/
		void _msgL(const NodeId X, const NodeId demanding_parent);  //allways sent from X to demanding_X
			
		// extemes apres marginalisation
		void _compute_ext(GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, GUM_SCALAR &num_min, GUM_SCALAR &num_max, GUM_SCALAR &den_min, GUM_SCALAR &den_max);
		
		// pour marginaliser sur une combinaison
		void _compute_ext(std::vector< std::vector<GUM_SCALAR> > &combi_msg_p, const gum::NodeId &id, GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, const gum::Idx &pos);
		
		// enumere combinaisons des messages parents
		void _enum_combi(std::vector< std::vector< std::vector<GUM_SCALAR> > > &msgs_p, const gum::NodeId &id, GUM_SCALAR &msg_l_min, GUM_SCALAR &msg_l_max, std::vector<GUM_SCALAR> &lx, const gum::Idx &pos);
		
		/**
		* message vers les enfants
		*/
		void _msgP(const NodeId X, const NodeId demanding_child);
		
		// enumere combinaisons des messages parents
		void _enum_combi(std::vector< std::vector< std::vector<GUM_SCALAR> > > &msgs_p, const gum::NodeId &id, GUM_SCALAR &msg_p_min, GUM_SCALAR &msg_p_max);
		
		// marginalise et calcul des extremes pour une combinaison
		void _compute_ext(std::vector< std::vector<GUM_SCALAR> > &combi_msg_p, const gum::NodeId &id, GUM_SCALAR &msg_p_min, GUM_SCALAR &msg_p_max);
				
		
		void _refreshLMsPIs();
		
		GUM_SCALAR _calculateEpsilon();
		
		typename Property<GUM_SCALAR>::onNodes _oldMarginal_min;
		typename Property<GUM_SCALAR>::onNodes _oldMarginal_max;
		
		
		Property<bool>::onNodes _update_p;
		Property<bool>::onNodes _update_l;
		
		gum::NodeSet active_nodes_set;
		gum::NodeSet next_active_nodes_set;
		
		typename Property< gum::NodeSet * >::onNodes _msg_l_sent;
		
		typename Property<GUM_SCALAR>::onNodes _Evidence;
		
		// messages min/max sur X = 1
		typename Property<GUM_SCALAR>::onArcs _ArcsL_min;
		typename Property<GUM_SCALAR>::onArcs _ArcsP_min;
		typename Property<GUM_SCALAR>::onNodes _NodesL_min;
		typename Property<GUM_SCALAR>::onNodes _NodesP_min;
		
		typename Property<GUM_SCALAR>::onArcs _ArcsL_max;
		typename Property<GUM_SCALAR>::onArcs _ArcsP_max;
		typename Property<GUM_SCALAR>::onNodes _NodesL_max;
		typename Property<GUM_SCALAR>::onNodes _NodesP_max;
		
		bool _InferenceUpToDate;
	
	private:
		inline GUM_SCALAR __euclidean(GUM_SCALAR p, GUM_SCALAR q);
		InferenceType __inferenceType;
		
		const CredalNet<GUM_SCALAR> *cn;
		/*const gum::BayesNet<GUM_SCALAR> *bn_min;
		const gum::BayesNet<GUM_SCALAR> *bn_max;*/
		
		const gum::BayesNet<GUM_SCALAR> *bnet;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// 			intRand 					////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline unsigned int intRand(unsigned int limit)
	{
		return  std::rand() % limit;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// 			maybe					////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline bool maybe(double p)
	{
		if(std::rand() <= p * RAND_MAX) return true;
		return false;
	}



}

#include "LoopyPropagation.tcc"
#endif
