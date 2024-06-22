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

#ifndef TEST_UNITTEST_H_
#define TEST_UNITTEST_H_

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

class UnitTest {
public:
    explicit UnitTest(const std::string &test_group_name);

    void set_test(const std::string &test_name, const std::function<bool()> &f);
    void set_test(const std::string &test_name, std::function<bool()> &&f);

    std::size_t run(bool verbose = false);

    void set_enter_function(const std::function<void()> &f);
    void set_leave_function(const std::function<bool()> &f);

private:
    std::string group_name;
    std::function<void()> enter;
    std::function<bool()> leave;

    struct TestItem {
        std::string name;
        std::function<bool()> func;

        TestItem(const std::string &n, const std::function<bool()> &f);
        TestItem(const std::string &n, std::function<bool()> &&f);
    };
    std::vector<TestItem> items;
};

#endif // TEST_UNITTEST_H_
