template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>::BijectionIterator(typename HashTable<FirstType, SecondType>::iterator& iter) :
  __iter(iter)
{
  GUM_CONSTRUCTOR(BijectionIterator);
}

template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>::~BijectionIterator() {
  GUM_DESTRUCTOR(BijectionIterator);
}

template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>::BijectionIterator(const BijectionIterator<FirstType, SecondType>& toCopy) :
  __iter(toCopy.__iter)
{
  GUM_CONS_CPY(BijectionIterator);
}


template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>&
BijectionIterator<FirstType, SecondType>::operator=(const BijectionIterator<FirstType, SecondType>& toCopy) {
  __iter = toCopy.__iter;
}

template <typename FirstType, typename SecondType> INLINE
const FirstType& BijectionIterator<FirstType, SecondType>::first() const {
  return __iter.key();
}


template <typename FirstType, typename SecondType> INLINE
const SecondType& BijectionIterator<FirstType, SecondType>::second() const {
  return *__iter;
}

template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>& BijectionIterator<FirstType, SecondType>::operator++() {
  ++__iter;
  return *this;
}

template <typename FirstType, typename SecondType> INLINE
BijectionIterator<FirstType, SecondType>& BijectionIterator<FirstType, SecondType>::operator--() {
  --__iter;
  return *this;
}

template <typename FirstType, typename SecondType> INLINE
bool BijectionIterator<FirstType, SecondType>::operator!=(const BijectionIterator<FirstType, SecondType>& toCompare) const {
  return __iter != toCompare.__iter;
}


template <typename FirstType, typename SecondType> INLINE
bool BijectionIterator<FirstType, SecondType>::operator==(const BijectionIterator<FirstType, SecondType>& toCompare) const {
  return __iter == toCompare.__iter;
}

// key uniqueness = false because we do our tests

template <typename FirstType, typename SecondType> INLINE
Bijection<FirstType, SecondType>::Bijection(Size size,
					    bool resize) :
  __firstToSecond(size, resize, false),
  __secondToFirst(size, resize, false)
{
  GUM_CONSTRUCTOR(Bijection);
}


template <typename FirstType, typename SecondType> INLINE
Bijection<FirstType, SecondType>::~Bijection() {
  GUM_DESTRUCTOR(Bijection);
}


template <typename FirstType, typename SecondType> INLINE
Bijection<FirstType, SecondType>::Bijection(const Bijection<FirstType, SecondType>& toCopy) :
  __firstToSecond(toCopy.__firstToSecond),
  __secondToFirst(toCopy.__secondToFirst)
{
  GUM_CONS_CPY(Bijection);
}

template <typename FirstType, typename SecondType> INLINE
Bijection<FirstType, SecondType>& 
Bijection<FirstType, SecondType>::operator=(const Bijection<FirstType, SecondType>& toCopy) {
  __firstToSecond = toCopy.__firstToSecond;
  __secondToFirst = toCopy.__secondToFirst;
}

template <typename FirstType, typename SecondType> INLINE
typename Bijection<FirstType, SecondType>::iterator
Bijection<FirstType, SecondType>::begin() const {
  return BijectionIterator<FirstType, SecondType>(__firstToSecond.begin(), &__secondToFirst);
}

template <typename FirstType, typename SecondType> INLINE
typename Bijection<FirstType, SecondType>::iterator
Bijection<FirstType, SecondType>::end() const {
  return BijectionIterator<FirstType, SecondType>(__firstToSecond.end(), &__secondToFirst);
}


template <typename FirstType, typename SecondType> INLINE
const FirstType& Bijection<FirstType, SecondType>::first(const SecondType& second) const {
  return __secondToFirst[second];
}


template <typename FirstType, typename SecondType> INLINE
const FirstType& Bijection<FirstType, SecondType>::firstWithDefault(const SecondType& second, const FirstType& val) const {
  return __secondToFirst.getWithDefault(second, val);
}


template <typename FirstType, typename SecondType> INLINE
const SecondType& Bijection<FirstType, SecondType>::second(const FirstType& first) const {
  return __firstToSecond[first];
}


template <typename FirstType, typename SecondType> INLINE
const SecondType& Bijection<FirstType, SecondType>::secondWithDefault(const FirstType& first, const SecondType& val) const {
  return __firstToSecond.getWithDefault(first, val);
}

template <typename FirstType, typename SecondType> INLINE
bool Bijection<FirstType, SecondType>::existsFirst(const FirstType& first) const {
  return __firstToSecond.exists(first);
}

template <typename FirstType, typename SecondType> INLINE
bool Bijection<FirstType, SecondType>::existsSecond(const SecondType& second) const {
  return __secondToFirst.exists(second);
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::insert(const FirstType& first, const SecondType& second) {
  if(existsFirst(first) || existsSecond(second)) {
    GUM_ERROR( DuplicateElement,
	       "the bijection contains an element with the same key" );
  }
  __firstToSecond.insert(first, second);
  __secondToFirst.insert(second, first);
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::clear() {
  __firstToSecond.clear();
  __secondToFirst.clear();
}

template <typename FirstType, typename SecondType> INLINE
bool Bijection<FirstType, SecondType>::empty() const {
  GUM_ASSERT(__firstToSecond.empty() == __secondToFirst.empty());
  return __firstToSecond.empty();
}

template <typename FirstType, typename SecondType> INLINE
Size Bijection<FirstType, SecondType>::size() const {
  GUM_ASSERT(__firstToSecond.size() == __secondToFirst.size());
  return __firstToSecond.size();
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::eraseFirst(const FirstType& first) {
  try {
    __secondToFirst.erase(__firstToSecond[first]);
    __firstToSecond.erase(first);
  } catch(NotFound&) { }
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::eraseSecond(const SecondType& second) {
  try {
    __firstToSecond.erase(__secondToFirst[second]);
    __secondToFirst.erase(second);
  } catch(NotFound&) { }
}

template <typename FirstType, typename SecondType> INLINE
Size Bijection<FirstType, SecondType>::capacity() const {
  return __firstToSecond.capacity();
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::resize( Size new_size ) {
  __firstToSecond.resize(new_size);
  __secondToFirst.resize(new_size);
}

template <typename FirstType, typename SecondType> INLINE
void Bijection<FirstType, SecondType>::setResizePolicy( const bool new_policy ) {
  __firstToSecond.setResizePolicy(new_policy);
  __secondToFirst.setResizePolicy(new_policy);
}

template <typename FirstType, typename SecondType> INLINE
bool Bijection<FirstType, SecondType>::resizePolicy() const {
  return __firstToSecond.resizePolicy();
}


