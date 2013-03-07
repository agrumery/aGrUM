#ifndef __LOOPY_PROPAGATION__H__
#define __LOOPY_PROPAGATION__H__

//#include "CredalNet.h"
//#include "PearlPropagation.h"
#include <agrum/BN/algorithms/approximationScheme.h>
#include <agrum/core/sequence.h>
#include <cstdlib>
#include <ctime>
#include <limits>

#define _INF std::numeric_limits<T_DATA>::infinity()

//double _INF = std::numeric_limits<double>::infinity();

namespace gum
{
	template<typename T_DATA>
	class LoopyPropagation : public InferenceEngine<T_DATA>
	{
	public:
		//typedef std::vector<T_DATA> msg;
		typedef std::vector< gum::Potential<T_DATA>* > msg;
		
		//typedef typename Property< std::vector<T_DATA>* >::onNodes msg2;
		
		
		//typedef gum::List<const msg*> Lpmsg;
		typedef const gum::Arc* cArcP;
	
		enum InferenceType {ordered, randomOrder, randomEvaluation}; 
		
		const gum::BayesNet<T_DATA>& bn() const;
		const gum::BayesNet<T_DATA>& bn_org() const;
		/*const gum::BayesNet<T_DATA>& bn_min() const;
		const gum::BayesNet<T_DATA>& bn_max() const;*/
		const std::vector< std::vector<T_DATA> >& get_CPT_min() const;
		const std::vector< std::vector<T_DATA> >& get_CPT_max() const;
		const CredalNet<T_DATA>& get_cn() const;

		virtual void makeInference();
		
		void setInferenceType(InferenceType inft){__inferenceType = inft;}
		InferenceType inferenceType(){return __inferenceType;}
		
    // deprecated, use saveMarginals() from InferenceEngine instead if scripts are needed
    // this one is actually easier to read from file, but harder to parse
		void saveInference(const std::string &path);
		
		LoopyPropagation(const CredalNet<T_DATA> &cn, const BayesNet<T_DATA>& bn);
		~LoopyPropagation();
		
		
		std::vector< typename Property<T_DATA>::onNodes * > _oldMarginal_min_t;
		std::vector< typename Property<T_DATA>::onNodes * > _oldMarginal_max_t;
		
		typename std::vector<T_DATA> eps;
		
    void insertEvidence( const std::string & path );
    void insertEvidence( const std::map< std::string, std::vector< T_DATA > > & eviMap );
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
		void _compute_ext(T_DATA &msg_l_min, T_DATA &msg_l_max, std::vector<T_DATA> &lx, T_DATA &num_min, T_DATA &num_max, T_DATA &den_min, T_DATA &den_max);
		
		// pour marginaliser sur une combinaison
		void _compute_ext(std::vector< std::vector<T_DATA> > &combi_msg_p, const gum::NodeId &id, T_DATA &msg_l_min, T_DATA &msg_l_max, std::vector<T_DATA> &lx, const gum::Idx &pos);
		
		// enumere combinaisons des messages parents
		void _enum_combi(std::vector< std::vector< std::vector<T_DATA> > > &msgs_p, const gum::NodeId &id, T_DATA &msg_l_min, T_DATA &msg_l_max, std::vector<T_DATA> &lx, const gum::Idx &pos);
		
		/**
		* message vers les enfants
		*/
		void _msgP(const NodeId X, const NodeId demanding_child);
		
		// enumere combinaisons des messages parents
		void _enum_combi(std::vector< std::vector< std::vector<T_DATA> > > &msgs_p, const gum::NodeId &id, T_DATA &msg_p_min, T_DATA &msg_p_max);
		
		// marginalise et calcul des extremes pour une combinaison
		void _compute_ext(std::vector< std::vector<T_DATA> > &combi_msg_p, const gum::NodeId &id, T_DATA &msg_p_min, T_DATA &msg_p_max);
				
		
		void _refreshLMsPIs();
		
		T_DATA _calculateEpsilon();
		
		typename Property<T_DATA>::onNodes _oldMarginal_min;
		typename Property<T_DATA>::onNodes _oldMarginal_max;
		
		
		Property<bool>::onNodes _update_p;
		Property<bool>::onNodes _update_l;
		
		gum::NodeSet active_nodes_set;
		gum::NodeSet next_active_nodes_set;
		
		typename Property< gum::NodeSet * >::onNodes _msg_l_sent;
		
		typename Property<T_DATA>::onNodes _Evidence;
		
		// messages min/max sur X = 1
		typename Property<T_DATA>::onArcs _ArcsL_min;
		typename Property<T_DATA>::onArcs _ArcsP_min;
		typename Property<T_DATA>::onNodes _NodesL_min;
		typename Property<T_DATA>::onNodes _NodesP_min;
		
		typename Property<T_DATA>::onArcs _ArcsL_max;
		typename Property<T_DATA>::onArcs _ArcsP_max;
		typename Property<T_DATA>::onNodes _NodesL_max;
		typename Property<T_DATA>::onNodes _NodesP_max;
		
		bool _InferenceUpToDate;
	
	private:
		inline T_DATA __euclidean(T_DATA p, T_DATA q);
		InferenceType __inferenceType;
		
		const CredalNet<T_DATA> *cn;
		/*const gum::BayesNet<T_DATA> *bn_min;
		const gum::BayesNet<T_DATA> *bn_max;*/
		
		const gum::BayesNet<T_DATA> *bnet;
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




#include "LoopyPropagation_v0.tcc"
#endif
