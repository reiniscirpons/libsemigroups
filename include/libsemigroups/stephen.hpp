//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_STEPHEN_HPP_
#define LIBSEMIGROUPS_STEPHEN_HPP_

#include <chrono>       // for high_resolution_clock
#include <cstddef>      // for size_t
#include <cstdint>      // for size_t
#include <type_traits>  // for decay_t, is_base_of_v
#include <utility>      // for forward
#include <vector>       // for vector

#include "constants.hpp"  // for PositiveInfinity
#include "dot.hpp"        // for Dot
#include "exception.hpp"
#include "paths.hpp"            // for const_pislo_iterator etc
#include "presentation.hpp"     // for Presentation
#include "runner.hpp"           // for Runner
#include "to-presentation.hpp"  // for make
#include "types.hpp"            // for word_type
#include "word-graph.hpp"       // for WordGraph, Act...

#include "detail/int-range.hpp"                // for IntegralRange<>::v...
#include "detail/node-managed-graph.hpp"       // for NodeManagedGraph
#include "detail/stl.hpp"                      // for IsStdSharedPtr
#include "detail/word-graph-with-sources.hpp"  // for DigraphWithSources

// TODO(0)
// * iwyu
// * update so that run_for, run_until work properly (at present basically
//   run_impl starts again from scratch every time)
// TODO(2)
// * minimal rep (as per Reinis) (named normal_form?)
// * invert() - just swap the initial and accept states and re-standardize
// * idempotent() - just make the accept state = initial state.
// * class_of for inverse Stephen (i.e. all walks in the graph through all
// nodes) (not sure how to do this just yet). This is different than
// words_accepted see Corollary 3.2 in Stephen's "Presentations of inverse
// monoids" paper (not thesis).
// * canonical_form (as per Howie's book)

//! \defgroup stephen_group Stephen
//!
//! On this page we describe the functionality in `libsemigroups` relating
//! to Stephen's procedure for finitely presented semigroups.

namespace libsemigroups {

  // TODO(0): put reference in bib and use it with \cite
  //! \ingroup stephen_group
  //!
  //! \brief For constructing the word graph of left factors of a word in an
  //! f.p. semigroup.
  //!
  //! Defined in `sims.hpp`.
  //!
  //! This class implements Stephen's procedure for constructing
  //! the WordGraph corresponding to the left factors of a word in a finitely
  //! presented semigroup or a finitely presented inverse semigroup. The
  //! algorithm implemented in this class is closely related to the Todd-Coxeter
  //! algorithm  (as implemented in ToddCoxeter) and originates in [Applications
  //! of automata theory to presentations of monoids and inverse
  //! monoids](https://rb.gy/brsuvc) by J. B. Stephen.
  //!
  //! \tparam PresentationType a type derived from \ref PresentationBase. This
  //! is the type of the underlying presentation used in the Stephen algorithm.
  //! Common choices include \ref Presentation<word_type>, \ref
  //! Presentation<std::string>, \ref InversePresentation<word_type> and \ref
  //! InversePresentation<std::string>. If an \ref InversePresentation is
  //! supplied, then the \ref Stephen class will use the Stephen procedure for
  //! inverse semigroups when run. Otherwise the Stephen procedure for general
  //! semigroups is used instead.
  template <typename PresentationType>
  class Stephen : public Runner {
    template <typename Q>
    static constexpr bool is_valid_presentation() {
      return std::is_same_v<Q, Presentation<word_type>>
             || std::is_same_v<Q, InversePresentation<word_type>>;
      // TODO(2): uncomment when we figure out how to handle std::string
      // presentations
      // TODO(0): initially make it only work with canonical presentations of
      // word_type
      //|| std::is_same_v<Q, Presentation<std::string>>
      //|| std::is_same_v<Q, InversePresentation<std::string>>;
    }

