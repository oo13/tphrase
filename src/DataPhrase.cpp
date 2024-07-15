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

#include <utility>

#include "DataPhrase.h"
#include "random.h"
#include "select_and_generate.h"

namespace tphrase {
    DataPhrase::DataPhrase()
        : syntaxes{}, weights{}, equalized_chance{false}
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
        DataSyntax add_syntax{syntax};
        add_syntax.bind_syntax(start_condition, err_msg);
        if (!add_syntax.is_valid()) {
            return 0;
        }

        syntaxes.emplace_back(std::move(add_syntax));
        weights.emplace_back(get_weight() + syntaxes.back().get_weight());

        return syntaxes.back().get_syntax_id();
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

        return syntaxes.back().get_syntax_id();
    }

    bool DataPhrase::remove(SyntaxID_t id)
    {
        bool removed{false};
        for (auto it = syntaxes.cbegin(); it != syntaxes.cend(); ++it) {
            if (it->get_syntax_id() == id) {
                syntaxes.erase(it);
                removed = true;
                break;
            }
        }
        if (removed) {
            weights.pop_back();
            double sum{0.0};
            for (std::size_t i=0; i<weights.size(); ++i) {
                sum += syntaxes[i].get_weight();
                weights[i] = sum;
            }
        }
        return removed;
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
