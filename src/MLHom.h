/****************************************************************************/
/*								            */
/* This file is part of libDDD, a library for manipulation of DDD and SDD.  */
/*     						                            */
/*     Copyright (C) 2001-2008 Yann Thierry-Mieg, Jean-Michel Couvreur      */
/*                             and Denis Poitrenaud                         */
/*     						                            */
/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU Lesser General Public License as       */
/*     published by the Free Software Foundation; either version 3 of the   */
/*     License, or (at your option) any later version.                      */
/*     This program is distributed in the hope that it will be useful,      */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/*     GNU LEsserGeneral Public License for more details.                   */
/*     						                            */
/* You should have received a copy of the GNU Lesser General Public License */
/*     along with this program; if not, write to the Free Software          */
/*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*     						                            */
/****************************************************************************/

#ifndef __MLHOM__H__
#define __MLHOM__H__

#include "Hom.h"
#include "AdditiveMap.hpp"

class MLHom;

class HomNodeMap : public AdditiveMap<GHom,GDDD> {
public :
  GDDD build () const ;
};

class HomHomMap : public AdditiveMap<GHom,MLHom> {
};

class _MLHom;
class StrongMLHom;

class MLHom {

   /// By definition, as homomorphism are linear, (h+g) (d) = h(d) + g(d) ;
  /// Where g,h are homomorphisms and d is a DDD.
  friend MLHom operator+(const MLHom &,const MLHom &);
  /// The real implementation class. All true operations are delagated on this pointer.
  /// Construction/destruction take care of ensuring concret is only instantiated once in memory.  
  const _MLHom* concret;
public :

  /// Elementary homomorphism Identity, defined as a constant.
  /// id(d) = <id, d>
  static const MLHom id;

  /// Default public constructor.
  /// Builds Identity homomorphism : forall d in DDD, id(d) = d
  MLHom():concret(id.concret){};
  
  MLHom(const GHom &h);

  MLHom(const StrongMLHom *);
  
  
  /// Create variable/value pair and left concatenate to a homomorphism.
  /// h(var,val,g) (d) = DDD(var,val) ^ g(d).
  /// In other words : var -- val -> g
  /// \param var the variable index
  /// \param val the value associated to the variable
  /// \param h the homomorphism to apply on successor node. Default is identity, so is equivalent to a left concatenation of a DDD(var,val).
  MLHom(int var, int val, const MLHom &h=MLHom::id);  

  virtual ~MLHom();

  
  bool operator<(const MLHom &h) const {return concret<h.concret;};
  bool operator==(const MLHom &h) const {return concret==h.concret;};
  /// Hash key computation. It is essential for good hash table operation that the spread
  /// of the keys be as good as possible. Also, fast hash key computation is a good design goal.
  /// Note that bad hash functions will yield more collisions, thus equality comparisons which
  /// may be quite costly.
  size_t hash() const { return ddd::knuth32_hash(reinterpret_cast<const size_t>(concret)); };
  
  /// The computation function responsible for evaluation over a node.
  /// Users should not directly use this. Normal behavior is to use operator()
  /// that encapsulates this call with operation caching.
  HomNodeMap eval(const GDDD &) const ;
  /// cache calls to eval
  HomNodeMap operator() (const GDDD &) const;

  /// For garbage collection. Used in first phase of garbage collection.
  virtual void mark() const{};

};

/// Composition by union of two homomorphisms. 
/// See also GShom::add(). This commutative operation computes a homomorphism 
/// that evaluates as the sum of two homomorphism.
///
/// Semantics : (h1 + h2) (d) = h1(d) + h2(d).
MLHom operator+(const MLHom &,const MLHom &); 

class _MLHom {
  mutable int refCounter;
public:
  /// Virtual Destructor. 
  virtual ~_MLHom(){};
  
};

class StrongMLHom : public _MLHom {
public :

  virtual HomNodeMap eval(const GDDD &) const ;

  /// User defined behavior is input through this function
  virtual HomHomMap phi (int var,int val) const=0;   

};

#endif