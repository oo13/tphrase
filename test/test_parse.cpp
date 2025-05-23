/* test for parse function

   Copyright © 2024 OOTA, Masato

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

   OR

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use TPhrase except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <cstddef>
#include <string>

#include "tphrase/Generator.h"

#include "UnitTest.h"
#include "unit_test_utility.h"

std::size_t test_parse()
{
    UnitTest ut("parse");

    auto stub_random{get_sequence_random_func({})};

    ut.set_enter_function([&]() {
        tphrase::Generator::set_random_function(stub_random);
    });
    ut.set_leave_function([&]() {
        return true;
    });


    ut.set_test("Hello World.", [&]() {
        tphrase::Generator ph;
        ph.add("main=Hello World.");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Spaces before equal", [&]() {
        tphrase::Generator ph;
        ph.add("main =Hello World.");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Spaces after equal", [&]() {
        tphrase::Generator ph;
        ph.add("main= Hello World.");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Newline after equal", [&]() {
        tphrase::Generator ph;
        ph.add(R"(main=
Hello World.)");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Assignment equal chance", [&]() {
        tphrase::Generator ph;
        ph.add("main := Hello World.");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Assignment trailing spaces", [&]() {
        tphrase::Generator ph;
        ph.add(R"(main = Hello World.    {* --}
        {* --- }
 )");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Assignment after spaces", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
        {* --- }


         main = Hello World.)");
        const auto r = ph.generate();
        return r == "Hello World."
            && ph.get_error_message().empty();
    });

    ut.set_test("Assignments top down", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = {sub}
            sub = A
        )");
        const auto r = ph.generate();
        return r == "A"
            && ph.get_error_message().empty();
    });

    ut.set_test("Assignments bottom up", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            sub = A
            main = {sub}
        )");
        const auto r = ph.generate();
        return r == "A"
            && ph.get_error_message().empty();
    });

    ut.set_test("Spaces", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            {* comment } main 	{* comment } =  	{* comment }
                {* comment } text1 	{* comment } | 	{* comment }
                {* comment } "text2" 	{* comment } ~  	{* comment }
                {* comment } /A/Z/ 	{* comment }
                {* comment } 
                {* comment } sub 	{* comment } :=  	{* comment }
                {* comment } 'text3' 	{* comment } | 	{* comment }
                {* comment } `text4` 	{* comment }
        )");
        const auto r = ph.generate();
        return r == "text1"
            && ph.get_error_message().empty();
    });

    ut.set_test("Production Rule Simple", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | text2 | text3 ~ /pat1/repl1/ ~ /pat2/repl2/g
        )");
        const auto r = ph.generate();
        return r == "text1"
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Quoted", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | "text2" 2
            sub = 'text1' 2 | `text2`
        )");
        const auto r = ph.generate();
        return r == "text1" && ph.get_weight() == 3
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Quoted with Real Number#1", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | "text2" 2.1
        )");
        const auto r = ph.generate();
        return r == "text1" && ph.get_weight() == 3.1
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Quoted with Real Number#2", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | "text2" .32
        )");
        const auto r = ph.generate();
        return r == "text1" && ph.get_weight() == 1.32
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Quoted with All Decimals", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | "text2" 12345678901.
        )");
        const auto r = ph.generate();
        return r == "text1" && ph.get_weight() == 12345678902
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Quoted with Number Decimal Only", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = text1 | "text2" .
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("A number is expected. (\".\" is not a number.)") != std::string::npos;
    });

    ut.set_test("Text Non-quoted", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 	text1 	|  
                te|xt2
        )");
        const auto r = ph.generate();
        return r == "text1" && ph.get_combination_number() == 3
            && ph.get_error_message().empty();
    });

    ut.set_test("Text Empty", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 	'' | "" | `` | {} | '' | {*
            comment }"" |
                '{* comment }' |
        ``)");
        const auto r = ph.generate();
        return r == "" && ph.get_combination_number() == 8
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Prior Rule", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "  {"
{'`|~ 	}  "
        )");
        const auto r = ph.generate();
        return r == "  \"\n{'`|~ \t  "
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Nonterminal#1", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{AAA}+="
            AAA = ZZZ
        )");
        const auto r = ph.generate();
        return r == "-+ZZZ+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Nonterminal#2", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{1}+="
            1 = ZZZ
        )");
        const auto r = ph.generate();
        return r == "-+ZZZ+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Nonterminal#3", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{_}+="
            _ = ZZZ
        )");
        const auto r = ph.generate();
        return r == "-+ZZZ+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Braces", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{(}+={)}|-"
        )");
        const auto r = ph.generate();
        return r == "-+{+=}|-"
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Comment", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{*comment}+="
            comment = ZZZ
        )");
        const auto r = ph.generate();
        return r == "-++="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Anonymous Rule#1", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{= A | B | C }+="
        )");
        const auto r = ph.generate();
        return r == "-+A+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Anonymous Rule#2", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{:=1|2|3~/1/9/~|2|8|}+="
        )");
        const auto r = ph.generate();
        return r == "-+9+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Anonymous Rule#3", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{=
               A | B | C
            }+="
        )");
        const auto r = ph.generate();
        return r == "-+A+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion Unsolved", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = "-+{AAA}+="
        )");
        const auto r = ph.generate();
        return r == "-+AAA+="
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub Simple", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 1 | 2 | 3~/A/C/g
        )");
        const auto r = ph.generate();
        return r == "1"
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub Separator", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 1 | 2 | 3~|A|C|g~/B/D/ ~ "C"")");
        const auto r = ph.generate();
        return r == "1"
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub with Character except g", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 1 | 2 | 3 ~ ~A~B~1)");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The end of the text or \"\\n\" is expected.") != std::string::npos;
    });

    ut.set_test("Gsub separator error", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 1 | 2 | 3 ~ あAあBあ)");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The separator must be a 7 bit character.") != std::string::npos;
    });

    ut.set_test("Parse Error in the Last Line", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = 1 | 2 | 3 ~ /A//+)");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The end of the text or \"\\n\" is expected.") != std::string::npos;
    });

    ut.set_test("Recursive Expansion Error", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = {A}
            A = {B}
            B = {C}
            C = {B}
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("Recursive expansion of \"B\" is detected.") != std::string::npos;
    });

    ut.set_test("No Local Nonterminal Error", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = {A}
            A = {_B}
            B = C
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The local nonterminal \"_B\" is not found.") != std::string::npos;
    });

    ut.set_test("Nonterminal with weight#1", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main 10 = A | B | C
        )");
        const auto r = ph.generate();
        return r == "A"
            && ph.get_weight() == 10
            && ph.get_error_message().empty();
    });

    ut.set_test("Nonterminal with weight#2", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main 10.5= A | B | C
        )");
        const auto r = ph.generate();
        return r == "A"
            && ph.get_weight() == 10.5
            && ph.get_error_message().empty();
    });

    ut.set_test("Nonterminal characters", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = {0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.}
            0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_. = 9
        )");
        const auto r = ph.generate();
        return r == "9"
            && ph.get_error_message().empty();
    });

    ut.set_test("Redefined nonterminal error", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = {A}
            A = 1 | 2 | 3
            A = 4 | 5 | 6
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The nonterminal \"A\" is already defined.") != std::string::npos;
    });

    ut.set_test("unclosed comment 1", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            {*
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The end of the comment is expected.") != std::string::npos;
    });

    ut.set_test("unclosed comment 2", [&]() {
        tphrase::Generator ph;
        ph.add(R"(
            main = A
            {*
        )");
        const auto r = ph.generate();
        const std::vector<std::string> err_msg{ph.get_error_message()};
        return r == "nil"
            && err_msg.size() == 1
            && err_msg[0].find("The end of the comment is expected.") != std::string::npos;
    });

    return ut.run();
}
