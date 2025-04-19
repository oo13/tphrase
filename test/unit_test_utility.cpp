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

#include <cmath>
#include <iostream>
#include <random>

#include "tphrase/Generator.h"

#include "unit_test_utility.h"

std::function<double()> get_sequence_random_func(const std::vector<double> &seq)
{
    std::size_t i = 0;
    return [=]() mutable {
        if (i < seq.size()) {
            const double r = seq[i];
            ++i;
            return r;
        } else {
            return 0.0;
        }
    };
}

std::vector<double> get_linear_weight(const std::size_t num)
{
    std::vector<double> w{};
    for (std::size_t i=1; i<=num; ++i) {
        w.push_back((i - 0.5) / num);
    }
    return w;
}

std::function<double()> get_default_random_func()
{
    std::default_random_engine engine;
    std::uniform_real_distribution<> dist(0.0, 1.0);
    return [=]() mutable -> double { return dist(engine); };
}

bool check_distribution(tphrase::Generator &ph,
                        std::size_t num,
                        const std::unordered_map<std::string, double> &dist,
                        double allowance)
{
    std::unordered_map<std::string, std::size_t> count{};
    for (std::size_t i = 0; i < num; ++i) {
        const std::string s{ph.generate()};
        if (dist.find(s) != dist.end()) {
            auto it = count.find(s);
            if (it != count.end()) {
                ++(it->second);
            } else {
                count.insert({s, 1});
            }
        } else {
            std::cout << "The result \"" << s << "\" is not expected.\n";
            return false;
        }
    }

    bool match = true;
    for (const auto &dist_it : dist) {
        double op = 0.0;
        const auto count_it = count.find(dist_it.first);
        if (count_it != count.end()) {
            op = static_cast<double>(count_it->second) / num;
        }
        if (std::abs(op - dist_it.second) > allowance) {
            std::cout << "The probability (" << op
                      << ") of the result \"" << dist_it.first
                      << "\" is not match the expected value " << dist_it.second
                      << '.' << std::endl;
            match = false;
        }
    }
    return match;
}
