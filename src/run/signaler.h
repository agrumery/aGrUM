#ifndef SIGNALER_H__
#define SIGNALER_H__

#include <agrum/core/set.h>
#include <agrum/core/list.h>

namespace gum {

  class Listener;


  namespace __sig__ {

    class ISignaler {
      public:
        virtual void detachFromTarget( Listener* target ) = 0;
        virtual void duplicateTarget( const Listener* oldtarget, Listener* newtarget ) = 0;
    };
  } //namespace sig

  class Listener {
    private:
      typedef Set<__sig__::ISignaler *> Senders_set;
      typedef Senders_set::iterator const_iterator; //Senders_set::const_iterator const_iterator;

    public:
      Listener() {
        GUM_CONSTRUCTOR( Listener );
      }

      Listener( const Listener& l ) {
        GUM_CONS_CPY( Listener );

        for ( const_iterator it = l._senders.begin(); it != l._senders.end() ; ++it ) {
          ( *it )->duplicateTarget( &l, this );
          _senders.insert( *it );
        }
      }

      virtual ~Listener() {
        GUM_DESTRUCTOR( Listener );
        for ( const_iterator it = _senders.begin(); it != _senders.end() ; ++it )
          ( *it )->detachFromTarget( this );

        _senders.clear();
      }

      INLINE void attachSignal__( __sig__::ISignaler* sender ) {
        _senders.insert( sender );
      }

      INLINE void detachSignal__( __sig__::ISignaler* sender ) {
        _senders.erase( sender );
      }

    private:
      Senders_set _senders;
  };
} // namespace gum

#define GUM_CONNECT(sender,signal,receiver,target) (sender).signal.attach(&(receiver),&target)

#endif // SIGNALER_H__

