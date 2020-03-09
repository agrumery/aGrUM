#include <iostream>

#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/multidim/potential.h>

void simplePotentials() {
  auto a=gum::LabelizedVariable("a","afoo");
  auto b=gum::LabelizedVariable("b","bfoo");
  auto c=gum::LabelizedVariable("c","cfoo");
  
  gum::Potential<double> p1;
  p1<<a<<b;  
  p1.fillWith({0.1,0.2,0.3,0.4});
  
  gum::Potential<double> p2;
  p2<<b<<c;  
  p2.fillWith({0.3,0.4,0.1,0.2});

  std::cout<<"p1: "<<p1<<std::endl;
  std::cout<<"p2: "<<p2<<std::endl;
  
  std::cout<<"p1*p2: "<<p1*p2<<std::endl;  
  std::cout<<"p1+p2: "<<p1+p2<<std::endl;  
  std::cout<<"p1/p2: "<<p1/p2<<std::endl;  
  std::cout<<"p1-p2: "<<p1-p2<<std::endl;  
  
}


int main(void) {
  simplePotentials();
}
