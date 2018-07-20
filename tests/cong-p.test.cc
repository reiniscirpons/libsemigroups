//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 Michael Torpey
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

// The purpose of this file is to test the Congruence::TC class, this is
// achieved by calling cong->p() before calculating anything about the
// congruence.

#include <utility>

#include "../src/cong-p.h"
#include "../src/fpsemi.h"
#include "catch.hpp"

#define CONG_P_REPORT false

namespace libsemigroups_tests_cong_p {

  using namespace libsemigroups;

  CongruenceInterface::Type TWOSIDED = CongruenceInterface::Type::TWOSIDED;
  CongruenceInterface::Type LEFT     = CongruenceInterface::Type::LEFT;
  CongruenceInterface::Type RIGHT    = CongruenceInterface::Type::RIGHT;

  TEST_CASE("CongP 01: 2-sided congruence on finite semigroup",
            "[quick][cong-p][01]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size() == 88);
    // REQUIRE(S.nrrules() == 18);

    std::vector<relation_t> extra(
        {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
    P<> p(TWOSIDED, &S, extra);

    REQUIRE(p.word_to_class_index({0, 0, 0, 1})
            == p.word_to_class_index({0, 0, 1, 0, 0}));
    REQUIRE(p.finished());
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 21);
    REQUIRE(p.nr_classes() == 21);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 02: left congruence on finite semigroup",
            "[quick][cong-p][02]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 88);
    // REQUIRE(S.nrrules(false) == 18);

    std::vector<relation_t> extra(
        {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
    P<> p(LEFT, &S, extra);
    glob_reporter.set_report(CONG_P_REPORT);

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 69);
    REQUIRE(p.nr_classes() == 69);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 03: right congruence on finite semigroup",
            "[quick][cong-p][03]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 88);
    // REQUIRE(S.nrrules(false) == 18);

    std::vector<relation_t> extra(
        {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
    P<> p(RIGHT, &S, extra);

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 5);
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 72);
    REQUIRE(p.nr_classes() == 72);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 04: trivial congruence on finite semigroup",
            "[quick][cong-p][04]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
           new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.nrrules(false) == 20);

    P<> p(TWOSIDED, &S, {});

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 53);
    REQUIRE(p.nr_classes() == 53);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 05: trivial left congruence on finite semigroup",
            "[quick][cong-p][05]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
           new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.nrrules(false) == 20);

