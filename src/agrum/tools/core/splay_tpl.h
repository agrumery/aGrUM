/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template implementation of splay trees
 *
 * @author Karim Tekkal
 */
#include <agrum/tools/core/splay.h>

namespace gum {
  /* =========================================================================*/
  /* =========================================================================*/
  /* ===                 Implementation of gumSplayBinaryNode === */
  /* =========================================================================*/
  /* =========================================================================*/

  // a function used to perform copies of HashTableLists

  template < class Element >
  INLINE void
     SplayBinaryNode< Element >::copy_(const SplayBinaryNode< Element >&                  from,
                                       HashTable< Element, SplayBinaryNode< Element >* >& addr) {
    if (addr.exists(from.elt)) addr[from.elt] = this;
    else addr.insert(from.elt, this);

    elt = from.elt;

    size = from.size;

    pere = 0;

    if (from.fg) {
      fg       = new SplayBinaryNode< Element >(*from.fg, addr);
      fg->pere = this;
    } else {
      fg = 0;
    }

    if (from.fd) {
      fd       = new SplayBinaryNode< Element >(*from.fd, addr);
      fd->pere = this;
    } else {
      fd = 0;
    }
  }

  // constructor

  template < class Element >
  INLINE SplayBinaryNode< Element >::SplayBinaryNode(
     const Element&                                     e,
     HashTable< Element, SplayBinaryNode< Element >* >& addr,
     SplayBinaryNode*                                   g,
     SplayBinaryNode*                                   d,
     SplayBinaryNode*                                   p) :
      elt(e),
      size(1), fg(g), fd(d), pere(p) {
    if (addr.exists(elt)) addr[elt] = this;
    else addr.insert(elt, this);

    // for debugging purposes
    GUM_CONSTRUCTOR(SplayBinaryNode);
  }

  // copy operator

  template < class Element >
  INLINE SplayBinaryNode< Element >::SplayBinaryNode(
     const SplayBinaryNode< Element >&                  from,
     HashTable< Element, SplayBinaryNode< Element >* >& addr) {
    copy_(from, addr);
    // for debugging purposes
    GUM_CONSTRUCTOR(SplayBinaryNode);
  }

  // destructor

  template < class Element >
  INLINE SplayBinaryNode< Element >::~SplayBinaryNode() {
    // for debugging purposes
    GUM_DESTRUCTOR(SplayBinaryNode);
    // remove the subtrees

    if (fg) delete fg;

    if (fd) delete fd;
  }

  // Perform a right rotation, returns the node

  template < class Element >
  INLINE SplayBinaryNode< Element >* SplayBinaryNode< Element >::zig() {
    Size size_;
    // Must be called on a node with a father
    GUM_ASSERT(pere != 0);
    // We chain childs
    pere->fg = fd;

    if (fd) fd->pere = pere;

    fd = pere;

    SplayBinaryNode< Element >* p = fd->pere;

    fd->pere = this;

    // We compute the size of rigth child
    size_ = 1;

    if (fd->fg) size_ += fd->fg->size;

    if (fd->fd) size_ += fd->fd->size;

    fd->size = size_;

    // size_ == fd->size
    if (fg) size_ += fg->size;

    ++size_;

    size = size_;

    // We chain father
    pere = p;

    if (pere) {
      if (pere->fg == fd) {
        // I'm left child of my father
        pere->fg = this;
      } else {
        // I'm right child of my father
        GUM_ASSERT(pere->fd == fd);
        pere->fd = this;
      }
    }

    return this;
  }

  // Perform a left rotation, returns the node

  template < class Element >
  INLINE SplayBinaryNode< Element >* SplayBinaryNode< Element >::zag() {
    Size size_;
    // Must be call on a node with a father
    GUM_ASSERT(pere != 0);
    // We chain childs
    pere->fd = fg;

    if (fg) fg->pere = pere;

    fg = pere;

    SplayBinaryNode< Element >* p = fg->pere;

    fg->pere = this;

    // We compute the size of rigth child
    size_ = 1;

    if (fg->fg) size_ += fg->fg->size;

    if (fg->fd) size_ += fg->fd->size;

    fg->size = size_;

    if (fd) size_ += fd->size;

    ++size_;

    size = size_;

    // We chain father
    pere = p;

    if (pere) {
      if (pere->fg == fg) {
        // I'm left child of my father
        pere->fg = this;
      } else {
        // I'm right child of my father
        GUM_ASSERT(pere->fd == fg);
        pere->fd = this;
      }
    }

    return this;
  }

