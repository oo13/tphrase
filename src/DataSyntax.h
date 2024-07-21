/** The data structure representing the phrase syntax.
    \file DataSyntax.h
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

#ifndef TPHRASE_SRC_DATASYNTAX_H_
#define TPHRASE_SRC_DATASYNTAX_H_

#include <cstddef>
#include <unordered_map>
#include <string>
#include <vector>

#include "tphrase/common/ext_context.h"
#include "DataProductionRule.h"

namespace tphrase {
    /** The data structure representing the phrase syntax. */
    class DataSyntax {
    public:
        /** The default constructor. */
        DataSyntax();
        /** The copy constructor.
            \param [in] a The source.
            \note The new instance is bound on this if the source is bound.
        */
        DataSyntax(const DataSyntax &a);
        /** The move constructor.
            \param [inout] a The source.
        */
        DataSyntax(DataSyntax &&a);

        /** The assignment.
            \param [in] a The source.
            \return *this
            \note The destination instance is bound on this if the source is bound.
        */
        DataSyntax &operator=(const DataSyntax &a);
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
        */
        DataSyntax &operator=(DataSyntax &&a);

        /** Generate a phrase.
            \param [in] ext_context The external context that has some nonterminals and the substitutions.
            \return A phrase.
        */
        std::string generate(const ExtContext_t &ext_context) const;

        /** Get the sum of the weight of the texts.
            \return The sum of the weight.
            \note The return value is meaningless if is_valid() is false.
        */
        double get_weight() const;
        /** Get the number of the possible texts generated by the instance.
            \return The the number of the possible texts generated by the instance.
            \note The return value is meaningless if is_valid() is false.
        */
        std::size_t get_combination_number() const;

        /** Does the instance has the nonterminal?
            \param [in] nonterminal The target nonterminal.
            \return The instance has the nonterminal.
        */
        bool has_nonterminal(const std::string &nonterminal) const;
        /** Is is a local instance?
            \param [in] nonterminal The target nonterminal.
            \return It's a local nonterminal.
        */
        bool is_local_nonterminal(const std::string &nonterminal) const;
        /** Get the production rule assigned to the nonterminal.
            \param [in] nonterminal The target nonterminal.
            \return The production rule.
        */
        DataProductionRule &get_production_rule(const std::string &nonterminal);

        /** Is the instance able to generate a phrase?
            \return The instance is able to generate a phrase.
            \note Is means the instance has the production rule assigned to the start condition and is successfully bound.
        */
        bool is_valid() const;

        /** Add a pair of a nonterminal and a production rule.
            \param [inout] nonterminal The nonterminal. (moved)
            \param [inout] rule The production rule to be assigned to the nonterminal. (moved)
            \param [inout] err_msg The error message is added if an error is detected.
            \return true if no errors are detected.
            \note It has a side effect to make the instance the unbound state (although the object that was bound on this remains bound on it).
            \note If this already contains nonterminal, then: (1) nonterminal and rule do NOT add to this, (2) an error message is added to err_msg, (3) false is returned.
            \note A single error may occur at most.
        */
        bool add(std::string &&nonterminal, DataProductionRule &&rule, std::string &err_msg);
        /** Add a set of the assignments.
            \param [inout] syntax The syntax with the assignments to be added. (moved)
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note It has a side effect to make the instance the unbound state (although the object that was bound on this remains bound on it).
            \note If syntax has the nonterminal that this already contains, then: (1) the nonterminal in syntax overwrites it, (2) an error message is added to err_msg.
        */
        void add(DataSyntax &&syntax, std::vector<std::string> &err_msg);

        /** Try to bind the expansions on the nonterminals in this.
            \param [in] start_condition The nonterminal where is the start condition.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \return true if no errors are detected.
            \note Only the nonterminals that are directly or indirectly referred by the start condition are tried binding.
            \note An error is caused if the recursive reference to a nonterminal exists.
            \note An error is cause if the nonterminal start_condition doesn't exist.
        */
        bool bind_syntax(const std::string &start_condition, std::vector<std::string> &err_msg);

        /** Fix the reference to the local nonterminal.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note An error is caused if the local nonterminal that is referred by a production rule doesn't exists.
        */
        void fix_local_nonterminal(std::vector<std::string> &err_msg);

        /** Clear the instance. */
        void clear();

    private:
        std::unordered_map<std::string, DataProductionRule> assignments; /**< The assignments in the syntax. */
        decltype(assignments)::iterator start_it; /**< The iterator for the start condition. */
        int binding_epoch; /**< The binding epoch. */
    };

    inline
    bool DataSyntax::is_valid() const
    {
        return start_it != assignments.end();
    }
}

#endif // TPHRASE_SRC_DATASYNTAX_H_
