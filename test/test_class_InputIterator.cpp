/* test for class InputIterator

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
#include <cstring>
#include <iterator>
#include <sstream>
#include <string>

#include "tphrase/common/InputIterator.h"

#include "UnitTest.h"

namespace {
    std::size_t destructor_count = 0;

    class TestInputIterator {
    public:
        TestInputIterator();
        TestInputIterator(const TestInputIterator &a) = delete;
        TestInputIterator(TestInputIterator &&a);
        ~TestInputIterator() noexcept;
        TestInputIterator &operator=(const TestInputIterator &a) = delete;
        TestInputIterator &operator=(TestInputIterator &&a);
        TestInputIterator &operator++();
        TestInputIterator &operator++(int);
        char operator*() const;

        using difference_type = int;
        using value_type = char;
        using iterator_concept = std::input_iterator_tag;
    private:
        char v;
    };

    TestInputIterator::TestInputIterator()
        : v{0}
    {
    }

    TestInputIterator::TestInputIterator(TestInputIterator &&a)
        : v{static_cast<char>(10 + a.v)}
    {
    }

    TestInputIterator::~TestInputIterator() noexcept
    {
        ++destructor_count;
    }

    TestInputIterator &TestInputIterator::operator=(TestInputIterator &&a)
    {
        v = static_cast<char>(20 + a.v);
        return *this;
    }

    TestInputIterator &TestInputIterator::operator++()
    {
        ++v;
        return *this;
    }

    TestInputIterator &TestInputIterator::operator++(int)
    {
        ++v;
        return *this;
    }

    char TestInputIterator::operator*() const
    {
        return v;
    }

    bool operator==(const TestInputIterator &a, char e)
    {
        return (*a % 10) == e;
    }
}


std::size_t test_class_InputIterator()
{
    /* avoid to warn unused when concept is not available. */
    TestInputIterator a;
    TestInputIterator b{std::move(a)};
    TestInputIterator c;
    b = std::move(c);
    b++;
    (void)b;

    UnitTest ut("class InputIterator");

    ut.set_enter_function([&]() {
        destructor_count = 0;
    });
    ut.set_leave_function([&]() {
        return true;
    });

    ut.set_test("Custom Input Iterator L-Value", [&]() {
        bool good = true;
        {
            TestInputIterator b;
            char e{2};
            tphrase::InputIterator<TestInputIterator, char> it{b, e};
            good &= *it == 0;
            good &= !it.is_end();
            ++it;
            good &= *it == 1;
            good &= !it.is_end();
            ++it;
            good &= it.is_end();
        }
        return good && destructor_count == 1;
    });

    ut.set_test("std::sstream Iterator L-Value#1", [&]() {
        std::istringstream s{"012"};
        using it_t = std::istreambuf_iterator<char>;
        it_t b{s};
        it_t e;
        tphrase::InputIterator<it_t, it_t> it{b, e};
        bool good = true;
        good &= *it == '0';
        good &= !it.is_end();
        ++it;
        good &= *it == '1';
        good &= !it.is_end();
        ++it;
        good &= *it == '2';
        good &= !it.is_end();
        ++it;
        good &= it.is_end();
        return good;
    });

    ut.set_test("std::sstream Iterator L-Value#2", [&]() {
        std::istringstream s{"012"};
        using it_t = std::istream_iterator<char>;
        it_t b{s};
        it_t e;
        tphrase::InputIterator<it_t, it_t> it{b, e};
        bool good = true;
        good &= *it == '0';
        good &= !it.is_end();
        ++it;
        good &= *it == '1';
        good &= !it.is_end();
        ++it;
        good &= *it == '2';
        good &= !it.is_end();
        ++it;
        good &= it.is_end();
        return good;
    });

    ut.set_test("const char* L-Value", [&]() {
        const char *s{"012"};
        auto b{s};
        auto e{s + std::strlen(s)};
        tphrase::InputIterator<decltype(b), decltype(e)> it{b, e};
        bool good = true;
        good &= *it == '0';
        good &= !it.is_end();
        ++it;
        good &= *it == '1';
        good &= !it.is_end();
        ++it;
        good &= *it == '2';
        good &= !it.is_end();
        ++it;
        good &= it.is_end();
        return good;
    });

    return ut.run();
}
