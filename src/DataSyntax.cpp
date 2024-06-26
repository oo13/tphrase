/** The data structure representing the phrase syntax.
    \file DataSyntax.cpp
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

#include <stdexcept>

#include "DataSyntax.h"

namespace {
    /** A common nonterminal name "main". */
    const std::string nonterminal_main{"main"};

    /** Get the production rule assigned to the nonterminal "main".
        \param [in] m The assignments in the syntax.
        \return The production rule assigned to the nonterminal "main" if it exists. If not, nullptr is returned.
    */
    tphrase::DataProductionRule
    *get_main_rule(std::unordered_map<std::string, tphrase::DataProductionRule> &m)
    {
        auto it = m.find(nonterminal_main);
        if (it != m.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    /** Insert an item, but assign the item if it already exists.
        \param [in] m The target map.
        \param [inout] k The key. (moved)
        \param [inout] r The value. (moved)
        \note This function is equivalent of std::unordered_map::insert_to_assign() in C++17 or later.
    */
    void
    insert_or_assign(std::unordered_map<std::string, tphrase::DataProductionRule> &m,
                     std::string &&k,
                     tphrase::DataProductionRule &&r)
    {
        auto it = m.find(k);
        if (it == m.end()) {
            m.emplace(std::move(k), std::move(r));
        } else {
            it->second = std::move(r);
        }
    }
}

namespace tphrase {
    DataSyntax::DataSyntax()
        : assignments{},
          main_rule{nullptr},
          is_bound{false},
          binding_epoch{0}
    {
    }

    DataSyntax::DataSyntax(const DataSyntax &a)
        : assignments{a.assignments},
          main_rule{get_main_rule(assignments)},
          is_bound{false},
          binding_epoch{0}
    {
        if (a.is_bound) {
            std::string err_msg;
            bind_syntax(err_msg); // It should not generate any error messages.
        }
    }

    DataSyntax &DataSyntax::operator=(const DataSyntax &a)
    {
        assignments = a.assignments;
        main_rule = get_main_rule(assignments);
        is_bound = false;
        binding_epoch = 0;
        if (a.is_bound) {
            std::string err_msg;
            bind_syntax(err_msg); // It should not generate any error messages.
        }
        return *this;
    }

    std::string DataSyntax::generate(const ExtContext_t &ext_context) const
    {
        if (is_valid()) {
            return main_rule->generate(ext_context);
        } else {
            return "nil";
        }
    }

    double DataSyntax::get_weight() const
    {
        if (is_valid()) {
            return main_rule->get_weight();
        } else {
            return 0.0;
        }
    }

    std::size_t DataSyntax::get_combination_number() const
    {
        if (is_valid()) {
            return main_rule->get_combination_number();
        } else {
            return 0;
        }
    }

    bool DataSyntax::has_nonterminal(const std::string &nonterminal) const
    {
        return assignments.find(nonterminal) != assignments.end();
    }

    bool DataSyntax::is_local_nonterminal(const std::string &nonterminal) const
    {
        return nonterminal[0] == '_';
    }

    DataProductionRule &
    DataSyntax::get_production_rule(const std::string &nonterminal)
    {
        auto it = assignments.find(nonterminal);
        if (it == assignments.end()) {
            throw std::runtime_error("The nonterminal is not found.");
        }
        return it->second;
    }

    void DataSyntax::add(std::string &&nonterminal, DataProductionRule &&rule)
    {
        insert_or_assign(assignments, std::move(nonterminal), std::move(rule));
        main_rule = get_main_rule(assignments);
        is_bound = false;
    }

    void DataSyntax::add(DataSyntax &&syntax)
    {
        for (auto &it : syntax.assignments) {
            std::string s{it.first};
            insert_or_assign(assignments, std::move(s), std::move(it.second));
        }
        main_rule = get_main_rule(assignments);
        is_bound = false;
    }

    void DataSyntax::bind_syntax(std::string &err_msg)
    {
        if (is_bound || !main_rule) {
            return;
        } else {
            ++binding_epoch;
            // The three variations (initial, current, not current) are enough to distinguish the binding epoch, even if you call a function of DataSyntax class directly. (It's generally 0 or 1 because the functions of class Syntax and Generator call neither add() nor bind_syntax() to the syntax that already bound.)
            if (binding_epoch > 2) {
                binding_epoch = 1;
            }
            const std::size_t prev_len{err_msg.size()};
            main_rule->bind_syntax(*this, binding_epoch, err_msg);
            is_bound = err_msg.size() == prev_len;
        }
    }

    void DataSyntax::fix_local_nonterminal(std::string &err_msg)
    {
        for (auto &it : assignments) {
            it.second.fix_local_nonterminal(*this, err_msg);
        }
        for (auto it = assignments.begin(); it != assignments.end(); ) {
            if (is_local_nonterminal(it->first)) {
                it = assignments.erase(it);
            } else {
                ++it;
            }
        }
    }

    void DataSyntax::clear()
    {
        assignments.clear();
        main_rule = nullptr;
        is_bound = false;
        binding_epoch = 0;
    }
}