  // Perform a splay rotation, the node return is the root

  template < class Element >
  INLINE SplayBinaryNode< Element >* SplayBinaryNode< Element >::splay() {
    SplayBinaryNode< Element >* gdp;

    while (pere) {
      // While this node isn't the root
      gdp = pere->pere;   // gdp can be nullptr

      if (!gdp) {
        if (this == pere->fg) {
          // I'm the left child of my father
          return zig();
        } else {
          GUM_ASSERT(this == pere->fd);
          // I'm the right child of my father
          return zag();
        }
      } else {
        if (this == pere->fg) {
          // I'm the left child of my father
          if (pere == gdp->fg) {
            gdp->fg = zig();
          } else {
            GUM_ASSERT(pere == gdp->fd);
            gdp->fd = zig();
          }
        } else {
          GUM_ASSERT(this == pere->fd);
          // I'm the right child of my father

          if (pere == gdp->fg) {
            gdp->fg = zag();
          } else {
            GUM_ASSERT(pere == gdp->fd);
            gdp->fd = zag();
          }
        }
      }
    }

    return this;   // for compiler satisfaction
  }

  // Concatenation of two threes

  template < class Element >
  INLINE SplayBinaryNode< Element >*
         SplayBinaryNode< Element >::join(const SplayBinaryNode< Element >*                  e,
                                      HashTable< Element, SplayBinaryNode< Element >* >& addr) {
    SplayBinaryNode< Element >* b = new SplayBinaryNode< Element >(*e, addr);
    GUM_ASSERT(b != 0);
    SplayBinaryNode< Element >* act = this;

    for (; act->fd; act = act->fd)
      ;

    // act is the rightmost element
    act->splay();

    // insertion
    act->fd = b;

    b->pere = act;

    act->size = 1;

    if (act->fg) act->size += act->fg->size;

    act->size += act->fd->size;

    return act;
  }

  // Get the position of the node

  template < class Element >
  INLINE int SplayBinaryNode< Element >::position() const {
    if (!pere) {
      // I'm the root
      if (fg) return fg->size + 1;
      else return 0;
    } else if (pere->fg == this) {
      // I'm the left child of my father
      int pos = pere->position() - 1;

      if (fd) pos -= fd->size;

      return pos;
    } else {
      // I'm the right child of my father
      int pos = pere->position() + 1;

      if (fg) pos += fg->size;

      return pos;
    }
  }

  // Get the element in the node

  template < class Element >
  INLINE const Element& SplayBinaryNode< Element >::getElement() const {
    return elt;
  }

  /*
   * @return the left child
   * @warning the returned value can be null
   */

  template < class Element >
  INLINE const SplayBinaryNode< Element >* SplayBinaryNode< Element >::getFg() const {
    return fg;
  }

  /*
   * @return the right child
   * @warning the returned value can be null
   */

  template < class Element >
  INLINE const SplayBinaryNode< Element >* SplayBinaryNode< Element >::getFd() const {
    return fd;
  }

  /* =========================================================================*/
  /* =========================================================================*/
  /* ===                     Implementation of SplayTree === */
  /* =========================================================================*/
  /* =========================================================================*/

  // a function used to perform copies

  template < class Element >
  INLINE void SplayTree< Element >::copy_(const SplayTree< Element >& from) {
    if (from.root) {
      root = new SplayBinaryNode< Element >(*from.root, addr);
    } else {
      root = 0;
    }
  }

  // basic constructor, make an empty splay tree

  template < class Element >
  INLINE SplayTree< Element >::SplayTree() : root(0), addr() {
    // for debugging purposes
    GUM_CONSTRUCTOR(SplayTree);
  }

  // basic constructor, make a splay tree with one element
  /*
   * @param e the element of the tree
   */

  template < class Element >
  INLINE SplayTree< Element >::SplayTree(const Element& e) : root(0), addr() {
    root = new SplayBinaryNode< Element >(e, addr);
    // for debugging purposes
    GUM_CONSTRUCTOR(SplayTree);
  }

  // copy constructor

  template < class Element >
  INLINE SplayTree< Element >::SplayTree(const SplayTree& from) : addr() {
    copy_(from);
    // for debugging purposes
    GUM_CONS_CPY(SplayTree);
  }

  // Assignment operator

