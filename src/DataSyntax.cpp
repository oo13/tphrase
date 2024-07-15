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

#include <atomic>
#include <limits>
#include <stdexcept>
#include <utility>

#include "DataSyntax.h"

namespace {
    /** The seed for the syntax ID. */
    std::atomic<tphrase::SyntaxID_t> syntax_id_seed{1};

    /** Get the new value that can be used as syntax ID.
        \return New ID.
    */
    tphrase::SyntaxID_t get_new_syntax_id()
    {
        tphrase::SyntaxID_t new_id{syntax_id_seed++};
        if (!new_id) {
            throw std::runtime_error("The syntax ID must not be equivalent to false.");
        }
        return new_id;
    }
}

namespace tphrase {
    DataSyntax::DataSyntax()
        : assignments{},
          start_it{assignments.end()},
          binding_epoch{0},
          id{0}
    {
    }

    DataSyntax::DataSyntax(const DataSyntax &a)
        : assignments{a.assignments},
          start_it{assignments.end()},
          binding_epoch{0},
          id{a.id}
    {
        if (a.start_it != a.assignments.end()) {
            std::string err_msg;
            bind_syntax_preserve_id(a.start_it->first, err_msg); // It should not generate any error messages.
        }
    }

    DataSyntax::DataSyntax(DataSyntax &&a)
        : assignments{},
          start_it{a.start_it},
          binding_epoch{a.binding_epoch},
          id{a.id}
    {
        // swap() doesn't invalidate any iterators except end().
        const bool is_end{a.start_it == a.assignments.end()};
        assignments.swap(a.assignments);
        if (is_end) {
            start_it = assignments.end();
        }
    }

    DataSyntax &DataSyntax::operator=(const DataSyntax &a)
    {
        assignments = a.assignments;
        start_it = assignments.end();
        binding_epoch = 0;
        id = a.id;
        if (a.start_it != a.assignments.end()) {
            std::string err_msg;
            bind_syntax_preserve_id(a.start_it->first, err_msg); // It should not generate any error messages.
        }
        return *this;
    }

    DataSyntax &DataSyntax::operator=(DataSyntax &&a)
    {
        // swap() doesn't invalidate any iterators except end().
        const bool is_end{a.start_it == a.assignments.end()};
        assignments.swap(a.assignments);
        if (is_end) {
            start_it = assignments.end();
        } else {
            start_it = a.start_it;
        }
        binding_epoch = a.binding_epoch;
        id = a.id;
        return *this;
    }

    std::string DataSyntax::generate(const ExtContext_t &ext_context) const
    {
        if (is_valid()) {
            return start_it->second.generate(ext_context);
        } else {
            return "nil";
        }
    }

    double DataSyntax::get_weight() const
    {
        if (is_valid()) {
            return start_it->second.get_weight();
        } else {
            return 0.0;
        }
    }

    std::size_t DataSyntax::get_combination_number() const
    {
        if (is_valid()) {
            return start_it->second.get_combination_number();
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
        auto it{assignments.find(nonterminal)};
        if (it == assignments.end()) {
            throw std::runtime_error("The nonterminal is not found.");
        }
        return it->second;
    }

    bool DataSyntax::add(std::string &&nonterminal,
                         DataProductionRule &&rule,
                         std::string &err_msg)
    {
        start_it = assignments.end();
        id = 0;
        auto it{assignments.find(nonterminal)};
        if (it == assignments.end()) {
            assignments.emplace(std::move(nonterminal), std::move(rule));
            return true;
        } else {
            // This is a parse error and it needs no newline.
            err_msg += "The nonterminal \"";
            err_msg += nonterminal;
            err_msg += "\" is already defined.";
            return false;
        }
    }

    void DataSyntax::add(DataSyntax &&syntax, std::string &err_msg)
    {
        start_it = assignments.end();
        id = 0;
        for (auto &it : syntax.assignments) {
            auto found{assignments.find(it.first)};
            if (found == assignments.end()) {
                assignments.emplace(it.first, std::move(it.second));
            } else {
                found->second = std::move(it.second);
                err_msg += "The nonterminal \"";
                err_msg += it.first;
                err_msg += "\" is already defined.\n";
            }
        }
    }

    bool DataSyntax::bind_syntax(const std::string &start_condition, std::string &err_msg)
    {
        id = 0;
        const bool is_bound{bind_syntax_preserve_id(start_condition, err_msg)};
        if (is_bound) {
            id = get_new_syntax_id();
        }
        return is_bound;
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
        start_it = assignments.end();
        binding_epoch = 0;
        id = 0;
    }

    bool DataSyntax::bind_syntax_preserve_id(const std::string &start_condition, std::string &err_msg)
    {
        auto it{assignments.find(start_condition)};
        if (it != assignments.end()) {
            start_it = it;
        } else {
            start_it = assignments.end();
            err_msg += "The nonterminal \"";
            err_msg += start_condition;
            err_msg += "\" doesn't exist.\n";
            return false;
        }

        ++binding_epoch;
        // It's generally 0 or 1 because the functions of class Syntax and Generator don't call bind_syntax() to the syntax that already bound. (The three variations (initial, current, not current) are enough to distinguish the binding epoch unless start_condition is changed.)
        if (binding_epoch == std::numeric_limits<int>::max()) {
            for (auto it : assignments) {
                it.second.reset_binding_epoch();
            }
            binding_epoch = 1;
        }
        const std::size_t prev_len{err_msg.size()};
        start_it->second.bind_syntax(*this, binding_epoch, err_msg);
        const bool is_bound{err_msg.size() == prev_len};
        if (!is_bound) {
            start_it = assignments.end();
        }
        return is_bound;
    }
}
