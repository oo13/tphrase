/* main function for the unit test

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

extern std::size_t test_class_Generator();
extern std::size_t test_class_InputIterator();
extern std::size_t test_class_Syntax();
extern std::size_t test_error_utils();
extern std::size_t test_generate();
extern std::size_t test_parse();

int main()
{
    std::size_t r{0};
    r += test_class_Generator();
    r += test_class_InputIterator();
    r += test_class_Syntax();
    r += test_error_utils();
    r += test_generate();
    r += test_parse();
    return r == 0 ? 0 : 1;
}
