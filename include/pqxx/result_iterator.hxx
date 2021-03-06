/*-------------------------------------------------------------------------
 *
 *   FILE
 *      pqxx/result.hxx
 *
 *   DESCRIPTION
 *      definitions for the pqxx::result class and support classes.
 *   pqxx::result represents the set of result rows from a database query
 *   DO NOT INCLUDE THIS FILE DIRECTLY; include pqxx/result instead.
 *
 * Copyright (c) 2001-2017, Jeroen T. Vermeulen <jtv@xs4all.nl>
 *
 * See COPYING for copyright license.  If you did not receive a file called
 * COPYING with this source code, please notify the distributor of this mistake,
 * or contact the author.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQXX_H_RESULT_ITERATOR
#define PQXX_H_RESULT_ITERATOR

#include "pqxx/compiler-public.hxx"
#include "pqxx/compiler-internal-pre.hxx"

#include "pqxx/row"


/* Result iterator.
 *
 * Don't include this header from your own application; it is included for you
 * by other libpqxx headers.
 */

namespace pqxx
{
/// Iterator for rows in a result.  Use as result::const_iterator.
/** A result, once obtained, cannot be modified.  Therefore there is no
 * plain iterator type for result.  However its const_iterator type can be
 * used to inspect its rows without changing them.
 */
class PQXX_LIBEXPORT const_result_iterator :
  public std::iterator<
	std::random_access_iterator_tag,
	const row,
	result::difference_type,
	const_result_iterator,
	row>,
  public row
{
public:
  typedef const row *pointer;
  typedef row reference;
  typedef result_size_type size_type;
  typedef result_difference_type difference_type;

  const_result_iterator() PQXX_NOEXCEPT : row(result(),0) {}
  const_result_iterator(const row &t) PQXX_NOEXCEPT : row(t) {}

  /**
   * @name Dereferencing operators
   */
  //@{
  /** The iterator "points to" its own row, which is also itself.  This
   * allows a result to be addressed as a two-dimensional container without
   * going through the intermediate step of dereferencing the iterator.  I
   * hope this works out to be similar to C pointer/array semantics in useful
   * cases.
   *
   * IIRC Alex Stepanov, the inventor of the STL, once remarked that having
   * this as standard behaviour for pointers would be useful in some
   * algorithms.  So even if this makes me look foolish, I would seem to be in
   * distinguished company.
   */
  pointer operator->() const { return this; }				//[t12]
  reference operator*() const { return row(*this); }			//[t12]
  //@}

  /**
   * @name Manipulations
   */
  //@{
  const_result_iterator operator++(int);				//[t12]
  const_result_iterator &operator++() { ++m_Index; return *this; }	//[t1]
  const_result_iterator operator--(int);				//[t12]
  const_result_iterator &operator--() { --m_Index; return *this; }	//[t12]

  const_result_iterator &operator+=(difference_type i)			//[t12]
      { m_Index = size_type(difference_type(m_Index) + i); return *this; }
  const_result_iterator &operator-=(difference_type i)			//[t12]
      { m_Index = size_type(difference_type(m_Index) - i); return *this; }
  //@}

  /**
   * @name Comparisons
   */
  //@{
  bool operator==(const const_result_iterator &i) const			//[t12]
      {return m_Index==i.m_Index;}
  bool operator!=(const const_result_iterator &i) const			//[t12]
      {return m_Index!=i.m_Index;}
  bool operator<(const const_result_iterator &i) const			//[t12]
      {return m_Index<i.m_Index;}
  bool operator<=(const const_result_iterator &i) const			//[t12]
      {return m_Index<=i.m_Index;}
  bool operator>(const const_result_iterator &i) const			//[t12]
      {return m_Index>i.m_Index;}
  bool operator>=(const const_result_iterator &i) const			//[t12]
      {return m_Index>=i.m_Index;}
  //@}

  /**
   * @name Arithmetic operators
   */
  //@{
  inline const_result_iterator operator+(difference_type) const;	//[t12]
  friend const_result_iterator operator+(				//[t12]
	difference_type,
	const_result_iterator);
  inline const_result_iterator operator-(difference_type) const;	//[t12]
  inline difference_type operator-(const_result_iterator) const;	//[t12]
  //@}

private:
  friend class pqxx::result;
  const_result_iterator(const pqxx::result *r, result_size_type i)
	PQXX_NOEXCEPT :
    row(*r, i) {}
};


/// Reverse iterator for result.  Use as result::const_reverse_iterator.
class PQXX_LIBEXPORT const_reverse_result_iterator :
  private const_result_iterator
{
public:
  typedef const_result_iterator super;
  typedef const_result_iterator iterator_type;
  using iterator_type::iterator_category;
  using iterator_type::difference_type;
  using iterator_type::pointer;
#ifndef _MSC_VER
  using iterator_type::value_type;
  using iterator_type::reference;
#else
  // Workaround for Visual C++.NET 2003, which has access problems
  typedef const row &reference;
  typedef row value_type;
#endif

  const_reverse_result_iterator(					//[t75]
	const const_reverse_result_iterator &rhs) :
    const_result_iterator(rhs) {}
  explicit const_reverse_result_iterator(				//[t75]
	const const_result_iterator &rhs) :
    const_result_iterator(rhs) { super::operator--(); }

  PQXX_PURE const_result_iterator base() const PQXX_NOEXCEPT;		//[t75]

  /**
   * @name Dereferencing operators
   */
  //@{
  using const_result_iterator::operator->;				//[t75]
  using const_result_iterator::operator*;				//[t75]
  //@}

  /**
   * @name Manipulations
   */
  //@{
  const_reverse_result_iterator &operator=(				//[t75]
	const const_reverse_result_iterator &r)
      { iterator_type::operator=(r); return *this; }
  const_reverse_result_iterator operator++()				//[t75]
      { iterator_type::operator--(); return *this; }
  const_reverse_result_iterator operator++(int);			//[t75]
  const_reverse_result_iterator &operator--()				//[t75]
      { iterator_type::operator++(); return *this; }
  const_reverse_result_iterator operator--(int);			//[t75]
  const_reverse_result_iterator &operator+=(difference_type i)		//[t75]
      { iterator_type::operator-=(i); return *this; }
  const_reverse_result_iterator &operator-=(difference_type i)		//[t75]
      { iterator_type::operator+=(i); return *this; }
  //@}

  /**
   * @name Arithmetic operators
   */
  //@{
  const_reverse_result_iterator operator+(difference_type i) const	//[t75]
      { return const_reverse_result_iterator(base() - i); }
  const_reverse_result_iterator operator-(difference_type i)		//[t75]
      { return const_reverse_result_iterator(base() + i); }
  difference_type operator-(						//[t75]
	const const_reverse_result_iterator &rhs) const
      { return rhs.const_result_iterator::operator-(*this); }
  //@}

  /**
   * @name Comparisons
   */
  //@{
  bool operator==(							//[t75]
	const const_reverse_result_iterator &rhs) const PQXX_NOEXCEPT
      { return iterator_type::operator==(rhs); }
  bool operator!=(							//[t75]
	const const_reverse_result_iterator &rhs) const PQXX_NOEXCEPT
      { return !operator==(rhs); }

  bool operator<(const const_reverse_result_iterator &rhs) const	//[t75]
      { return iterator_type::operator>(rhs); }
  bool operator<=(const const_reverse_result_iterator &rhs) const	//[t75]
      { return iterator_type::operator>=(rhs); }
  bool operator>(const const_reverse_result_iterator &rhs) const	//[t75]
      { return iterator_type::operator<(rhs); }
  bool operator>=(const const_reverse_result_iterator &rhs) const	//[t75]
      { return iterator_type::operator<=(rhs); }
  //@}
};


inline const_result_iterator
const_result_iterator::operator+(result::difference_type o) const
{
  return const_result_iterator(
	&m_Home, size_type(result::difference_type(m_Index) + o));
}

inline const_result_iterator
operator+(result::difference_type o, const_result_iterator i)
	{ return i + o; }

inline const_result_iterator
const_result_iterator::operator-(result::difference_type o) const
{
  return const_result_iterator(
	&m_Home,
	result_size_type(result::difference_type(m_Index) - o));
}

inline result::difference_type
const_result_iterator::operator-(const_result_iterator i) const
	{ return result::difference_type(num() - i.num()); }

inline const_result_iterator result::end() const PQXX_NOEXCEPT
	{ return const_result_iterator(this, size()); }


inline const_result_iterator result::cend() const PQXX_NOEXCEPT
	{ return end(); }


inline const_reverse_result_iterator
operator+(result::difference_type n,
	  const const_reverse_result_iterator &i)
	{ return const_reverse_result_iterator(i.base() - n); }

} // namespace pqxx


#include "pqxx/compiler-internal-post.hxx"

#endif
