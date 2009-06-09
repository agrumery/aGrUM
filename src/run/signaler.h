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

      Listener( const Listener& hs ) {
        GUM_CONS_CPY( Listener );

        const_iterator it = hs._senders.begin();
        const_iterator itEnd = hs._senders.end();

        while ( it != itEnd ) {
          ( *it )->slot_duplicate( &hs, this );
          _senders.insert( *it );
          ++it;
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

        const_iterator it = _senders.begin();
        const_iterator itEnd = _senders.end();

        while ( it != itEnd ) {
          ( *it )->slot_unlink( this );
          ++it;
        }

        _senders.clear();
      }

    private:
      Senders_set _senders;
  };
} // namespace gum

#define GUM_CONNECT(sender,signal,receiver,slot) (sender).signal.link(&(receiver),&slot)

#endif // SIGNALER_H__

