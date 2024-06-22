/* Utility for the unit test

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
