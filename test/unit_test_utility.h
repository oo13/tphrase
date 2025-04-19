/* Utility for the unit test

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

#ifndef TEST_UTILITY_H_
#define TEST_UTILITY_H_

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

std::function<double()> get_sequence_random_func(const std::vector<double> &seq);
std::vector<double> get_linear_weight(std::size_t num);

std::function<double()> get_default_random_func();


namespace tphrase {
    class Generator;
}

bool check_distribution(tphrase::Generator &ph,
                        std::size_t num,
                        const std::unordered_map<std::string, double> &dist,
                        double allowance);

#endif // TEST_UTILITY_H_
