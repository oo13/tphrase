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
            && syntax.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
            && syntax.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main2\" doesn't exist.\n") != std::string::npos;
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
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main3\" doesn't exist.\n") != std::string::npos;
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
            && ph.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
            && ph.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
            && ph.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
    });

    ut.set_test("Constructor via R-Value Syntax (a const char*)", [&]() {
        tphrase::Generator ph{R"(
            main = {:= X | Y | Z } | {A} | {B}

            A = A1 | A2 | A3 ~ ///

            B = B1 | B2 | B3
        )"};
        auto r = ph.generate();
        return r == "nil"
            && ph.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
        return ph1.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph1.get_error_message().find("A text is expected.") != std::string::npos
            && ph1.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos
            && ph2.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message().find("A text is expected.") != std::string::npos
            && ph2.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
        return ph2.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message().find("A text is expected.") != std::string::npos
            && ph2.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
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
        return ph1.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph1.get_error_message().find("A text is expected.") != std::string::npos
            && ph1.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos
            && ph2.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message().find("A text is expected.") != std::string::npos
            && ph2.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos
            && !add_result;
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
        return ph2.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && ph2.get_error_message().find("A text is expected.") != std::string::npos
            && ph2.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos
            && !add_result;
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

    ut.set_test("Add syntax (copy) wiht error", [&]() {
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
            && syntax.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
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

    ut.set_test("Add syntax (move) wiht error", [&]() {
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
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
    });

    ut.set_test("Add syntax (copy) with start condition", [&]() {
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

    ut.set_test("Add syntax (copy) with start condition and error", [&]() {
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
            && syntax.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
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

    ut.set_test("Add syntax (move) with start condition and error", [&]() {
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
            && ph.get_error_message().find("A text is expected.") != std::string::npos
            && ph.get_number_of_syntax() == 1
            && ph.get_weight() == 9
            && ph.get_combination_number() == 9
            && !add_result;
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

    ut.set_test("Get and clear error message", [&]() {
        tphrase::Generator ph{R"(
            main = {= X | Y | Z } | {A} | {B} |

            A = A1 | A2 | A3
            B = B1 | B2 | B3
        )"};
        std::string err_msg{ph.get_error_message()};
        ph.clear_error_message();
        auto r = ph.generate();
        return r == "nil"
            && err_msg.find("A text is expected.") != std::string::npos
            && err_msg.find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos
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
        std::string err_msg{ph.get_error_message()};
        ph.clear();
        ph.add(R"(
            main = {= V | W } | {C}
            C = C1 | C2 | C3
        )");
        auto r = ph.generate();
        return r == "V"
            && err_msg.find("A text is expected.") != std::string::npos
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
