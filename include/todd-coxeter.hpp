//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups.

// TODO:
// 1. In congruence::ToddCoxeter: add methods for add_pair, set_base, etc, for
// use with 0-parameter constructor, and remove other methods/constructors

#ifndef LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_

#include <stddef.h>  // for size_t

#include <cinttypes>      // for int64_t
#include <stack>          // for stack
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "internal/containers.hpp"  // for RecVec

#include "cong-base.hpp"  // for congruence_type, CongBase, CongBa...
#include "types.hpp"      // for relation_type, word_type, letter_...
#include "wrap.hpp"       // for WrappedCong

namespace libsemigroups {
  // Forward declarations
  class TCE;
  class FroidurePinBase;
  namespace congruence {
    class ToddCoxeter;
  }
  namespace fpsemigroup {
    class KnuthBendix;
    using ToddCoxeter = WrappedCong<congruence::ToddCoxeter>;
  }  // namespace fpsemigroup

  namespace congruence {
    class ToddCoxeter : public CongBase {
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs - private
      ////////////////////////////////////////////////////////////////////////
      using signed_class_index_type = int64_t;

     public:
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs + enums - public
      ////////////////////////////////////////////////////////////////////////

      using class_index_type = CongBase::class_index_type;

      enum class policy { none = 0, use_relations = 1, use_cayley_graph = 2 };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - constructors and destructor - public
      ////////////////////////////////////////////////////////////////////////

      explicit ToddCoxeter(congruence_type type);

      ToddCoxeter(congruence_type,
                  FroidurePinBase*,
                  policy = policy::use_relations);
      ToddCoxeter(congruence_type,
                  FroidurePinBase&,
                  policy = policy::use_relations);
      // TODO change to use_cayley_graph

      ToddCoxeter(congruence_type,
                  size_t,
                  std::vector<relation_type> const&,
                  std::vector<relation_type> const& = {});

      ToddCoxeter(congruence_type, fpsemigroup::ToddCoxeter&);
      ToddCoxeter(congruence_type, ToddCoxeter const&);
      ToddCoxeter(congruence_type, fpsemigroup::KnuthBendix&);

      ~ToddCoxeter() = default;

      ////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void run() override;

      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      size_t           nr_classes() override;
      class_index_type word_to_class_index(word_type const&) override;
      word_type        class_index_to_word(class_index_type) override;

      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      bool contains(word_type const&, word_type const&) override;
      bool is_quotient_obviously_finite() override;
      bool is_quotient_obviously_infinite() override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - methods - public
      ////////////////////////////////////////////////////////////////////////

      bool             empty() const;
      letter_type      class_index_to_letter(class_index_type);
      policy           get_policy() const noexcept;
      void             set_pack(size_t);
      class_index_type table(class_index_type, letter_type);

     private:
      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////

      void add_pair_impl(word_type const&, word_type const&) override;
      // Guaranteed to return a FroidurePin<TCE>*.
      FroidurePinBase* quotient_impl() override;

      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////

      class_index_type
           const_word_to_class_index(word_type const&) const override;
      void set_nr_generators_impl(size_t) override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - methods (validation) - private
      ////////////////////////////////////////////////////////////////////////

      void validate_table() const;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - methods (initialisation) - private
      ////////////////////////////////////////////////////////////////////////

      void init();
      void init_after_prefill();
      void init_relations();
      void prefill(FroidurePinBase&);
      void use_relations_or_cayley_graph();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - methods (other) - private
      ////////////////////////////////////////////////////////////////////////

      void compress();
      void new_coset(class_index_type const&, letter_type const&);
      void identify_cosets(class_index_type, class_index_type);
      inline void
      trace(class_index_type const&, relation_type const&, bool = true);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - data - private
      ////////////////////////////////////////////////////////////////////////
      // TODO(later): Pimpl
      size_t                               _active;  // Number of active cosets
      std::vector<signed_class_index_type> _bckwd;
      std::vector<class_index_type>        _class_index_to_letter;
      size_t                               _cosets_killed;
      class_index_type                     _current;
      class_index_type                     _current_no_add;
      size_t                               _defined;
      std::vector<relation_type>           _extra;
      std::vector<class_index_type>        _forwd;
      class_index_type                     _id_coset;
      bool                         _init_done;  // Has init() been run yet?
      class_index_type             _last;
      std::stack<class_index_type> _lhs_stack;
      class_index_type             _next;
      size_t _pack;  // Nr of active cosets allowed before a
                     // packing phase starts
      policy                       _policy;
      bool                         _prefilled;
      RecVec<class_index_type>     _preim_init;
      RecVec<class_index_type>     _preim_next;
      bool                         _relations_are_reversed;
      std::vector<relation_type>   _relations;
      std::stack<class_index_type> _rhs_stack;
      bool                         _stop_packing;
      RecVec<class_index_type>     _table;
    };
  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_
