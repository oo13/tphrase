/* test for class TempRefHolder

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
#include <utility>

#include "tphrase/common/TempRefHolder.h"

#include "UnitTest.h"

namespace {
    std::size_t destructor_count = 0;

    class TargetObject {
    public:
        explicit TargetObject(int a);
        TargetObject(const TargetObject &a) = delete;
        TargetObject(TargetObject &&a);
        ~TargetObject() noexcept;

        int get_value() const;
    private:
        int value;
    };

    TargetObject::TargetObject(int a)
        : value{a}
    {
    }

    TargetObject::TargetObject(TargetObject &&a)
        : value{a.value + 1000000}
    {
    }

    TargetObject::~TargetObject() noexcept
    {
        ++destructor_count;
    }

    int TargetObject::get_value() const
    {
        return value;
    }
}


std::size_t test_class_TempRefHolder()
{
    UnitTest ut("class TempRefHolder");

    ut.set_enter_function([&]() {
        destructor_count = 0;
    });
    ut.set_leave_function([&]() {
        return true;
    });

    ut.set_test("L-Value Holder", [&]() {
        std::size_t dest_count1 = 0;
        std::size_t dest_count2 = 0;
        int value = 0;
        {
            TargetObject target{123};
            {
                tphrase::TempRefHolder<TargetObject> holder{target};
                value = (*holder).get_value();
            }
            dest_count2 = destructor_count;
        }
        dest_count1 = destructor_count;
        return value = 123 && dest_count1 == 1 && dest_count2 == 0;
    });

    ut.set_test("R-Value Holder", [&]() {
        std::size_t dest_count1 = 0;
        std::size_t dest_count2 = 0;
        int value = 0;
        {
            TargetObject target{123};
            {
                tphrase::TempRefHolder<TargetObject> holder{std::move(target)};
                value = (*holder).get_value();
            }
            dest_count2 = destructor_count;
        }
        dest_count1 = destructor_count;
        return value == 1000123 && dest_count1 == 2 && dest_count2 == 1;
    });

    ut.set_test("const L-Value Holder", [&]() {
        std::size_t dest_count1 = 0;
        std::size_t dest_count2 = 0;
        int value = 0;
        {
            TargetObject target{456};
            {
                const tphrase::TempRefHolder<TargetObject> holder{target};
                value = (*holder).get_value();
            }
            dest_count2 = destructor_count;
        }
        dest_count1 = destructor_count;
        return value = 456 && dest_count1 == 1 && dest_count2 == 0;
    });

    ut.set_test("const R-Value Holder", [&]() {
        std::size_t dest_count1 = 0;
        std::size_t dest_count2 = 0;
        int value = 0;
        {
            TargetObject target{456};
            {
                const tphrase::TempRefHolder<TargetObject> holder{std::move(target)};
                value = (*holder).get_value();
            }
            dest_count2 = destructor_count;
        }
        dest_count1 = destructor_count;
        return value == 1000456 && dest_count1 == 2 && dest_count2 == 1;
    });

    ut.set_test("const char* L-Value Holder", [&]() {
        const char *text = "abcd";
        const char *p = nullptr;
        {
            const tphrase::TempRefHolder<const char*> holder{text};
            p = *holder;
        }
        return p == text;
    });

    ut.set_test("const char* R-Value Holder", [&]() {
        const char *text = "abcd";
        const char *p = nullptr;
        {
            const tphrase::TempRefHolder<const char*> holder{text + 1};
            p = *holder;
        }
        return p == text + 1;
    });

    return ut.run();
}
