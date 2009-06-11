#ifndef SIGNALER_H__
#define SIGNALER_H__

#include <agrum/core/set.h>
#include <agrum/core/list.h>

namespace gum {

  class Listener;


  namespace sig {

    class BasicSignaler {
      public:
        virtual void slot_unlink( Listener* pslot ) = 0;
        virtual void slot_duplicate( const Listener* poldslot, Listener* pnewslot ) = 0;
    };
  } //namespace sig

  class Listener {
    private:
      typedef Set<sig::BasicSignaler *> Senders_set;
      typedef Senders_set::iterator const_iterator; //Senders_set::const_iterator const_iterator;

    public:
      Listener() {
        GUM_CONSTRUCTOR( Listener );
      }

      Listener( const Listener& l ) {
        GUM_CONS_CPY( Listener );

        for ( const_iterator it = l._senders.begin(); it != l._senders.end() ; ++it ) {
          ( *it )->slot_duplicate( &l, this );
          _senders.insert( *it );
        }
      }

      void signal_link( sig::BasicSignaler* sender ) {
        _senders.insert( sender );
      }

      void signal_unlink( sig::BasicSignaler* sender ) {
        _senders.erase( sender );
      }

      virtual ~Listener() {
        GUM_DESTRUCTOR( Listener );
        unlink_all();
      }

      void unlink_all() {
        for ( const_iterator it = _senders.begin(); it != _senders.end() ; ++it )
          ( *it )->slot_unlink( this );

        _senders.clear();
      }

    private:
      Senders_set _senders;
  };
} // namespace gum

#define GUM_CONNECT(sender,signal,receiver,slot) (sender).signal.link(&(receiver),&slot)

#endif // SIGNALER_H__

