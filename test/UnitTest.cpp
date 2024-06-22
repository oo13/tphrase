/* Class for Unit Test

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

#include <iostream>
#include <utility>

#include "UnitTest.h"

UnitTest::UnitTest(const std::string &test_group_name)
    : group_name{test_group_name},
      enter{[](){return;}},
      leave{[](){return true;}},
      items{}
{
}

void UnitTest::set_test(const std::string &test_name,
                        const std::function<bool()> &f)
{
    items.emplace_back(test_name, f);
}

void UnitTest::set_test(const std::string &test_name,
                        std::function<bool()> &&f)
{
    items.emplace_back(test_name, std::move(f));
}

std::size_t UnitTest::run(bool verbose)
{
    if (!group_name.empty()) {
        std::cout << "Test Group \"" << group_name << "\":" << std::endl;
    }
    std::size_t num_tests = 0;
    std::size_t num_errors = 0;
    for (const auto &it : items) {
        ++num_tests;
        if (verbose) {
            std::cout << "Testing \"" << it.name << "\"...";
        }
        enter();
        const bool test_result = it.func();
        const bool leave_result = leave();
        if (test_result && leave_result) {
            if (verbose) {
                std::cout << " Done." << std::endl;
            }
        } else {
            ++num_errors;
            if (!verbose) {
                std::cout << "Testing \"" << it.name << "\"...";
            }
            std::cout << " Error!" << std::endl;
        }
    }
    if (num_errors == 0) {
        std::cout << "No errors are detected";
    } else if (num_errors == 1) {
        std::cout << "An error is detected";
    } else {
        std::cout << num_errors << " errors are detected";
    }
    if (num_tests == 1) {
        std::cout << " in a test." << std::endl;
    } else {
        std::cout << " in " << num_tests << " tests." << std::endl;
    }
    return num_errors;
}

void UnitTest::set_enter_function(const std::function<void()> &f)
{
    enter = f;
}

void UnitTest::set_leave_function(const std::function<bool()> &f)
{
    leave = f;
}

UnitTest::TestItem::TestItem(const std::string &n, const std::function<bool()> &f)
    : name{n}, func{f}
{
}

UnitTest::TestItem::TestItem(const std::string &n, std::function<bool()> &&f)
    : name{n}, func{std::move(f)}
{
}