  template < class Element >
  INLINE SplayTree< Element >& SplayTree< Element >::operator=(const SplayTree< Element >& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(SplayTree);
      // delete the old contents

      if (root) delete root;

      addr.clear();

      copy_(from);
    }

    return *this;
  }

  // Destructor

  template < class Element >
  INLINE SplayTree< Element >::~SplayTree() {
    // for debugging purposes
    GUM_DESTRUCTOR(SplayTree);

    if (root) delete (root);
  }

  // Get the element at the position n

  template < class Element >
  Element& SplayTree< Element >::operator[](const unsigned int i) {
    int val = i;

    if (!root) {
      GUM_ERROR(NotFound, "The tree is empty !")
    } else if (val >= root->size) {
      GUM_ERROR(NotFound, "The index is too large !")
    } else {
      // The element exists
      // Find it
      SplayBinaryNode< Element >* act     = root;
      int                         pos_act = val - 1;
      bool                        next    = true;

      while (next) {
        if (!act->fg) pos_act = 0;
        else pos_act = act->fg->size;

        if (pos_act > val) {
          act = act->fg;
        } else if (pos_act < val) {
          act = act->fd;
          val -= (pos_act + 1);
        } else {
          next = false;
        }
      }

      root = act->splay();

      return root->elt;
    }
  }

  template < class Element >
  const Element& SplayTree< Element >::operator[](const unsigned int i) const {
    int val = i;

    if (!root) {
      GUM_ERROR(NotFound, "The tree is empty !")
    } else if (val >= root->size) {
      GUM_ERROR(NotFound, "The index is too large !")
    } else {
      // The element exists
      // Find it
      SplayBinaryNode< Element >* act     = root;
      int                         pos_act = val - 1;
      bool                        next    = true;

      while (next) {
        if (!act->fg) pos_act = 0;
        else pos_act = act->fg->size;

        if (pos_act > val) {
          act = act->fg;
        } else if (pos_act < val) {
          act = act->fd;
          val -= (pos_act + 1);
        } else {
          next = false;
        }
      }

      root = act->splay();

      return root->elt;
    }
  }

  // Get the first element

  template < class Element >
  INLINE Element& SplayTree< Element >::front() {
    SplayBinaryNode< Element >* act = root;

    if (!root) { GUM_ERROR(NotFound, "The splay tree is empty") }

    if (act->fg)
      for (; act->fg; act = act->fg)
        ;

    root = act->splay();

    return root->elt;
  }

  // Get the last element

  template < class Element >
  INLINE Element& SplayTree< Element >::back() {
    SplayBinaryNode< Element >* act = root;

    if (!root) { GUM_ERROR(NotFound, "The splay tree is empty") }

    if (act->fd)
      for (; act->fd; act = act->fd)
        ;

    root = act->splay();

    return root->elt;
  }

  // Remove the first element

  template < class Element >
  INLINE void SplayTree< Element >::popFront() {
    SplayBinaryNode< Element >* act = root;

    if (root) {
      if (root->fg)
        for (; act->fg; act = act->fg)
          ;

      act = act->splay();

      root = act->fd;

      if (root) root->pere = 0;

      act->fd = 0;

      delete act;
    }
  }

  // Remove the last element

  template < class Element >
  INLINE void SplayTree< Element >::popBack() {
    SplayBinaryNode< Element >* act = root;

    if (root) {
      if (root->fd)
        for (; act->fd; act = act->fd)
          ;

      act = act->splay();

      root = act->fg;

      if (root) root->pere = 0;

      act->fg = 0;

      delete act;
    }
  }

  // Add an element in the first position

  template < class Element >
  INLINE void SplayTree< Element >::pushFront(const Element& e) {
    SplayBinaryNode< Element >* act = root;

    if (root) {
      if (root->fg)
        for (; act->fg; act = act->fg)
          ;

      root = act->splay();

      root->fg = new SplayBinaryNode< Element >(e, addr, 0, 0, root);
    } else {
      root = new SplayBinaryNode< Element >(e, addr);
    }
  }

  // Add an element in the last position

  template < class Element >
  INLINE void SplayTree< Element >::pushBack(const Element& e) {
    SplayBinaryNode< Element >* act = root;

    if (root) {
      if (root->fd)
        for (; act->fd; act = act->fd)
          ;

      root = act->splay();

      root->fd = new SplayBinaryNode< Element >(e, addr, 0, 0, root);
    } else {
      root = new SplayBinaryNode< Element >(e, addr);
    }
  }

  // Add an element to the tree

  template < class Element >
  INLINE void SplayTree< Element >::insert(const Element& e) {
    SplayBinaryNode< Element >* act = root;

    if (!root) {
      root = new SplayBinaryNode< Element >(e, addr);
    } else {
      while (act->fd) {
        ++act->size;
        act = act->fd;
      }

      // act->fd == nullptr
      act->fd = new SplayBinaryNode< Element >(e, addr, 0, 0, act);

      ++act->size;

      root = act->fd->splay();
    }
  }

  // Concatenation of two trees
  /*
   * @param s the tree to add
   */

  template < class Element >
  INLINE void SplayTree< Element >::join(const SplayTree< Element >& s) {
    if (s.root) {
      if (root) {
        root = root->join(s.root, addr);
      } else {
        root = new SplayBinaryNode< Element >(*s.root, addr);
      }
    }
  }

  // removeInfo removes all the information about the nodes contains by e

  template < class Element >
  INLINE static void removeInfo(const SplayBinaryNode< Element >*                  e,
                                HashTable< Element, SplayBinaryNode< Element >* >& addr) {
    GUM_ASSERT(addr.exists(e->getElement()));
    addr.erase(e->getElement());

    if (e->getFg()) removeInfo(e->getFg(), addr);

    if (e->getFd()) removeInfo(e->getFd(), addr);
  }

  // Split the tree at the element

  template < class Element >
  INLINE SplayTree< Element > SplayTree< Element >::split(const int i) {
    GUM_ASSERT(i >= 0 && i < root->size);
    GUM_ASSERT(root != 0);

    if (i == 0) {
      // the tree will be empty
      // the returned tree is a copy of the present tree
      SplayTree< Element > s = *this;
      addr.clear();
      delete root;
      root = 0;
      return s;
    } else if (i == root->size - 1) {
      SplayTree< Element > s;
      return s;
    } else {
      // We will find the node at position i
      SplayBinaryNode< Element >* act = root;
      int                         pos = root->position();

      while (pos != i) {
        GUM_ASSERT(act != 0);

        if (i < pos) {
          act = act->fg;
        } else {
          act = act->fd;
        }

        pos = act->position();
      }

      // We reorganize the tree
      act->splay();

      // We take the first part
      root = act->fg;

      if (root) root->pere = 0;

      // We take the second part
      SplayTree< Element > s;

      s.root = act;

      s.root->fg = 0;

      Size size_ = 1;

      if (s.root->fd) size_ += s.root->fd->size;

      s.root->size = size_;

      // We remove the old nodes in the hash table
      // Complexity O(n) => very expensive
      removeInfo(act, addr);

      return s;
    }
  }

  // Split the tree at the element

  template < typename Element >
  INLINE SplayTree< Element > SplayTree< Element >::split_by_val(const Element& e) {
    GUM_ASSERT(root != 0);

    if (!addr.exists(e)) { GUM_ERROR(NotFound, "not enough elements in the splay tree") }

    // We will find the node at position i
    SplayBinaryNode< Element >* act = addr[e];

    // We reorganize the tree
    act->splay();

    // We take the two parts
    SplayTree< Element > s;

    s.root = act->fd;

    if (s.root) { s.root->pere = 0; }

    root = act->fg;

    if (root) root->pere = 0;

    act->fg = 0;

    // We remove the old nodes in the hash table
    // Complexity O(n) => very expensive
    removeInfo(act, addr);

    act->fd = 0;

    delete act;

    return s;
  }

  // The number of elements in the tree

  template < class Element >
  INLINE Size SplayTree< Element >::size() const {
    if (root) return root->size;
    else return Size(0);
  }

  // Test if the tree contains the element

  template < class Element >
  INLINE bool SplayTree< Element >::contains(const Element& e) const {
    return addr.exists(e);
  }

  // Display the node

  template < typename Element >
  std::ostream& operator<<(std::ostream& out, const SplayBinaryNode< Element >& e) {
    if (e.fg) out << *e.fg << ",";

    out << e.elt;

    if (e.fd) out << "," << *e.fd;

    return out;
  }

  // Display the tree

  template < typename Element >
  INLINE std::ostream& operator<<(std::ostream& out, const SplayTree< Element >& s) {
    out << "|[";

    if (s.root) out << *s.root;

    out << "]|";

    return out;
  }

} /* namespace gum */
