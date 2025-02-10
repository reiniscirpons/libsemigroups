//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_
#define LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_

#include <cstddef>      // for size_t
#include <type_traits>  // for enable_if_t, is_base_of

#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "word-graph.hpp"  // for WordGraph

#include "detail/kbe.hpp"  // for KBE
#include "detail/ke.hpp"   // for KE
#include "detail/rewriters.hpp"
#include "detail/tce.hpp"  // for TCE

namespace libsemigroups {
#ifndef PARSED_BY_DOXYGEN
  class FroidurePinBase;  // forward decl
  template <typename Word>
  class Congruence;

  namespace detail {
    class ToddCoxeterImpl;

    template <typename Rewriter, typename ReductionOrder>
    class KnuthBendixImpl;

  }  // namespace detail
#endif  // PARSED_BY_DOXYGEN

  // TODO(0) doc
  template <typename Word>
  FroidurePin(Kambites<Word> const&) -> FroidurePin<detail::KE<Word>>;

  // TODO(0) doc
  template <typename Rewriter, typename ReductionOrder>
  FroidurePin(detail::KnuthBendixImpl<Rewriter, ReductionOrder> const&)
      -> FroidurePin<
          detail::KBE<detail::KnuthBendixImpl<Rewriter, ReductionOrder>>>;

  // TODO(0) doc
  FroidurePin(detail::ToddCoxeterImpl const&) -> FroidurePin<detail::TCE>;

  ////////////////////////////////////////////////////////////////////////
  // Congruence
  ////////////////////////////////////////////////////////////////////////

  // TODO doc
  template <template <typename...> typename Thing, typename Word>
  auto to(Congruence<Word>& cong)
      -> std::enable_if_t<std::is_same_v<Thing<int>, FroidurePin<int>>,
                          std::unique_ptr<FroidurePinBase>>;

  ////////////////////////////////////////////////////////////////////////
  // Kambites
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing, typename Word>
  auto to(Kambites<Word>& k) -> std::enable_if_t<
      std::is_same_v<Thing<detail::KE<Word>>, FroidurePin<detail::KE<Word>>>,
      FroidurePin<detail::KE<Word>>>;

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix
  ////////////////////////////////////////////////////////////////////////

  // TODO doc
  template <template <typename...> typename Thing,
            typename Rewriter,
            typename ReductionOrder,
            typename Element
            = detail::KBE<detail::KnuthBendixImpl<Rewriter, ReductionOrder>>>
  auto to(detail::KnuthBendixImpl<Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<Thing<Element>, FroidurePin<Element>>,
                          FroidurePin<Element>>;

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter
  ////////////////////////////////////////////////////////////////////////
  // TODO doc

  template <template <typename...> typename Thing>
  auto to(detail::ToddCoxeterImpl& tc) -> std::enable_if_t<
      std::is_same_v<Thing<detail::TCE>, FroidurePin<detail::TCE>>,
      FroidurePin<detail::TCE>>;

  ////////////////////////////////////////////////////////////////////////
  // WordGraph
  ////////////////////////////////////////////////////////////////////////

  //! Make a FroidurePin object from a WordGraph.
  //!
  //! If \f$m\f$ is the number of nodes in a WordGraph, \f$0 \leq a,  b< m\f$,
  //! and \f$n\f$ is an edge label, then we define \f$f: \{a, \ldots, b - 1\}
  //! \to \{0, \ldots, n - 1\}\f$ so that \f$(x)f\f$ equals the target of the
  //! edge starting at node \f$x\f$ with label \f$n\f$. In this way, every edge
  //! label in a WordGraph corresponds to a transformation of the nodes of the
  //! digraph. If \f$\{a, \ldots, b - 1\}f \subseteq \{a, \ldots, b - 1\}\f$,
  //! then \f$f\f$ is a transformation in the sense of Transf. Assuming that for
  //! every edge label of the WordGraph the corresponding \f$f\f$ satisfies
  //! \f$\{a, \ldots, b - 1\}f \subseteq \{a, \ldots, b - 1\}\f$, then this
  //! function returns the FroidurePin object corresponding to the semigroup
  //! generated by the set of all such transformations.
  //!
  //! \tparam S the type of the FroidurePin object being constructed (must
  //! be derived from FroidurePinBase).
  //! \tparam T the type of the nodes of the digraph.
  //!
  //! \param wg the WordGraph being used to construct the FroidurePin
  //! object.
  //! \param first the value of \f$a\f$ in the preceding discussion
  //! \param last the value of \f$b\f$ in the preceding discussion
  //!
  //! \returns The constructed FroidurePin object, a value of type \p S.
  //!
  //! \throws LibsemigroupsException if \ref validate(Transf<N, Scalar>
  //! const&) throws for any of the constructed transformations. This can
  //! happen if, for example, the WordGraph is not complete (i.e. there
  //! exists an edge label and node for which there is no edge with the
  //! given label and given source) or if there is an edge label such that
  //! \f$\{a, \ldots, b - 1\}f \not\subseteq \{a, \ldots, b - 1\}\f$ for the
  //! corresponding \f$f\f$.
  template <typename Result, typename Node>
  std::enable_if_t<
      std::is_same_v<FroidurePin<typename Result::element_type>, Result>,
      Result>
  to(WordGraph<Node> const& wg, size_t first, size_t last);

  //! Make a FroidurePin object from a WordGraph.
  //!
  //! Calls `to_froidure_pin(wg, 0, wg.number_of_nodes())`; see above.
  template <typename Result, typename Node>
  std::enable_if_t<
      std::is_same_v<FroidurePin<typename Result::element_type>, Result>,
      Result>
  to(WordGraph<Node> const& wg) {
    return to<Result>(wg, 0, wg.number_of_nodes());
  }
}  // namespace libsemigroups

#include "to-froidure-pin.tpp"
#endif  // LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_
