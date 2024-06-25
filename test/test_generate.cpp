/* test for generate function

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

#include <cstddef>
#include <string>
#include <unordered_map>

#include "tphrase/Generator.h"

#include "UnitTest.h"
#include "unit_test_utility.h"

std::size_t test_generate()
{
    UnitTest ut("generate");

    auto stub_random{get_sequence_random_func({})};

    ut.set_enter_function([&]() {
        tphrase::Generator::set_random_function(stub_random);
    });
    ut.set_leave_function([&]() {
        return true;
    });

    auto set_random_linear{[](std::size_t num) {
        tphrase::Generator::set_random_function(
            get_sequence_random_func(get_linear_weight(num))
        );
    }};


    ut.set_test("No Options", [&]() {
        tphrase::Generator ph(R"(
            main = ""
        )");
        return ph.generate() == ""
            && ph.get_error_message().empty();
    });

    ut.set_test("No Weight Options", [&]() {
        tphrase::Generator ph(R"(
            main = A | B | C
        )");
        set_random_linear(3);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        return r1 == "A"
            && r2 == "B"
            && r3 == "C"
            && ph.get_error_message().empty();
    });

    ut.set_test("Weighted Options", [&]() {
        tphrase::Generator ph(R"(
            main = A | "B" 2 | "C" 3
        )");
        set_random_linear(6);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        const auto r4 = ph.generate();
        const auto r5 = ph.generate();
        const auto r6 = ph.generate();
        return r1 == "A"
            && r2 == "B"
            && r3 == "B"
            && r4 == "C"
            && r5 == "C"
            && r6 == "C"
            && ph.get_error_message().empty();
    });

    ut.set_test("Weighted and Equalized Options", [&]() {
        tphrase::Generator ph(R"(
            main := A | "B" 2 | "C" 3
        )");
        set_random_linear(6);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        const auto r4 = ph.generate();
        const auto r5 = ph.generate();
        const auto r6 = ph.generate();
        return r1 == "A"
            && r2 == "A"
            && r3 == "B"
            && r4 == "B"
            && r5 == "C"
            && r6 == "C"
            && ph.get_error_message().empty();
    });

    ut.set_test("Options Distribution", [&]() {
        tphrase::Generator ph(R"(
            main = {A1} | {A2}
            A1 = 0 | 1 | 2
            A2 = {A21} | {A22}
            A21 = 3 | 4
            A22 = 5 | 6 | 7 | 8 | 9
        )");
        tphrase::Generator::set_random_function(get_default_random_func());
        std::unordered_map<std::string, double> dist{
            { "0", 0.1 },
            { "1", 0.1 },
            { "2", 0.1 },
            { "3", 0.1 },
            { "4", 0.1 },
            { "5", 0.1 },
            { "6", 0.1 },
            { "7", 0.1 },
            { "8", 0.1 },
            { "9", 0.1 },
        };
        return check_distribution(ph, 100000, dist, 0.01)
            && ph.get_error_message().empty();
    });

    ut.set_test("Options Distribution Equalized", [&]() {
        tphrase::Generator ph(R"(
            main = {A1} | {A2}
            A1 = 0 | 1 | 2
            A2 := {A21} | {A22}
            A21 = 3 | 4
            A22 = 5 | 6 | 7 | 8 | 9
        )");
        tphrase::Generator::set_random_function(get_default_random_func());
        std::unordered_map<std::string, double> dist{
            { "0", 0.1 },
            { "1", 0.1 },
            { "2", 0.1 },
            { "3", 0.175 },
            { "4", 0.175 },
            { "5", 0.07 },
            { "6", 0.07 },
            { "7", 0.07 },
            { "8", 0.07 },
            { "9", 0.07 },
        };
        return check_distribution(ph, 100000, dist, 0.01)
            && ph.get_error_message().empty();
    });

    ut.set_test("Options Distribution Weighted", [&]() {
        tphrase::Generator ph(R"(
            main = text1 | {B}
            B = text2 | "{C}" 2
            C = 1 | 2 | 3
        )");
        tphrase::Generator::set_random_function(get_default_random_func());
        std::unordered_map<std::string, double> dist{
            { "text1", 0.25 },
            { "text2", 0.25 },
            { "1", 0.1667 },
            { "2", 0.1667 },
            { "3", 0.1667 },
        };
        return check_distribution(ph, 100000, dist, 0.01)
            && ph.get_error_message().empty();
    });

    ut.set_test("Options Distribution Many Items", [&]() {
        tphrase::Generator ph(R"(
            main =
            "00" 5 | "01" | "02" | "03" | "04" | "05" | "06" | "07" | "08" | "09" |
            "10" | "11" 3 | "12" | "13" | "14" | "15" | "16" | "17" | "18" | "19" |
            "20" | "21" | "22" 4 | "23" | "24" | "25" | "26" | "27" | "28" | "29" |
            "30" | "31" | "32" | "33" 2 | "34" | "35" | "36" | "37" | "38" | "39"
        )");
        tphrase::Generator::set_random_function(get_default_random_func());
        constexpr double d = 1.0 / 50.0;
        std::unordered_map<std::string, double> dist{
            {"00", 5*d}, {"01", d}, {"02", d}, {"03", d}, {"04", d},
            {"05", d}, {"06", d}, {"07", d}, {"08", d}, {"09", d},
            {"10", d}, {"11", 3*d}, {"12", d}, {"13", d}, {"14", d},
            {"15", d}, {"16", d}, {"17", d}, {"18", d}, {"19", d},
            {"20", d}, {"21", d}, {"22", 4*d}, {"23", d}, {"24", d},
            {"25", d}, {"26", d}, {"27", d}, {"28", d}, {"29", d},
            {"30", d}, {"31", d}, {"32", d}, {"33", 2*d}, {"34", d},
            {"35", d}, {"36", d}, {"37", d}, {"38", d}, {"39", d},
        };
        return check_distribution(ph, 100000, dist, 0.01)
            && ph.get_error_message().empty();
    });

    ut.set_test("Anonymous Rule", [&]() {
        tphrase::Generator ph(R"(
            main = 1{= A | B | C }2
        )");
        set_random_linear(3);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        return r1 == "1A2"
            && r2 == "1B2"
            && r3 == "1C2"
            && ph.get_error_message().empty();
    });

    ut.set_test("Anonymous Rule Weighted", [&]() {
        tphrase::Generator ph(R"(
            main = 1{= A | "B" 2 | "C" 3}2
        )");
        set_random_linear(6);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        const auto r4 = ph.generate();
        const auto r5 = ph.generate();
        const auto r6 = ph.generate();
        return r1 == "1A2"
            && r2 == "1B2"
            && r3 == "1B2"
            && r4 == "1C2"
            && r5 == "1C2"
            && r6 == "1C2"
            && ph.get_error_message().empty();
    });

    ut.set_test("Anonymous Rule Weighted and Equalized", [&]() {
        tphrase::Generator ph(R"(
            main = 1{:= A | "B" 2 | "C" 3}2
        )");
        set_random_linear(6);
        const auto r1 = ph.generate();
        const auto r2 = ph.generate();
        const auto r3 = ph.generate();
        const auto r4 = ph.generate();
        const auto r5 = ph.generate();
        const auto r6 = ph.generate();
        return r1 == "1A2"
            && r2 == "1A2"
            && r3 == "1B2"
            && r4 == "1B2"
            && r5 == "1C2"
            && r6 == "1C2"
            && ph.get_error_message().empty();
    });

    ut.set_test("Special Expansion", [&]() {
        tphrase::Generator ph(R"(
            main = "A{(}B{"}C{|}D{~}E{)}F{{}G{*comment}H{
}"
        )");
        return ph.generate() == "A{B\"C|D~E}F{GH\n"
            && ph.get_error_message().empty();
    });

    ut.set_test("Generate with External Context", [&]() {
        tphrase::Generator ph(R"(
            main = {A} {B} {C}
            A = head
            C = tail
        )");
        return ph.generate({
                { "B", "body" },
                { "C", "foot" },
            }) == "head body tail"
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub", [&]() {
        tphrase::Generator ph(R"(
            main = "The quick brown fox jumps over the lazy dog." ~ /jumps/jumped/ ~ |dog|dogs|
        )");
        return ph.generate() == "The quick brown fox jumped over the lazy dogs."
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub Captured", [&]() {
        tphrase::Generator ph(R"(
            main = "tail head" ~ /([a-z]+) ([a-z]+)/$2 $1/
        )");
        return ph.generate() == "head tail"
            && ph.get_error_message().empty();
    });

    ut.set_test("Gsub Global", [&]() {
        tphrase::Generator ph(R"(
            main = "oooooooooooooooooooo
@@@@@@@@@@@@@@@@@@@@ $$$$$$$$$$$$$$$$$$$$" ~ /o/0/ ~|@|a|g ~'\$'S'
        )");
        return ph.generate() == "0ooooooooooooooooooo\naaaaaaaaaaaaaaaaaaaa S$$$$$$$$$$$$$$$$$$$"
            && ph.get_error_message().empty();
    });

    ut.set_test("Expansion, External Context, and Gsub", [&]() {
        tphrase::Generator ph(R"(
            main = {A} {B} {C} ~ /head/HEAD/ ~ /tail/TAIL/ ~ /body/BODY/
            A = head
            C = tail
        )");
        return ph.generate({ { "B", "body" } }) == "HEAD BODY TAIL"
            && ph.get_error_message().empty();
    });

    ut.set_test("Sharing Syntax", [&]() {
        tphrase::Syntax common(R"(
            sub = {sub2}
        )");
        tphrase::Syntax main1(R"(
            main = {sub}
            sub2 = 1
        )");
        tphrase::Syntax main2(R"(
            main = {sub}
            sub2 = 2
        )");
        main1.add(common);
        main2.add(common);
        tphrase::Generator ph1(main1);
        tphrase::Generator ph2(main2);
        return ph1.generate() == "1" && ph2.generate() == "2"
            && common.get_error_message().empty()
            && main1.get_error_message().empty()
            && main2.get_error_message().empty()
            && ph1.get_error_message().empty()
            && ph2.get_error_message().empty();
    });

    ut.set_test("Sharing Syntax Distribution", [&]() {
        tphrase::Syntax common(R"(
            sub = {sub2}
        )");
        tphrase::Syntax main1(R"(
            main = {sub}
            sub2 = 1 | 2 | 3 | 4
        )");
        tphrase::Syntax main2(R"(
            main = {sub}
            sub2 = A | B
        )");
        main1.add(common);
        main2.add(common);
        tphrase::Generator ph1(main1);
        tphrase::Generator ph2(main2);
        tphrase::Generator::set_random_function(get_default_random_func());
        const bool good1 = check_distribution(ph1, 100000, {
                { "1", 0.25 },
                { "2", 0.25 },
                { "3", 0.25 },
                { "4", 0.25 },
            }, 0.01);
        const bool good2 = check_distribution(ph2, 100000, {
                { "A", 0.5 },
                { "B", 0.5 },
            }, 0.01);
        return good1 && good2
            && common.get_error_message().empty()
            && main1.get_error_message().empty()
            && main2.get_error_message().empty()
            && ph1.get_error_message().empty()
            && ph2.get_error_message().empty();
    });

    ut.set_test("Overwrite Nonterminal", [&]() {
        tphrase::Syntax sub(R"(
            sub = A
        )");
        tphrase::Syntax main(R"(
            main = {sub}
            sub = B
        )");
        main.add(sub);
        tphrase::Generator ph(main);
        return ph.generate() == "A"
            && sub.get_error_message().empty()
            && main.get_error_message().empty()
            && ph.get_error_message().empty();
    });

    ut.set_test("Don't Overwrite Local Nonterminal", [&]() {
        tphrase::Syntax sub(R"(
            _sub = A
        )");
        tphrase::Syntax main(R"(
            main = {_sub}
            _sub = B
        )");
        main.add(sub);
        tphrase::Generator ph(main);
        return ph.generate() == "B"
            && sub.get_error_message().empty()
            && main.get_error_message().empty()
            && ph.get_error_message().empty();
    });

    ut.set_test("Sharing Local Nonterminal", [&]() {
        tphrase::Syntax common(R"(
            sub = {_sub2}
            _sub2 = {sub3}
        )");
        tphrase::Syntax main1(R"(
            main = {sub}
            sub3 = 1
        )");
        tphrase::Syntax main2(R"(
            main = {sub}
            sub3 = 2
        )");
        main1.add(common);
        main2.add(common);
        tphrase::Generator ph1(main1);
        tphrase::Generator ph2(main2);
        return ph1.generate() == "1" && ph2.generate() == "2"
            && common.get_error_message().empty()
            && main1.get_error_message().empty()
            && main2.get_error_message().empty()
            && ph1.get_error_message().empty()
            && ph2.get_error_message().empty();
    });

    return ut.run();
}
