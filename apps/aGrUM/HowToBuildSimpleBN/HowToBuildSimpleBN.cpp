#include <iostream>

#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/BN/BayesNetFactory.h>

void createSimpleBN(gum::BayesNet<double>& bn) {
  for(const auto& s : {"foo","bar","qux"}) {
    bn.add(gum::LabelizedVariable(s,s)); // 2 modalities by default
  }

  bn.addArc(bn.idFromName("foo"),bn.idFromName("bar"));
  bn.addArc(bn.idFromName("qux"),bn.idFromName("bar"));

  bn.cpt(bn.idFromName("foo")).fillWith({0.1,0.9});
  bn.cpt(bn.idFromName("qux")).fillWith({0.3,0.7});
  bn.cpt(bn.idFromName("bar")).fillWith({
    0.2,0.8,  //foo=0 | qux=0
    0.3,0.7,  //foo=1 | qux=0
    0.9,0.1,  //foo=0 | qux=1
    0.5,0.5   //foo=1 | qux=1
  });
}


int main(void) {
  gum::BayesNet<double> bn1;

  createSimpleBN(bn1);

  std::cout<<"== Created BN =="<<std::endl;
  std::cout<<"================"<<std::endl<<std::endl;
  std::cout<<bn1<<std::endl;
  std::cout<<std::endl<<std::endl;

  std::cout<<"== dot version of the BN =="<<std::endl;
  std::cout<<"==========================="<<std::endl<<std::endl;
  std::cout<<bn1.toDot()<<std::endl;
  std::cout<<std::endl<<std::endl;

  std::cout<<"== cpt for 'bar' variable =="<<std::endl;
  std::cout<<"============================"<<std::endl;
  std::cout<<bn1.cpt(bn1.idFromName("bar"))<<std::endl<<std::endl;
  std::cout<<std::endl<<std::endl;
}
