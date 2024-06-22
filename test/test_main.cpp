/* main function for the unit test

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

extern std::size_t test_class_Generator();
extern std::size_t test_class_InputIterator();
extern std::size_t test_class_Syntax();
extern std::size_t test_class_TempRefHolder();
extern std::size_t test_error_utils();
extern std::size_t test_generate();
extern std::size_t test_parse();

int main()
{
    std::size_t r{0};
    r += test_class_Generator();
    r += test_class_InputIterator();
    r += test_class_Syntax();
    r += test_class_TempRefHolder();
    r += test_error_utils();
    r += test_generate();
    r += test_parse();
    return r == 0 ? 0 : 1;
}
