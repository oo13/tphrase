/** The data structure representing the phrase generator.
    \file DataPhrase.cpp
    \author OOTA, Masato
    \copyright Copyright © 2024 OOTA, Masato
    \par License GPL-3.0-or-later
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
    \endparblock
*/

#include <algorithm>
#include <utility>

#include "DataPhrase.h"
#include "random.h"
#include "select_and_generate.h"

namespace tphrase {
    DataPhrase::DataPhrase()
        : syntaxes{}, weights{}, equalized_chance{false}, ids{}
    {
    }

    std::string DataPhrase::generate(const ExtContext_t &ext_context) const
    {
        return select_and_generate(syntaxes, weights, equalized_chance, ext_context);
    }

    SyntaxID_t DataPhrase::add(const DataSyntax &syntax,
                               const std::string &start_condition,
                               std::string &err_msg)
    {
        return add(DataSyntax{syntax}, start_condition, err_msg);
    }

    SyntaxID_t DataPhrase::add(DataSyntax &&syntax,
                               const std::string &start_condition,
                               std::string &err_msg)
    {
        syntax.bind_syntax(start_condition, err_msg);
        if (!syntax.is_valid()) {
            return 0;
        }

        syntaxes.emplace_back(std::move(syntax));
        weights.emplace_back(get_weight() + syntaxes.back().get_weight());
        if (ids.empty()) {
            ids.emplace_back(1);
        } else {
            ids.emplace_back(ids.back() + 1);
        }

        return ids.back();
    }

    bool DataPhrase::remove(SyntaxID_t id)
    {
        const auto it{std::lower_bound(ids.cbegin(), ids.cend(), id)};
        if (it == ids.end() || *it != id) {
            return false;
        }

        std::size_t idx = it - ids.begin();
        ids.erase(it);
        syntaxes.erase(syntaxes.begin() + idx);
        weights.pop_back();
        double sum{0.0};
        if (idx >= 1) {
            sum = weights[idx - 1];
        }
        for ( ; idx < syntaxes.size(); ++idx) {
            sum += syntaxes[idx].get_weight();
            weights[idx] = sum;
        }
        return true;
    }

    void DataPhrase::clear()
    {
        syntaxes.clear();
        weights.clear();
        equalized_chance = false;
    }

    void DataPhrase::equalize_chance(const bool enable)
    {
        equalized_chance = enable;
    }

    double DataPhrase::get_weight() const
    {
        return weights.empty() ? 0.0 : weights.back();
    }

    std::size_t DataPhrase::get_combination_number() const
    {
        std::size_t sum{0};
        for (const auto &s : syntaxes) {
            sum += s.get_combination_number();
        }
        return sum;
    }
}
