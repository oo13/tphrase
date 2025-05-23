/** The data structure representing the set of the text options.
    \file DataOptions.cpp
    \author OOTA, Masato
    \copyright Copyright © 2024 OOTA, Masato
    \par License GPL-3.0-or-later or Apache-2.0
    \parblock
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
    \endparblock
*/

#include <cmath>
#include <utility>

#include "DataOptions.h"
#include "DataSyntax.h"
#include "DataText.h"
#include "select_and_generate.h"

namespace tphrase {

    DataOptions::DataOptions()
        : texts{}, weights{}, equalized_chance{false}
    {
    }

    std::string DataOptions::generate(const ExtContext_t &ext_context) const
    {
        return select_and_generate(texts, weights, equalized_chance, ext_context);
    }

    double DataOptions::get_weight() const
    {
        return weights.empty() ? 0.0 : weights.back();
    }

    std::size_t DataOptions::get_combination_number() const
    {
        std::size_t sum{0};
        for (const auto &t : texts) {
            sum += t.get_combination_number();
        }
        return sum;
    }

    void DataOptions::add_text(DataText &&s)
    {
        texts.emplace_back(std::move(s));
        weights.emplace_back(get_weight() + 1.0);
    }

    void DataOptions::equalize_chance(const bool enable)
    {
        equalized_chance = enable;
    }

    void
    DataOptions::bind_syntax(DataSyntax &syntax,
                             int epoch,
                             std::vector<std::string> &err_msg)
    {
        double sum{0.0};
        auto it = weights.begin();
        for (auto &t : texts) {
            t.bind_syntax(syntax, epoch, err_msg);
            sum += t.get_weight();
            *it = sum;
            ++it;
        }
    }

    void
    DataOptions::fix_local_nonterminal(DataSyntax &syntax,
                                       std::vector<std::string> &err_msg)
    {
        for (auto &t : texts) {
            t.fix_local_nonterminal(syntax, err_msg);
        }
    }
}
