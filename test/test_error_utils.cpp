/* test for error_utils

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
#include <string>

#include "tphrase/error_utils.h"
#include "tphrase/common/InputIterator.h"

#include "UnitTest.h"

std::size_t test_error_utils()
{
    UnitTest ut("error_utils");

    ut.set_enter_function([&]() {
    });
    ut.set_leave_function([&]() {
        return true;
    });

    ut.set_test("trunc_syntax std::string", [&]() {
        auto r{tphrase::trunc_syntax(std::string{"main = \"0123456789\""})};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("trunc_syntax const char*", [&]() {
        auto r{tphrase::trunc_syntax("main = \"0123456789\"")};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("trunc_syntax input iterator's pair", [&]() {
        std::istringstream s{"main = \"0123456789\""};
        s.unsetf(std::ios_base::skipws);
        auto r{tphrase::trunc_syntax(std::istream_iterator<char>{s},
                                     std::istream_iterator<char>{})};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("trunc_syntax InputIterator", [&]() {
        std::istringstream s{"main = \"0123456789\""};
        s.unsetf(std::ios_base::skipws);
        tphrase::InputIterator<std::istream_iterator<char>,
                               std::istream_iterator<char>>
            it{std::istream_iterator<char>{s},
               std::istream_iterator<char>{}};
        auto r{tphrase::trunc_syntax(it)};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("trunc_syntax std::string with min_len", [&]() {
        auto r{tphrase::trunc_syntax(std::string{R"(
            main = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
        )"}, 30)};
        return r == "main = 0 | 1 | 2 | 3 | 4 | 5 |...";
    });

    ut.set_test("trunc_syntax const char* with min_len", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            main = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
        )", 30)};
        return r == "main = 0 | 1 | 2 | 3 | 4 | 5 |...";
    });

    ut.set_test("trunc_syntax input iterator's pair with min_len", [&]() {
        std::istringstream s{R"(
            main = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
        )"};
        s.unsetf(std::ios_base::skipws);
        auto r{tphrase::trunc_syntax(std::istream_iterator<char>{s},
                                     std::istream_iterator<char>{}, 15)};
        return r == "main = 0 | 1 |...";
    });

    ut.set_test("trunc_syntax InputIterator with min_len", [&]() {
        std::istringstream s{R"(
            main = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
        )"};
        s.unsetf(std::ios_base::skipws);
        tphrase::InputIterator<std::istream_iterator<char>,
                               std::istream_iterator<char>>
            it{std::istream_iterator<char>{s},
               std::istream_iterator<char>{}};
        auto r{tphrase::trunc_syntax(it, 25)};
        return r == "main = 0 | 1 | 2 | 3 | 4...";
    });

    ut.set_test("truncate preceding spaces", [&]() {
        auto r{tphrase::trunc_syntax(R"(
	        main = "0123456789")")};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("truncate succeeding spaces", [&]() {
        auto r{tphrase::trunc_syntax(R"(main = "0123456789" 	)")};
        return r == "main = \"0123456789\"";
    });

    ut.set_test("truncate character#1", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||)")};
        return r == "||||||||||||||||||||||||||||||||||||||||...";
    });

    ut.set_test("truncate character#2", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~)")};
        return r == "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~...";
    });

    ut.set_test("truncate character#3", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            =================================================================)")};
        return r == "========================================...";
    });

    ut.set_test("truncate character#4", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a)")};
        return r == "a a a a a a a a a a a a a a a a a a a a...";
    });

    ut.set_test("truncate character#5", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a	a)")};
        return r == "a\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta\ta...";
    });

    ut.set_test("not truncate character#1", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            -----------------------------------------------------------------)")};
        return r == "-----------------------------------------------------------------";
    });

    ut.set_test("not truncate character#2", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            jugemujugemugokounosurikirekaijarisuigyonosuigyoumatuunraimatuhuuraimatu)")};
        return r == "jugemujugemugokounosurikirekaijarisuigyonosuigyoumatuunraimatuhuuraimatu";
    });

    ut.set_test("truncate return", [&]() {
        auto r{tphrase::trunc_syntax(R"(
            1 2 3
)")};
        return r == "1 2 3...";
    });

    return ut.run();
}