    // TODO (2): Change the error message once std::string presentations are
    // supported
    static_assert(is_valid_presentation<PresentationType>(),
                  "the template parameter PresentationType must be "
                  "Presentation<word_type> or InversePresentation<word_type>");

   public:
    //! The underlying presentation type.
    using presentation_type = PresentationType;

    //! The return type of the function \ref libsemigroups::Stephen::word_graph.
    using word_graph_type = WordGraph<uint32_t>;
    //! The node type of \ref libsemigroups::Stephen::word_graph_type.
    using node_type = word_graph_type::node_type;

   private:
    class StephenGraph;  // forward decl

    // Data members
    node_type                         _accept_state;
    bool                              _finished;
    bool                              _is_word_set;
    std::shared_ptr<PresentationType> _presentation;
    word_type                         _word;
    StephenGraph                      _word_graph;

   public:
    //! \brief Default constructor.
    //!
    //! Default constructs an empty instance, use \ref init and \ref set_word
    //! to specify the presentation and the word, respectively.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Stephen();

    //! \brief Reinitialize an existing Stephen object.
    //!
    //! This function puts a Stephen object back into the same state as if
    //! it had been newly default constructed.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Stephen& init();

    //! \brief Construct from a presentation (copy).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! copies \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    explicit Stephen(PresentationType const& p) : Stephen() {
      init(p);
    }

