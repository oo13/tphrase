/* test for class Syntax

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

std::size_t test_class_Syntax()
{
    UnitTest ut("class Syntax");

    auto stub_random{get_sequence_random_func({})};

    ut.set_enter_function([&]() {
        tphrase::Generator::set_random_function(stub_random);
    });
    ut.set_leave_function([&]() {
        return true;
    });

    ut.set_test("Default Constructor", [&]() {
        tphrase::Syntax syntax;
        return syntax.get_error_message() == "";
    });

    ut.set_test("Constructor with a pair of input iterators", [&]() {
        std::istringstream s{"main = a |"};
        s.unsetf(std::ios_base::skipws);
        tphrase::Syntax syntax{std::istream_iterator<char>{s},
                               std::istream_iterator<char>{}};
        return syntax.get_error_message().find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Constructor with a std::string", [&]() {
        tphrase::Syntax syntax{std::string{"main = a | b ~"}};
        return syntax.get_error_message().find("Unexpected EOT.") != std::string::npos;
    });

    ut.set_test("Constructor with a const char *", [&]() {
        tphrase::Syntax syntax{"main = a | b ~///"};
        return syntax.get_error_message().find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Copy Constructor", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} ~///
            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{syntax1};
        return syntax1.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && syntax2.get_error_message().find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Move Constructor", [&]() {
        tphrase::Syntax syntax{tphrase::Syntax{R"(
            main = {= A | B | C } | {B} | {C} ~///
            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"}};
        return syntax.get_error_message().find("The non-empty pattern is expected.") != std::string::npos;
    });

    ut.set_test("Copy Assignment", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        syntax2 = syntax1;
        return syntax1.get_error_message().find("A text is expected.") != std::string::npos
            && syntax2.get_error_message().find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Move Assignment", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        syntax2 = std::move(syntax1);
        return syntax2.get_error_message().find("A text is expected.") != std::string::npos;
    });

    ut.set_test("Add Syntax (copy) without error", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(syntax2);
        tphrase::Generator ph{syntax1};
        auto r = ph.generate();
        return syntax1.get_error_message().find("A text is expected.") != std::string::npos
            && good
            && r == "X";
    });

    ut.set_test("Add Syntax (copy) with error", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(syntax2);
        return syntax1.get_error_message().find("A text is expected.") != std::string::npos
            && syntax1.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && !good;
    });

    ut.set_test("Add Syntax (copy) with overwriting", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C}

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(syntax2);
        tphrase::Generator ph{syntax1};
        auto r = ph.generate();
        return r == "X"
            && syntax1.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && ph.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && good;
    });

    ut.set_test("Add Syntax (move) without error", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(std::move(syntax2));
        tphrase::Generator ph{syntax1};
        auto r = ph.generate();
        return syntax1.get_error_message().find("A text is expected.") != std::string::npos
            && good
            && r == "X";
    });

    ut.set_test("Add Syntax (move)", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(std::move(syntax2));
        return syntax1.get_error_message().find("A text is expected.") != std::string::npos
            && syntax1.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && !good;
    });

    ut.set_test("Add Syntax (move) with overwriting", [&]() {
        tphrase::Syntax syntax1{R"(
            main = {= A | B | C } | {B} | {C}

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        tphrase::Syntax syntax2{R"(
            main = {= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax1.add(std::move(syntax2));
        tphrase::Generator ph{syntax1};
        auto r = ph.generate();
        return r == "X"
            && syntax1.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && ph.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && good;
    });

    ut.set_test("Add a pair of input iterators", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        std::istringstream s{R"(
            main = {:= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        s.unsetf(std::ios_base::skipws);
        const bool good = syntax.add(std::istream_iterator<char>{s},
                                     std::istream_iterator<char>{});
        return syntax.get_error_message().find("A text is expected.") != std::string::npos
            && syntax.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && !good;
    });

    ut.set_test("Add a pair of input iterators with overwriting", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C}

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        std::istringstream s{R"(
            main = {:= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        s.unsetf(std::ios_base::skipws);
        const bool good = syntax.add(std::istream_iterator<char>{s},
                                     std::istream_iterator<char>{});
        tphrase::Generator ph{syntax};
        auto r = ph.generate();
        return r == "X"
            && syntax.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && ph.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && good;
    });

    ut.set_test("Add a std::string", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        std::string s{R"(
            main = {:= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax.add(s);
        return syntax.get_error_message().find("A text is expected.") != std::string::npos
            && syntax.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && !good;
    });

    ut.set_test("Add a std::string with overwriting", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C}

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        std::string s{R"(
            main = {:= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )"};
        const bool good = syntax.add(s);
        tphrase::Generator ph{syntax};
        auto r = ph.generate();
        return r == "X"
            && syntax.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && ph.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && good;
    });

    ut.set_test("Add a const char*", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        const bool good = syntax.add(R"(
            main = {:= X | Y | Z } | {Y} | {Z} ~ ///

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )");
        return syntax.get_error_message().find("A text is expected.") != std::string::npos
            && syntax.get_error_message().find("The non-empty pattern is expected.") != std::string::npos
            && !good;
    });

    ut.set_test("Add a const char* with overwriting", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C}

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        const bool good = syntax.add(R"(
            main = {:= X | Y | Z } | {Y} | {Z}

            Y = Y1 | Y2 | Y3
            Z = Z1 | Z2 | Z3
        )");
        tphrase::Generator ph{syntax};
        auto r = ph.generate();
        return r == "X"
            && syntax.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && ph.get_error_message() == "The nonterminal \"main\" is already defined.\n"
            && good;
    });

    ut.set_test("clear_error", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        const bool found = syntax.get_error_message().find("A text is expected.") != std::string::npos;
        syntax.clear_error_message();
        const bool empty = syntax.get_error_message().empty();
        return found && empty;
    });

    ut.set_test("clear", [&]() {
        tphrase::Syntax syntax{R"(
            main = {:= A | B | C } | {B} | {C} |

            B = B1 | B2 | B3
            C = C1 | C2 | C3
        )"};
        const bool found = syntax.get_error_message().find("A text is expected.") != std::string::npos;
        syntax.clear();
        tphrase::Generator ph{syntax};
        auto r = ph.generate();
        const bool empty = syntax.get_error_message().empty();
        return found && empty
            && ph.get_error_message().find("The nonterminal \"main\" doesn't exist.\n") != std::string::npos;
    });

    return ut.run();
}