    std::vector<relation_t> extra({});
    P<>                     p(LEFT, &S, extra);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 53);
    REQUIRE(p.nr_classes() == 53);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 06: trivial right congruence on finite semigroup",
            "[quick][cong-p][06]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
           new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.nrrules(false) == 20);

    std::vector<relation_t> extra({});
    P<>                     p(RIGHT, &S, extra);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 53);
    REQUIRE(p.nr_classes() == 53);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 07: universal congruence on finite semigroup",
            "[quick][cong-p][07]") {
    glob_reporter.set_report(CONG_P_REPORT);
    std::vector<Element*> gens
        = {new PartialPerm<u_int16_t>({0, 1, 3}, {4, 1, 0}, 5),
           new PartialPerm<u_int16_t>({0, 1, 2, 3, 4}, {0, 2, 4, 1, 3}, 5)};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 142);
    // REQUIRE(S.nrrules(false) == 32);

    std::vector<relation_t> extra({relation_t({1}, {0, 0, 0, 1, 0})});
    P<>                     p(TWOSIDED, &S, extra);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) == p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 1);
    REQUIRE(p.nr_classes() == 1);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 08: 2-sided congruence on finite semigroup",
            "[extreme][cong-p][08]") {
    glob_reporter.set_report(true);
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
           new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
           new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
           new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.nrrules(false) == 2460);

    std::vector<relation_t> extra(
        {relation_t({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
    P<> p(TWOSIDED, &S, extra);

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);

    REQUIRE(p.word_to_class_index({1, 2, 1, 3, 3, 2, 1, 2})
            == p.word_to_class_index({2, 1, 3, 3, 2, 1, 0}));
    REQUIRE(p.word_to_class_index({0, 3, 1, 1, 1, 3, 2, 2, 1, 0})
            == p.word_to_class_index({0, 3, 2, 2, 1}));
    REQUIRE(p.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
            != p.word_to_class_index({0, 0, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0})
            != p.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 525);
    REQUIRE(p.nr_classes() == 525);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 09: 2-sided congruence on finite semigroup",
            "[standard][cong-p][09]") {
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
           new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
           new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
           new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.nrrules(false) == 2460);

    std::vector<relation_t> extra(
        {relation_t({1, 3, 0, 1, 2, 2, 0, 2}, {1, 0, 0, 1, 3, 1})});
    P<> p(TWOSIDED, &S, extra);
    glob_reporter.set_report(CONG_P_REPORT);

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);
    REQUIRE(p.word_to_class_index({0, 1, 1, 2, 3}) == 0);

    REQUIRE(p.word_to_class_index({0, 1, 1, 2, 3})
            == p.word_to_class_index({1, 0, 3, 3, 3, 2, 0}));
    REQUIRE(p.word_to_class_index({3, 0, 2, 0, 2, 0, 2})
            == p.word_to_class_index({1, 2, 3, 1, 2}));
    REQUIRE(p.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
            != p.word_to_class_index({0, 0, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0})
            != p.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 9597);
    REQUIRE(p.nr_classes() == 9597);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 10: left congruence on big finite semigroup",
            "[quick][cong-p][10]") {
    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
           new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
           new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
           new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    // The following lines are intentionally commented out so that we can check
    // that P does not enumerate the semigroup, they remain to remind us of the
    // size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.nrrules(false) == 2460);
    std::vector<relation_t> extra(
        {relation_t({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
    P<> p(LEFT, &S, extra);
    glob_reporter.set_report(CONG_P_REPORT);

    REQUIRE(p.word_to_class_index({1, 1, 0, 3}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);
    REQUIRE(p.word_to_class_index({2, 2, 0, 1}) == 0);

    REQUIRE(p.word_to_class_index({1, 1, 3, 2, 2, 1, 3, 1, 3, 3})
            == p.word_to_class_index({2, 2, 0, 1}));
    REQUIRE(p.word_to_class_index({2, 1, 3, 1, 2, 2, 1, 3, 3})
            == p.word_to_class_index({1, 2, 1, 2, 1, 3, 2, 2, 1, 3, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0, 3})
            != p.word_to_class_index({1, 0, 3, 2, 0, 2, 0, 3, 2, 2, 1}));
    REQUIRE(p.word_to_class_index({1, 3, 2, 1, 3, 1, 3, 2, 2, 1, 3, 3, 3})
            != p.word_to_class_index({3, 1, 0, 2, 0, 3, 1}));

    REQUIRE(!S.is_begun());
    REQUIRE(!S.is_done());

    REQUIRE(p.nr_classes() == 7449);
    REQUIRE(p.nr_classes() == 7449);
    REQUIRE(S.is_done());  // nr_classes requires S.size();
  }

  TEST_CASE("CongP 11: run_for", "[quick][cong-p][11]") {
    glob_reporter.set_report(CONG_P_REPORT);
    RWS rws;
    rws.set_alphabet("ab");
    rws.add_relation("aa", "a");
    rws.add_relation("ab", "a");
    rws.add_relation("ba", "a");

    P<RWSE*> p(TWOSIDED, &rws);

    REQUIRE(p.contains({0, 0}, {0}));
    REQUIRE(p.contains({0, 1}, {0}));
    REQUIRE(p.contains({1, 0}, {0}));

    p.add_pair({1, 1, 1, 1, 1, 1, 1}, {1});
    p.run_for(std::chrono::milliseconds(200));
  }
}  // namespace libsemigroups_tests_cong_p
