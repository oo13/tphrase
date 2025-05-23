/** The data structure representing the production rule.
    \file DataProductionRule.h
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

#ifndef TPHRASE_SRC_DATAPRODUCTIONRULE_H_
#define TPHRASE_SRC_DATAPRODUCTIONRULE_H_

#include <cstddef>
#include <string>
#include <vector>

#include "tphrase/common/ext_context.h"
#include "DataOptions.h"
#include "DataGsubs.h"

namespace tphrase {
    class DataSyntax;

    /** The data structure representing the production rule.
        \note The instance bound on a syntax doesn't own the syntax, so the users must keep the syntax alive until the instance is unused.
        \note The copied instance is unbound.
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
            \note The new instance is unbound.
        */
        DataProductionRule(const DataProductionRule &a);
        /** The move constructor.
            \param [inout] a The source.
        */
        DataProductionRule(DataProductionRule &&a) = default;

        /** The assignment.
            \param [in] a The source.
            \return *this
            \note The destination instance is unbound.
        */
        DataProductionRule &operator=(const DataProductionRule &a);
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

        /** Set the weight of the production rule.
            \param [in] weight The weight of the production rule. The default value is used if weight is NaN.
            \note The default weight is the value propagated from the options.
        */
        void set_weight(double weight);
        /** Equalize the chance to select each text.
            \param [in] enable equalized if enable is true. If not, the chance depends on the weight of the text. (Default)
        */
        void equalize_chance(bool enable = true);

        /** Bind the instance on a syntax.
            \param [inout] syntax The syntax to be bound on.
            \param [in] epoch The current binding epoch.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \return false if the function call is recursive.
            \note No err_msg is added if false is returned.
            \note An error message is added to err_msg if a text in this instance detects a recursive expansion.
        */
        bool bind_syntax(DataSyntax &syntax,
                         int epoch,
                         std::vector<std::string> &err_msg);

        /** Fix the reference to the local nonterminal.
            \param [inout] syntax The syntax to be fixed.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note An error is caused if the local nonterminal that is referred by a production rule doesn't exists.
        */
        void fix_local_nonterminal(DataSyntax &syntax,
                                   std::vector<std::string> &err_msg);

        /** Reset the binding epoch. */
        void reset_binding_epoch();

    private:
        DataOptions options; /**< The options in the production rule. */
        DataGsubs gsubs; /**< The gsubs in the production rule. */
        int binding_epoch; /**< The binding epoch. */
        double weight; /**< The weight specified by the phrase syntax. */
    };

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
    DataProductionRule::fix_local_nonterminal(DataSyntax &syntax,
                                              std::vector<std::string> &err_msg)
    {
        return options.fix_local_nonterminal(syntax, err_msg);
    }
}

#endif // TPHRASE_SRC_DATAPRODUCTIONRULE_H_
