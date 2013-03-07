#include < iostream >
#include < agrum/CN/MCSampling.h >

namespace gum_tests {

  class mcSamplingListener : public gum::Listener {
    private :
      int __nbr;
      std::string __msg;
    protected :

    public :
      mcSamplingListener() : __nbr ( 0 ) {};

      void whenProgress ( const void * buffer, )
  }

} // end of : namespace gum_tests
