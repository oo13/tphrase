/** The data structure representing the production rule.
    \file DataProductionRule.h
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

#ifndef TPHRASE_SRC_DATAPRODUCTIONRULE_H_
#define TPHRASE_SRC_DATAPRODUCTIONRULE_H_

#include <cstddef>
#include <string>
#include <unordered_set>

#include "tphrase/common/ext_context.h"
#include "DataOptions.h"
#include "DataGsubs.h"

namespace tphrase {
    class DataSyntax;

    /** The data structure representing the production rule.
        \note The instance bound on a syntax doesn't own the syntax, so the users must keep the syntax alive until the instance is unused.
    */
    class DataProductionRule {
    public:
        DataProductionRule() = delete;
        /** The constructor.
           \param [inout] options The options in the production rule. (moved)
           \param [inout] gsubs The gsubs in the production rule. (moved)
        */
        DataProductionRule(DataOptions &&options, DataGsubs &&gsubs);
        /** The copy constructor.
            \param [in] a The source.
            \note The new instance is bound on no syntax.
        */
        DataProductionRule(const DataProductionRule &a) = default;
        /** The move constructor.
            \param [inout] a The source.
        */
        DataProductionRule(DataProductionRule &&a) = default;

        /** The assignment.
            \param [in] a The source.
            \return *this
            \note The destination instance is bound on no syntax.
        */
        DataProductionRule &operator=(const DataProductionRule &a) = default;
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
        */
        DataProductionRule &operator=(DataProductionRule &&a) = default;

        /** Generate a text.
            \param [in] ext_context The external context that has some nonterminals and the substitutions.
            \return A text.
        */
        std::string generate(const ExtContext_t &ext_context) const;

        /** Get the sum of the weight of the texts.
            \return The sum of the weight.
            \note The return value is meaningless when the instance is bound on no syntax.
        */
        double get_weight() const;
        /** Get the number of the possible texts generated by the instance.
            \return The the number of the possible texts generated by the instance.
        */
        std::size_t get_combination_number() const;

        /** Equalize the chance to select each text.
            \param [in] enable equalized if enable is true. If not, the chance depends on the weight of the text. (Default)
        */
        void equalize_chance(bool enable = true);

        /** Bind the instance on a syntax.
            \param [inout] syntax The syntax to be bound on.
            \param [inout] used_nonterminals The set of the nonterminals that have already used for an expansion.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note used_nonterminals is non-const but the items in the set won't be changed. Some items will be added, and then removed.
            \note An error is detected if the nonterminal existing in used_nonterminals uses for an expansion again.
        */
        void bind_syntax(DataSyntax &syntax,
                         std::unordered_set<std::string> &used_nonterminals,
                         std::string &err_msg);

        /** Fix the reference to the local nonterminal.
            \param [inout] syntax The syntax to be fixed.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note An error is caused if the local nonterminal that is referred by a production rule doesn't exists.
        */
        void fix_local_nonterminal(DataSyntax &syntax,
                                   std::string &err_msg);

    private:
        DataOptions options; /**< The options in the production rule. */
        DataGsubs gsubs; /**< The gsubs in the production rule. */
    };

    inline
    double DataProductionRule::get_weight() const
    {
        return options.get_weight();
    }

    inline
    std::size_t DataProductionRule::get_combination_number() const
    {
        return options.get_combination_number();
    }

    inline
    void DataProductionRule::equalize_chance(bool enable)
    {
        options.equalize_chance(enable);
    }

    inline
    void
    DataProductionRule::bind_syntax(DataSyntax &syntax,
                                    std::unordered_set<std::string> &used_nonterminals,
                                    std::string &err_msg)
    {
        return options.bind_syntax(syntax, used_nonterminals, err_msg);
    }

    inline
    void
    DataProductionRule::fix_local_nonterminal(DataSyntax &syntax,
                                              std::string &err_msg)
    {
        return options.fix_local_nonterminal(syntax, err_msg);
    }
}

#endif // TPHRASE_SRC_DATAPRODUCTIONRULE_H_
