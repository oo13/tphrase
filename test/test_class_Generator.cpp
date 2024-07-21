/* test for class Generator

   This file is part of TPhrase.

   TPhrase is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   TPhrase is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with TPhrase.  If not, see <http://www.gnu.org/licenses/>.

   Copyright © 2024 OOTA, Masato
*/

#include <ios>
#include <iterator>
#include <sstream>
#include <utility>

#include "tphrase/Generator.h"

#include "UnitTest.h"
#include "unit_test_utility.h"

namespace {
    struct PhraseNumber_t {
        std::size_t syntax;
        std::size_t combination;
        double weight;

        PhraseNumber_t(std::size_t s, std::size_t c, double w)
            : syntax{s}, combination{c}, weight{w}
        {
        }
        PhraseNumber_t(const tphrase::Generator &a)
            : syntax{a.get_number_of_syntax()},
              combination{a.get_combination_number()},
              weight{a.get_weight()}
        {
        }
        bool operator==(const PhraseNumber_t &a) const
        {
            return syntax == a.syntax
                && combination == a.combination
                && weight == a.weight;
        }
    };
}

std::size_t test_class_Generator()
{
    UnitTest ut("class Generator");

    auto stub_random{get_sequence_random_func({})};
    auto default_gsub{tphrase::Generator::get_gsub_function_creator()};

    ut.set_enter_function([&]() {
        tphrase::Generator::set_random_function(stub_random);
    });
    ut.set_leave_function([&]() {
        tphrase::Generator::set_gsub_function_creator(default_gsub);
        return true;
    });

    ut.set_test("Default Constructor", [&]() {
        tphrase::Generator ph;
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 0
            && ph.get_weight() == 0
            && ph.get_combination_number() == 0;
    });

    ut.set_test("Constructor with Syntax (copy)", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {Y} | {Z}

            Y = Y1 | Y2 | Y3 |

            Z = Z1 | Z2 | Z3
        )"};
        tphrase::Generator ph{syntax};
        auto r = ph.generate();
        return r == "nil"
            && syntax.get_error_message().size() == 1
            && syntax.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Constructor with Syntax (move)", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {Y} | {Z}

            Y = Y1 | Y2 | Y3 |

            Z = Z1 | Z2 | Z3
        )"};
        tphrase::Generator ph{std::move(syntax)};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Constructor with Syntax (copy) and start condition", [&]() {
        tphrase::Syntax syntax{R"(
            main2 = {:= A | B | C } | {Y} | {Z}

            Y = Y1 | Y2 | Y3 |

            Z = Z1 | Z2 | Z3
        )"};
        tphrase::Generator ph{syntax, "main2"};
        auto r = ph.generate();
        return r == "nil"
            && syntax.get_error_message().size() == 1
            && syntax.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Constructor with Syntax (move) and start condition", [&]() {
        tphrase::Syntax syntax{R"(
            main3 = {:= A | B | C } | {Y} | {Z}

            Y = Y1 | Y2 | Y3 |

            Z = Z1 | Z2 | Z3
        )"};
        tphrase::Generator ph{std::move(syntax), "main3"};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Constructor via R-Value Syntax (a pair of input iterators)#1", [&]() {
        std::istringstream s{R"(
            main = {:= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        tphrase::Generator ph{{std::istreambuf_iterator<char>{s},
                               std::istreambuf_iterator<char>{}}};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Constructor via R-Value Syntax (a pair of input iterators)#2", [&]() {
        std::istringstream s{R"(
            main = {:= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        s.unsetf(std::ios_base::skipws);
        tphrase::Generator ph{{std::istream_iterator<char>{s},
                               std::istream_iterator<char>{}}};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Constructor via R-Value Syntax (a std::string)", [&]() {
        std::string s{R"(
            main = {:= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        tphrase::Generator ph{s};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Constructor via R-Value Syntax (a const char*)", [&]() {
        tphrase::Generator ph{R"(
            main = {:= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Copy Constructor#1", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )"};
        ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2{ph1};
        ph1.equalize_chance();
        tphrase::Generator::set_random_function(get_default_random_func());
        const bool good1 = check_distribution(ph1, 100000, {
                { "X", 1.0 / (2.0 * 9.0) },
                { "Y", 1.0 / (2.0 * 9.0) },
                { "Z", 1.0 / (2.0 * 9.0) },
                { "A1", 1.0 / (2.0 * 9.0) },
                { "A2", 1.0 / (2.0 * 9.0) },
                { "A3", 1.0 / (2.0 * 9.0) },
                { "B1", 1.0 / (2.0 * 9.0) },
                { "B2", 1.0 / (2.0 * 9.0) },
                { "B3", 1.0 / (2.0 * 9.0) },
                { "V", 1.0 / (2.0 * 5.0) },
                { "W", 1.0 / (2.0 * 5.0) },
                { "C1", 1.0 / (2.0 * 5.0) },
                { "C2", 1.0 / (2.0 * 5.0) },
                { "C3", 1.0 / (2.0 * 5.0) },
            }, 0.01);
        const bool good2 = check_distribution(ph2, 100000, {
                { "X", 1.0 / 14.0 },
                { "Y", 1.0 / 14.0 },
                { "Z", 1.0 / 14.0 },
                { "A1", 1.0 / 14.0 },
                { "A2", 1.0 / 14.0 },
                { "A3", 1.0 / 14.0 },
                { "B1", 1.0 / 14.0 },
                { "B2", 1.0 / 14.0 },
                { "B3", 1.0 / 14.0 },
                { "V", 1.0 / 14.0 },
                { "W", 1.0 / 14.0 },
                { "C1", 1.0 / 14.0 },
                { "C2", 1.0 / 14.0 },
                { "C3", 1.0 / 14.0 },
            }, 0.01);

        return good1 && good2
            && ph1.get_error_message().empty()
            && ph2.get_error_message().empty()
            && ph1.get_number_of_syntax() == 2
            && ph2.get_number_of_syntax() == 2
            && ph1.get_weight() == 14
            && ph2.get_weight() == 14
            && ph1.get_combination_number() == 14
            && ph2.get_combination_number() == 14;
    });

    ut.set_test("Copy Constructor#2", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3 |
        )");
        tphrase::Generator ph2{ph1};
        return ph1.get_error_message().size() == 2
            && ph1.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph1.get_error_message()[1].find("A text is expected.") != std::string::npos
            && ph2.get_error_message().size() == 2
            && ph2.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message()[1].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Copy Constructor (ID is not changed)", [&]() {
        tphrase::Generator ph1;
        const auto id1 = ph1.add(R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )");
        const auto id2 = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2{ph1};

        PhraseNumber_t n12{ph1};
        ph1.remove(id1);
        PhraseNumber_t n11{ph1};
        ph1.remove(id2);
        PhraseNumber_t n10{ph1};

        PhraseNumber_t n22{ph2};
        ph2.remove(id1);
        PhraseNumber_t n21{ph2};
        ph2.remove(id2);
        PhraseNumber_t n20{ph2};
        return id1 && id2
            && n12 == n22
            && n11 == n21
            && n10 == n20
            && n22 == PhraseNumber_t{2, 14, 14}
            && n21 == PhraseNumber_t{1, 5, 5}
            && n20 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Move Constructor#1", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )"};
        ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2{std::move(ph1)};
        tphrase::Generator::set_random_function(get_default_random_func());
        const bool good2 = check_distribution(ph2, 100000, {
                { "X", 1.0 / 14.0 },
                { "Y", 1.0 / 14.0 },
                { "Z", 1.0 / 14.0 },
                { "A1", 1.0 / 14.0 },
                { "A2", 1.0 / 14.0 },
                { "A3", 1.0 / 14.0 },
                { "B1", 1.0 / 14.0 },
                { "B2", 1.0 / 14.0 },
                { "B3", 1.0 / 14.0 },
                { "V", 1.0 / 14.0 },
                { "W", 1.0 / 14.0 },
                { "C1", 1.0 / 14.0 },
                { "C2", 1.0 / 14.0 },
                { "C3", 1.0 / 14.0 },
            }, 0.01);

        return good2
            && ph2.get_error_message().empty()
            && ph2.get_number_of_syntax() == 2
            && ph2.get_weight() == 14
            && ph2.get_combination_number() == 14;
    });

    ut.set_test("Move Constructor#2", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3 |
        )");
        tphrase::Generator ph2{std::move(ph1)};
        return ph2.get_error_message().size() == 2
            && ph2.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message()[1].find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Move Constructor (ID is not changed)", [&]() {
        tphrase::Generator ph1;
        const auto id1 = ph1.add(R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )");
        const auto id2 = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        PhraseNumber_t n12{ph1};
        tphrase::Generator ph2{std::move(ph1)};

        PhraseNumber_t n22{ph2};
        ph2.remove(id1);
        PhraseNumber_t n21{ph2};
        ph2.remove(id2);
        PhraseNumber_t n20{ph2};
        return id1 && id2
            && n12 == n22
            && n22 == PhraseNumber_t{2, 14, 14}
            && n21 == PhraseNumber_t{1, 5, 5}
            && n20 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Copy Assignment#1", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )"};
        const bool add_result = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2;
        ph2 = ph1;
        ph1.equalize_chance();
        tphrase::Generator::set_random_function(get_default_random_func());
        const bool good1 = check_distribution(ph1, 100000, {
                { "X", 1.0 / (2.0 * 9.0) },
                { "Y", 1.0 / (2.0 * 9.0) },
                { "Z", 1.0 / (2.0 * 9.0) },
                { "A1", 1.0 / (2.0 * 9.0) },
                { "A2", 1.0 / (2.0 * 9.0) },
                { "A3", 1.0 / (2.0 * 9.0) },
                { "B1", 1.0 / (2.0 * 9.0) },
                { "B2", 1.0 / (2.0 * 9.0) },
                { "B3", 1.0 / (2.0 * 9.0) },
                { "V", 1.0 / (2.0 * 5.0) },
                { "W", 1.0 / (2.0 * 5.0) },
                { "C1", 1.0 / (2.0 * 5.0) },
                { "C2", 1.0 / (2.0 * 5.0) },
                { "C3", 1.0 / (2.0 * 5.0) },
            }, 0.01);
        const bool good2 = check_distribution(ph2, 100000, {
                { "X", 1.0 / 14.0 },
                { "Y", 1.0 / 14.0 },
                { "Z", 1.0 / 14.0 },
                { "A1", 1.0 / 14.0 },
                { "A2", 1.0 / 14.0 },
                { "A3", 1.0 / 14.0 },
                { "B1", 1.0 / 14.0 },
                { "B2", 1.0 / 14.0 },
                { "B3", 1.0 / 14.0 },
                { "V", 1.0 / 14.0 },
                { "W", 1.0 / 14.0 },
                { "C1", 1.0 / 14.0 },
                { "C2", 1.0 / 14.0 },
                { "C3", 1.0 / 14.0 },
            }, 0.01);

        return good1 && good2
            && ph1.get_error_message().empty()
            && ph2.get_error_message().empty()
            && ph1.get_number_of_syntax() == 2
            && ph2.get_number_of_syntax() == 2
            && ph1.get_weight() == 14
            && ph2.get_weight() == 14
            && ph1.get_combination_number() == 14
            && ph2.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Copy Assignment#2", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        const bool add_result = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3 |
        )");
        tphrase::Generator ph2;
        ph2 = ph1;
        return ph1.get_error_message().size() == 2
            && ph1.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph1.get_error_message()[1].find("A text is expected.") != std::string::npos
            && ph2.get_error_message().size() == 2
            && ph2.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message()[1].find("A text is expected.") != std::string::npos
            && !add_result;
    });

    ut.set_test("Copy Assignment (ID is not changed)", [&]() {
        tphrase::Generator ph1;
        const auto id1 = ph1.add(R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )");
        const auto id2 = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2;
        ph2 = ph1;

        PhraseNumber_t n12{ph1};
        ph1.remove(id1);
        PhraseNumber_t n11{ph1};
        ph1.remove(id2);
        PhraseNumber_t n10{ph1};

        PhraseNumber_t n22{ph2};
        ph2.remove(id1);
        PhraseNumber_t n21{ph2};
        ph2.remove(id2);
        PhraseNumber_t n20{ph2};
        return id1 && id2
            && n12 == n22
            && n11 == n21
            && n10 == n20
            && n22 == PhraseNumber_t{2, 14, 14}
            && n21 == PhraseNumber_t{1, 5, 5}
            && n20 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Move Assignment#1", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )"};
        const bool add_result = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        tphrase::Generator ph2;
        ph2 = std::move(ph1);
        tphrase::Generator::set_random_function(get_default_random_func());
        const bool good2 = check_distribution(ph2, 100000, {
                { "X", 1.0 / 14.0 },
                { "Y", 1.0 / 14.0 },
                { "Z", 1.0 / 14.0 },
                { "A1", 1.0 / 14.0 },
                { "A2", 1.0 / 14.0 },
                { "A3", 1.0 / 14.0 },
                { "B1", 1.0 / 14.0 },
                { "B2", 1.0 / 14.0 },
                { "B3", 1.0 / 14.0 },
                { "V", 1.0 / 14.0 },
                { "W", 1.0 / 14.0 },
                { "C1", 1.0 / 14.0 },
                { "C2", 1.0 / 14.0 },
                { "C3", 1.0 / 14.0 },
            }, 0.01);

        return good2
            && ph2.get_error_message().empty()
            && ph2.get_number_of_syntax() == 2
            && ph2.get_weight() == 14
            && ph2.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Move Assignment#2", [&]() {
        tphrase::Generator ph1{R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        const bool add_result = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3 |
        )");
        tphrase::Generator ph2;
        ph2 = std::move(ph1);
        return ph2.get_error_message().size() == 2
            && ph2.get_error_message()[0].find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message()[1].find("A text is expected.") != std::string::npos
            && !add_result;
    });

    ut.set_test("Move Assignment (ID is not changed)", [&]() {
        tphrase::Generator ph1;
        const auto id1 = ph1.add(R"(
            main = {= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3

            B = B1 | B2 | B3
        )");
        const auto id2 = ph1.add(R"(
            main = {= V | W } | {C}

            C = C1 | C2 | C3
        )");
        PhraseNumber_t n12{ph1};
        tphrase::Generator ph2;
        ph2 = std::move(ph1);

        PhraseNumber_t n22{ph2};
        ph2.remove(id1);
        PhraseNumber_t n21{ph2};
        ph2.remove(id2);
        PhraseNumber_t n20{ph2};
        return id1 && id2
            && n12 == n22
            && n22 == PhraseNumber_t{2, 14, 14}
            && n21 == PhraseNumber_t{1, 5, 5}
            && n20 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("generate with no external context", [&]() {
        tphrase::Generator ph{R"(
            main = {= {X} | {Y} | {Z} }
        )"};
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 3
            && ph.get_combination_number() == 3;
    });

    ut.set_test("generate with an external context", [&]() {
        tphrase::Generator ph{R"(
            main = {= {X} | {Y} | {Z} }
        )"};
        auto r = ph.generate({ { "X", "x" }, { "Y", "y" }, { "Z", "z" } });
        return r == "x"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 3
            && ph.get_combination_number() == 3;
    });

    ut.set_test("Add syntax (copy)", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        const bool add_result = ph.add(syntax);
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Add syntax (copy) with error#1", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3 |
        )"};
        const bool add_result = ph.add(syntax);
        auto r = ph.generate();
        return r == "X"
            && syntax.get_error_message().size() == 1
            && syntax.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
    });

    ut.set_test("Add syntax (copy) with error#2 and not same ID", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        syntax.add(R"(
            C = C4
        )");
        const auto id1 = ph.add(syntax);
        const auto err_msg{ph.get_error_message()};
        syntax.clear_error_message();
        const auto id2 = ph.add(syntax);
        const auto id3 = ph.add(syntax);
        return !id1 && id2 && id3
            && id2 != id3
            && err_msg.size() == 1
            && err_msg[0] == "The nonterminal \"C\" is already defined."
            && PhraseNumber_t{ph} == PhraseNumber_t{3, 15, 15};
    });

    ut.set_test("Add syntax (move)", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        const bool add_result = ph.add(std::move(syntax));
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Add syntax (move) with error#1", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3 |
        )"};
        const bool add_result = ph.add(std::move(syntax));
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
    });

    ut.set_test("Add syntax (move) with error#2 and not same ID", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        syntax.add(R"(
            C = C4
        )");
        tphrase::Syntax tmp1{syntax};
        const auto id1 = ph.add(std::move(tmp1));
        const auto err_msg{ph.get_error_message()};
        syntax.clear_error_message();
        tphrase::Syntax tmp2{syntax};
        const auto id2 = ph.add(std::move(tmp2));
        tphrase::Syntax tmp3{syntax};
        const auto id3 = ph.add(std::move(tmp3));
        return !id1 && id2 && id3
            && id2 != id3
            && err_msg.size() == 1
            && err_msg[0] == "The nonterminal \"C\" is already defined."
            && PhraseNumber_t{ph} == PhraseNumber_t{3, 15, 15};
    });

    ut.set_test("Add syntax (move) with start condition", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main2 = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        const bool add_result = ph.add(syntax, "main2");
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Add syntax (copy) with start condition and error#1", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main2 = {= V | W } | {C}
            C = C1 | C2 | C3 |
        )"};
        const bool add_result = ph.add(syntax, "main2");
        auto r = ph.generate();
        return r == "X"
            && syntax.get_error_message().size() == 1
            && syntax.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
    });

    ut.set_test("Add syntax (copy) with start condition, error#2, and not same ID", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main2 = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        syntax.add(R"(
            C = C4
        )");
        const auto id1 = ph.add(syntax, "main2");
        const auto err_msg{ph.get_error_message()};
        syntax.clear_error_message();
        const auto id2 = ph.add(syntax, "main2");
        const auto id3 = ph.add(syntax, "main2");
        return !id1 && id2 && id3
            && id2 != id3
            && err_msg.size() == 1
            && err_msg[0] == "The nonterminal \"C\" is already defined."
            && PhraseNumber_t{ph} == PhraseNumber_t{3, 15, 15};
    });

    ut.set_test("Add syntax (move) with start condition", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main3 = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        const bool add_result = ph.add(std::move(syntax), "main3");
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14
            && add_result;
    });

    ut.set_test("Add syntax (move) with start condition and error#1", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main2 = {= V | W } | {C}
            C = C1 | C2 | C3 |
        )"};
        const bool add_result = ph.add(std::move(syntax), "main2");
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().size() == 1
            && ph.get_error_message()[0].find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
    });

    ut.set_test("Add syntax (move) with start condition, error#2, and not same ID", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        tphrase::Syntax syntax{R"(
            main2 = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        syntax.add(R"(
            C = C4
        )");
        tphrase::Syntax tmp1{syntax};
        const auto id1 = ph.add(std::move(tmp1), "main2");
        const auto err_msg{ph.get_error_message()};
        syntax.clear_error_message();
        tphrase::Syntax tmp2{syntax};
        const auto id2 = ph.add(std::move(tmp2), "main2");
        tphrase::Syntax tmp3{syntax};
        const auto id3 = ph.add(std::move(tmp3), "main2");
        return !id1 && id2 && id3
            && id2 != id3
            && err_msg.size() == 1
            && err_msg[0] == "The nonterminal \"C\" is already defined."
            && PhraseNumber_t{ph} == PhraseNumber_t{3, 15, 15};
    });

    ut.set_test("Add via R-Value Syntax (a pair of input iterators)#1", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        std::istringstream s{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        ph.add({std::istreambuf_iterator<char>{s},
                std::istreambuf_iterator<char>{}});
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14;
    });

    ut.set_test("Add via R-Value Syntax (a pair of input iterators)#2", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        std::istringstream s{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        s.unsetf(std::ios_base::skipws);
        ph.add({std::istream_iterator<char>{s},
                std::istream_iterator<char>{}});
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14;
    });

    ut.set_test("Add via R-Value Syntax (a std::string)", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        std::string s{R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )"};
        ph.add(s);
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14;
    });

    ut.set_test("Add via R-Value Syntax (a const char*)", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        ph.add(R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )");
        auto r = ph.generate();
        return r == "X"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 2
            && ph.get_weight() == 14
            && ph.get_combination_number() == 14;
    });

    ut.set_test("Remove phrase first", [&]() {
        tphrase::Generator::set_random_function(get_sequence_random_func({
            0.9, 0.9, 0.9, 0.9, 0.9, 0.9
        }));
        tphrase::Generator ph;
        const auto id1 = ph.add(R"(main = "1" 2 | 2 | 3)");
        const auto id2 = ph.add(R"(main = A | "B" 3 | C)");
        const auto id3 = ph.add(R"(main = あ | い | "う" 4)");
        const auto r3 = ph.generate();
        const PhraseNumber_t n3{ph};
        const auto del3 = ph.remove(id1);
        const auto del32 = ph.remove(id1);
        const auto r2 = ph.generate();
        const PhraseNumber_t n2{ph};
        const auto del2 = ph.remove(id2);
        const auto del22 = ph.remove(id2);
        const auto r1 = ph.generate();
        const PhraseNumber_t n1{ph};
        const auto del1 = ph.remove(id3);
        const auto del12 = ph.remove(id3);
        const auto r0 = ph.generate();
        const PhraseNumber_t n0{ph};
        return del1 && del2 && del3
            && !del32 && !del22 && !del12
            && id1 && id2 && id3
            && r3 == "う" && r2 == "う" && r1 == "う" && r0 == "nil"
            && n3 == PhraseNumber_t{3, 9, 15}
            && n2 == PhraseNumber_t{2, 6, 11}
            && n1 == PhraseNumber_t{1, 3, 6}
            && n0 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Remove phrase last", [&]() {
        tphrase::Generator::set_random_function(get_sequence_random_func({
            0.9, 0.9, 0.9, 0.9, 0.9, 0.9
        }));
        tphrase::Generator ph;
        const auto id1 = ph.add(R"(main = "1" 2 | 2 | 3)");
        const auto id2 = ph.add(R"(main = A | "B" 3 | C)");
        const auto id3 = ph.add(R"(main = あ | い | "う" 4)");
        const auto r3 = ph.generate();
        const PhraseNumber_t n3{ph};
        const auto del3 = ph.remove(id3);
        const auto del32 = ph.remove(id3);
        const auto r2 = ph.generate();
        const PhraseNumber_t n2{ph};
        const auto del2 = ph.remove(id2);
        const auto del22 = ph.remove(id2);
        const auto r1 = ph.generate();
        const PhraseNumber_t n1{ph};
        const auto del1 = ph.remove(id1);
        const auto del12 = ph.remove(id1);
        const auto r0 = ph.generate();
        const PhraseNumber_t n0{ph};
        return del1 && del2 && del3
            && !del32 && !del22 && !del12
            && id1 && id2 && id3
            && r3 == "う" && r2 == "C" && r1 == "3" && r0 == "nil"
            && n3 == PhraseNumber_t{3, 9, 15}
            && n2 == PhraseNumber_t{2, 6, 9}
            && n1 == PhraseNumber_t{1, 3, 4}
            && n0 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Remove phrase middle", [&]() {
        tphrase::Generator::set_random_function(get_sequence_random_func({
            0.9, 0.9, 0.9, 0.9, 0.9, 0.9
        }));
        tphrase::Generator ph;
        const auto id1 = ph.add(R"(main = "1" 2 | 2 | 3)");
        const auto id2 = ph.add(R"(main = A | "B" 3 | C)");
        const auto id3 = ph.add(R"(main = あ | い | "う" 4)");
        const auto r3 = ph.generate();
        const PhraseNumber_t n3{ph};
        const auto del3 = ph.remove(id2);
        const auto del32 = ph.remove(id2);
        const auto r2 = ph.generate();
        const PhraseNumber_t n2{ph};
        const auto del2 = ph.remove(id1);
        const auto del22 = ph.remove(id1);
        const auto r1 = ph.generate();
        const PhraseNumber_t n1{ph};
        const auto del1 = ph.remove(id3);
        const auto del12 = ph.remove(id3);
        const auto r0 = ph.generate();
        const PhraseNumber_t n0{ph};
        return del1 && del2 && del3
            && !del32 && !del22 && !del12
            && id1 && id2 && id3
            && r3 == "う" && r2 == "う" && r1 == "う" && r0 == "nil"
            && n3 == PhraseNumber_t{3, 9, 15}
            && n2 == PhraseNumber_t{2, 6, 10}
            && n1 == PhraseNumber_t{1, 3, 6}
            && n0 == PhraseNumber_t{0, 0, 0};
    });

    ut.set_test("Remove and add phrase", [&]() {
        tphrase::Generator::set_random_function(get_sequence_random_func({
            0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9
        }));
        tphrase::Generator ph;
        const auto id1 = ph.add(R"(main = 1)");
        const auto id2 = ph.add(R"(main = A | B)");
        const auto id3 = ph.add(R"(main = あ | い | う)");
        const auto r3{ph.generate()};
        const PhraseNumber_t n3{ph};
        const auto del2 = ph.remove(id2);
        const auto r3d2{ph.generate()};
        const PhraseNumber_t n3d2{ph};
        const auto id4 = ph.add(R"(main = 11 | 12 | 13 | 14)");
        const auto r4{ph.generate()};
        const PhraseNumber_t n4{ph};
        const auto del4 = ph.remove(id4);
        const auto r4d4{ph.generate()};
        const PhraseNumber_t n4d4{ph};
        const auto id5 = ph.add(R"(main = AA | BB | CC | DD | EE)");
        const auto r5{ph.generate()};
        const PhraseNumber_t n5{ph};
        return del2 && del4
            && id1 && id2 && id3 && id4 && id5
            && id1 != id2 && id1 != id3 && id1 != id4 && id1 != id5
            && id2 != id3
            && id3 != id4 && id3 != id5
            && r3 == "う" && r3d2 == "う"
            && r4 == "14" && r4d4 == "う"
            && r5 == "EE"
            && n3 == PhraseNumber_t{3, 6, 6}
            && n3d2 == PhraseNumber_t{2, 4, 4}
            && n4 == PhraseNumber_t{3, 8, 8}
            && n4d4 == PhraseNumber_t{2, 4, 4}
            && n5 == PhraseNumber_t{3, 9, 9};
    });

    ut.set_test("Get and clear error message", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B} |

            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        std::vector<std::string> err_msg{ph.get_error_message()};
        ph.clear_error_message();
        auto r = ph.generate();
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 0
            && ph.get_weight() == 0
            && ph.get_combination_number() == 0;
    });

    ut.set_test("Clear", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B}
            A = A1 | A2 | A3 |

            B = B1 | B2 | B3
        )"};
        std::vector<std::string> err_msg{ph.get_error_message()};
        ph.clear();
        ph.add(R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )");
        auto r = ph.generate();
        return r == "V"
            && err_msg.size() == 1
            && err_msg[0].find("A text is expected.") != std::string::npos
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 5
            && ph.get_combination_number() == 5;
    });

    ut.set_test("Set Gsub creator", [&]() {
        tphrase::Generator::set_gsub_function_creator([](const std::string &,
                                                         const std::string &,
                                                         bool) {
            return [](const std::string&) { return std::string{"123"}; };
        });
        tphrase::Generator ph{R"(
            main = abc ~ /x/y/
        )"};
        auto r = ph.generate();
        return r == "123"
            && ph.get_error_message().empty()
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 1
            && ph.get_combination_number() == 1;
    });

    return ut.run();
}
