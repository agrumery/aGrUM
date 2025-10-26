#ifndef __cxxtest__AgrumErrorPrinter_h__
#define __cxxtest__AgrumErrorPrinter_h__

//
// The AgrumErrorPrinter is a simple TestListener that
// just prints "OK" if everything goes well, otherwise
// reports the error in the format of compiler messages.
// The AgrumErrorPrinter uses std::cerr
//

#include <cxxtest/Flags.h>

#ifndef _CXXTEST_HAVE_STD
#  define _CXXTEST_HAVE_STD
#endif   // _CXXTEST_HAVE_STD

#include <cxxtest/StdValueTraits.h>

// in order to force GUM_CHECKPOINT / GUM_TRACE / GUM_TRACE_VAR to works even in
// release mode
#define GUM_TRACE_ON

#include <cxxtest/AgrumErrorFormatter.h>

#ifdef _CXXTEST_OLD_STD
#  include <iostream.h>
#else    // !CXXTEST_OLD_STD_
#  include <iostream>
#endif   // _CXXTEST_OLD_STD

namespace CxxTest {
  class AgrumErrorPrinter final: public AgrumErrorFormatter {
    public:
    AgrumErrorPrinter(CXXTEST_STD(ostream) & o = CXXTEST_STD(cout),
                      const char* preLine      = ":",
                      const char* postLine     = "") :
        AgrumErrorFormatter(new Adapter(o), preLine, postLine) {}

    virtual ~AgrumErrorPrinter() { delete outputStream(); }

    private:
    class Adapter final: public OutputStream {
      CXXTEST_STD(ostream) & _o;

      public:
      Adapter(CXXTEST_STD(ostream) & o) : _o(o) {}

      void flush() { _o.flush(); }

      OutputStream& operator<<(const char* s) {
        _o << s;
        return *this;
      }

      OutputStream& operator<<(Manipulator m) { return OutputStream::operator<<(m); }

      OutputStream& operator<<(unsigned i) {
        char s[1 + 3 * sizeof(unsigned)];
        numberToString(i, s);
        _o << s;
        return *this;
      }
    };
  };
}   // namespace CxxTest

#endif   //  __cxxtest__ErrorPrinter_h__