    //! \brief Construct from a presentation (move).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! moves \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    explicit Stephen(PresentationType&& p) : Stephen() {
      init(std::move(p));
    }

    //! \brief Construct from a shared pointer to presentation (copy).
    //!
    //! Construct an instance from the shared pointer \p ptr to a shared
    //! presentation object. This constructor copies \p ptr.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param ptr a shared pointer to a presentation.
    //!
    //! \throws LibsemigroupsException if `*ptr->validate()` throws.
    //! \throws LibsemigroupsException if `*ptr->alphabet().size()` is `0`.
    explicit Stephen(std::shared_ptr<PresentationType> const& ptr) : Stephen() {
      init(ptr);
    }

    //! Default copy constructor
    Stephen(Stephen const& that) = default;

    //! Default move constructor
    Stephen(Stephen&&) = default;

    //! Default copy assignment operator
    Stephen& operator=(Stephen const&) = default;

    //! Default move assignment operator
    Stephen& operator=(Stephen&&) = default;

    ~Stephen() = default;

    //! \brief Initialize from a presentation (copy).
    //!
    //! This function puts a Stephen object back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! copies \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    Stephen& init(PresentationType const& p) {
      return init(std::make_shared<PresentationType>(p));
    }

    //! \brief Initialize from a presentation (move).
    //!
    //! This function puts a Stephen object back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! moves \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    Stephen& init(PresentationType&& p) {
      return init(std::make_shared<PresentationType>(std::move(p)));
    }

    //! \brief Initialize from a shared pointer to presentation (copy).
    //!
    //! This function puts a Stephen object back into the same state as if it
    //! had been newly constructed from the shared pointer to a presentation \p
    //! ptr. This initializer copies \p ptr.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \param ptr a shared pointer to a presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `ptr->validate()` throws.
    //! \throws LibsemigroupsException if `ptr->alphabet().size()` is `0`.
    Stephen& init(std::shared_ptr<PresentationType> const& ptr);

    //! \brief Construct from a different presentation type (copy).
    //!
    //! Construct an instance from the presentation \p q with a possibly
    //! different underlying presentation type. This constructor copies \p q.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \tparam OtherPresentationType a type derived from \ref PresentationBase
    //!
    //! \param q the presentation.
    //!
    //! \throws LibsemigroupsException if `q.validate()` throws.
    //! \throws LibsemigroupsException if `q.alphabet().size()` is `0`.
    template <typename OtherPresentationType>
    explicit Stephen(OtherPresentationType const& q) : Stephen() {
      init(q);
    }

    //! \brief Initialize from a different presentation type (copy).
    //!
    //! This function puts a Stephen object back into the same state as if it
    //! had been newly constructed from the presentation \p q with a possibly
    //! different underlying presentation type. This initializer copies \p q.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase
    //!
    //! \tparam OtherPresentationType a type derived from \ref PresentationBase
    //!
    //! \param q the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `q.validate()` throws.
    //! \throws LibsemigroupsException if `q.alphabet().size()` is `0`.
    template <typename OtherPresentationType>
    Stephen& init(OtherPresentationType const& q);

    //! \brief Get the input presentation.
    //!
    //! This function returns a const reference to the input presentation.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a \ref presentation_type.
    //!
    //! \exceptions
    //! \noexcept
    presentation_type const& presentation() const noexcept {
      return *_presentation;
    }

    //! \brief Set the initial word (copy).
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \param w a const reference to the input word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the letters in \p w do not all belong
    //! to the alphabet of the \ref presentation.
    Stephen& set_word(word_type const& w);

    //! \brief Set the initial word (move).
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is moved.
    //!
    //! \param w an rvalue reference to the input word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the letters in \p w do not all belong
    //! to the alphabet of the \ref presentation.
    Stephen& set_word(word_type&& w);

    //! \brief Check if the initial word is set
    //!
    //! Returns true if a word has been set with \ref set_word since the last
    //! presentation change and false otherwise.
    //!
    //! \param (None)
    //!
    //! \returns A bool.
    //!
    //! \exceptions
    //! \noexcept
    bool is_word_set() const noexcept {
      return _is_word_set;
    }

    //! \brief Get the initial word.
    //!
    //! Returns a const reference to the word set by \ref set_word.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    word_type const& word() const {
      throw_if_not_ready();
      return _word;
    }

    //! \brief Get the word graph.
    //!
    //! Returns a const reference to the word graph in its present state. The
    //! algorithm implemented in this class is not triggered by calls to this
    //! function.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a \ref word_graph_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    word_graph_type const& word_graph() const {
      throw_if_not_ready();
      return _word_graph;
    }

    //! \brief Get the accept state of the word graph.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns the accept state of
    //! the produced word graph.
    //!
    //! \param (None)
    //!
    //! \returns A \ref node_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    // Throws if run throws, also this is not in the helper namespace because
    // we cache the return value.
    node_type accept_state();

    //! \brief Get the initial state of the word graph.
    //!
    //! This function return the initial state of \ref word_graph.
    //!
    //! \param (None)
    //!
    //! \returns A \ref node_type.
    //!
    //! \exceptions
    //! \noexcept
    static constexpr node_type initial_state() noexcept {
      return 0;
    }

    //! \brief Append a Stephen object
    //!
    //! This function triggers a run of the Stephen algorithm of \c this and \p
    //! that, if it hasn't been run already and then appends the Stephen
    //! object \p that to \c this. This modifies the current Stephen instance
    //! in-place. The result is a Stephen instance with underlying word equal
    //! to the concatenation of \c this.word() and \c that.word().
    //!
    //! The advantage of this is that if either \c this or \p that have already
    //! been run, then we can reuse the underlying word graphs instead of having
    //! to recompute them completely from scratch.
    //!
    //! \param that the Stephen instance to append.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \c this or \p that or with Stephen::init or if no
    //! word was set with Stephen::set_word .
    //!
    //! \throws LibsemigroupsException if the presentations for \c this and \p
    //! that differ.
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    void operator*=(Stephen<PresentationType>& that) {
      if (this->presentation() != that.presentation()) {
        LIBSEMIGROUPS_EXCEPTION(
            "this.presentation() must equal that.presentation() when appending "
            "Stephen instances")
      }
      // TODO(0): if only one of this and that is finished, then just tack on
      // the linear graph.
      // TODO(0): Maybe just append without running? Previously there was an
      // issue with resetting words, but with the current implementation,
      // provided the words have ween set, we should be able to do this with no
      // issues.
      this->run();
      that.run();
      // TODO (2) FIXME _word_graph has two mem fns number_nodes_active (in
      // NodeManager) and number_active_nodes (in WordGraph), this is super
      // confusing!
      size_t const N = _word_graph.number_of_nodes_active();
      _word_graph.disjoint_union_inplace_no_checks(that._word_graph);
      _word_graph.merge_nodes_no_checks(accept_state(),
                                        that.initial_state() + N);
      _word_graph.template process_coincidences<detail::DoNotRegisterDefs>();
      _accept_state = UNDEFINED;
      _finished     = false;
      _word.insert(_word.end(), that._word.cbegin(), that._word.cend());
      _word_graph.cursor() = initial_state();
    }

   private:
    void report_before_run();
    void report_after_run();

    Stephen& init_after_presentation_set();
    void     throw_if_presentation_empty(presentation_type const&) const;
    void     throw_if_not_ready() const;
    void     init_word_graph_from_word_no_checks();

    void run_impl() override;
    void really_run_impl();

    bool finished_impl() const noexcept override {
      return _finished;
    }

    void standardize();
  };

  // Deduction guides
  // The following is not a mistake but intentional, if no presentation type is
  // explicitly used, then we use Presentation<word_type>.
  // Presentation<std::string> is not allowed.
  // TODO(2): allow for Presentation<std::string> by templating on word (after
  // other changes have been made)
  // TODO(0): Remove template (or have empty template) and just replace Word
  // with word_type for now
  // TODO(0): wait what's the point of these deduction guides then?
  template <typename Word>
  Stephen(Presentation<Word> const&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(Presentation<Word>&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(Presentation<Word>&&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(std::shared_ptr<Presentation<Word>>&&)
      -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word> const&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word>&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word>&&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(std::shared_ptr<InversePresentation<Word>>&&)
      -> Stephen<InversePresentation<word_type>>;
  // TODO(2): other shared_ptr guides?

}  // namespace libsemigroups

namespace libsemigroups {
  //! \ingroup stephen_group
  //!
  //! \brief Helper functions for the \ref Stephen class.
  //!
  //! Defined in \c stephen.hpp.
  //!
  //! This page contains documentation for some helper functions for the \ref
  //! Stephen class. The helpers documented on this page all belong to the
  //! namespace `stephen`.
  namespace stephen {

    //! \brief Check if a word is equivalent to Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true if the input
    //! word \p w is equivalent to Stephen::word in the semigroup defined by
    //! Stephen::presentation. A word is equivalent to Stephen::word if it
    //! labels a path in Stephen::word_graph with source \c 0 and target
    //! Stephen::accept_state.
    //!
    //! \param s the Stephen instance
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    template <typename PresentationType>
    bool accepts(Stephen<PresentationType>& s, word_type const& w);

    //! \brief Check if a word is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true if the input
    //! word \p w is a left factor of Stephen::word in the semigroup defined
    //! by Stephen::presentation. A word is a left factor of Stephen::word
    //! if it labels a path in Stephen::word_graph with source \c 0.
    //!
    //! \param s the Stephen instance
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    template <typename PresentationType>
    bool is_left_factor(Stephen<PresentationType>& s, word_type const& w);

    //! \brief Returns a range object containing all words equivalent to
    //! Stephen::word in short-lex order.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already).
    //!
    //! \param s the Stephen instance
    //!
    //! \returns A range object containing all words equivlanet to Stephen::word
    //! in short-lex order.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    template <typename PresentationType>
    auto words_accepted(Stephen<PresentationType>& s) {
      s.run();
      Paths paths(s.word_graph());
      return paths.source(s.initial_state()).target(s.accept_state());
    }

    //! \brief Returns a range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already).
    //!
    //! \param s the Stephen instance
    //!
    //! \returns A range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    template <typename PresentationType>
    auto left_factors(Stephen<PresentationType>& s) {
      s.run();
      Paths paths(s.word_graph());
      return paths.source(s.initial_state());
    }

    //! \brief Returns the number of words accepted with length in a given
    //! range.
    //!
    //! This function returns the number of words that are equivalent to
    //! Stephen::word in the instance \p s with length between \p min and \p
    //! max. This is the same as the number of paths in Stephen::word_graph
    //! (if Stephen::run has been called) with source \c 0, target
    //! Stephen::accept_state,  and length in the range \p min to \p max.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    //!
    //! \sa WordGraph::number_of_paths.
    // Not noexcept because number_of_paths isn't
    template <typename PresentationType>
    uint64_t number_of_words_accepted(Stephen<PresentationType>& s,
                                      size_t                     min = 0,
                                      size_t max = POSITIVE_INFINITY) {
      s.run();
      using node_type = typename Stephen<PresentationType>::node_type;
      return number_of_paths(
          s.word_graph(), node_type(0), s.accept_state(), min, max);
    }

    //! \brief Returns the number of left factors with length in a given range.
    //!
    //! This function returns the number of left factors of the
    //! Stephen::word in the instance \p s with length between \p min and \p
    //! max. This is the same as the number of paths in Stephen::word_graph
    //! (if Stephen::run has been called) with source \c 0 and length in the
    //! range \p min to \p max.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_intf_warn_undecidable{Stephen}.
    //!
    //! \sa WordGraph::number_of_paths.
    // Number of words that represent left factors of word()
    // Not noexcept because number_of_paths isn't
    template <typename PresentationType>
    uint64_t number_of_left_factors(Stephen<PresentationType>& s,
                                    size_t                     min = 0,
                                    size_t max = POSITIVE_INFINITY) {
      s.run();
      return number_of_paths(s.word_graph(), 0, min, max);
    }

    // TODO(0): doc
    template <typename PresentationType>
    Dot dot(Stephen<PresentationType>& s);
  }  // namespace stephen

  //! \brief Check equality of two Stephen instances.
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \param x a Stephen instance
  //! \param y a Stephen instance
  //!
  //! \returns A \c const_iterator_left_factors.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set with
  //! Stephen::set_word .
  //!
  //! \throws LibsemigroupsException if the presentations for \p x and \p y
  //! differ.
  //!
  //! \cong_intf_warn_undecidable{Stephen}.
  template <typename PresentationType>
  bool operator==(Stephen<PresentationType> const& x,
                  Stephen<PresentationType> const& y) {
    if (x.presentation() != y.presentation()) {
      LIBSEMIGROUPS_EXCEPTION(
          "x.presentation() must equal y.presentation() when comparing "
          "Stephen instances")
    }
    return equal_to_no_checks(x, y);
  }

  //! \brief Check equality of two Stephen instances (no checks).
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \param x a Stephen instance
  //! \param y a Stephen instance
  //!
  //! \returns A \c const_iterator_left_factors.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set with
  //! Stephen::set_word .
  //!
  //! \cong_intf_warn_undecidable{Stephen}.
  //!
  //! \warning
  //! No checks are made on the validity of the parameters to this function. Bad
  //! things may happen if \p x and \p y have different underlying
  //! presentations.
  template <typename PresentationType>
  bool equal_to_no_checks(Stephen<PresentationType> const& x,
                          Stephen<PresentationType> const& y) {
    return stephen::accepts(const_cast<Stephen<PresentationType>&>(x), y.word())
           && stephen::accepts(const_cast<Stephen<PresentationType>&>(y),
                               x.word());
  }

  //! \brief Return a human readable representation of a Stephen object.
  //!
  //! Return a human readable representation of a Stephen object.
  //!
  //! \param x the Stephen object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename PresentationType>
  [[nodiscard]] std::string
  to_human_readable_repr(Stephen<PresentationType> const& x);
}  // namespace libsemigroups

#include "stephen.tpp"

#endif  // LIBSEMIGROUPS_STEPHEN_HPP_
