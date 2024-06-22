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

namespace {
    /** A common error message. */
    const std::string err_msg_main{"The nonterminal \"main\" doesn't exist.\n"};
}

namespace tphrase {
    DataPhrase::DataPhrase()
        : syntaxes{}, weights{}, equalized_chance{false}
    {
    }

    std::string DataPhrase::generate(const ExtContext_t &ext_context) const
    {
        return select_and_generate(syntaxes, weights, equalized_chance, ext_context);
    }

    bool DataPhrase::add(const DataSyntax &syntax, std::string &err_msg)
    {
        if (!syntax.has_main()) {
            err_msg += err_msg_main;
            return false;
        }

        DataSyntax add_syntax{syntax};
        add_syntax.bind_syntax(err_msg);
        if (!add_syntax.is_valid()) {
            return false;
        }

        syntaxes.emplace_back(std::move(add_syntax));
        weights.emplace_back(get_weight() + syntaxes.back().get_weight());

        return true;
    }

    bool DataPhrase::add(DataSyntax &&syntax, std::string &err_msg)
    {
        if (!syntax.has_main()) {
            err_msg += err_msg_main;
            return false;
        }

        syntax.bind_syntax(err_msg);
        if (!syntax.is_valid()) {
            return false;
        }

        syntaxes.emplace_back(std::move(syntax));
        weights.emplace_back(get_weight() + syntaxes.back().get_weight());

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
