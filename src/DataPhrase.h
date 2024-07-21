/** The data structure representing the phrase generator.
    \file DataPhrase.h
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

#ifndef TPHRASE_SRC_DATAPHRASE_H_
#define TPHRASE_SRC_DATAPHRASE_H_

#include <cstddef>
#include <string>
#include <vector>

#include "tphrase/common/ext_context.h"
#include "tphrase/common/syntax_id.h"
#include "DataSyntax.h"

namespace tphrase {
    /** The data structure representing the phrase generator. */
    class DataPhrase {
    public:
        /** The default constructor. */
        DataPhrase();
        /** The copy constructor.
            \param [in] a The source.
            \note The syntax ID is not changed and the ID generated by the source can be used as a valid ID for the new instance.
        */
        DataPhrase(const DataPhrase &a) = default;
        /** The move constructor.
            \param [inout] a The source.
            \note The syntax ID is not changed and the ID generated by the source can be used as a valid ID for the new instance.
        */
        DataPhrase(DataPhrase &&a) = default;

        /** The assignment.
            \param [in] a The source.
            \return *this
            \note The pre-existing syntax ID is invalidate and the ID generated by the source can be used as a valid ID for this.
        */
        DataPhrase &operator=(const DataPhrase &a) = default;
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
            \note The pre-existing syntax ID is invalidate and the ID generated by the source can be used as a valid ID for this.
        */
        DataPhrase &operator=(DataPhrase &&a) = default;

        /** Generate a phrase.
            \param [in] ext_context The external context that has some nonterminals and the substitutions.
            \return A phrase.
        */
        std::string generate(const ExtContext_t &ext_context) const;

        /** Add a phrase syntax.
            \param [in] syntax The phrase syntax to be copied and added.
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \return ID for the syntax added into the instance, or a value that is equivalent to false if no phrase syntax is added.
            \note Only the phrase syntax that contains the start condition can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note The syntax ID is unique only in this.
            \note The syntax ID is not the same even if the equivalent phrase syntax is added again.
        */
        SyntaxID_t add(const DataSyntax &syntax, const std::string &start_condition, std::vector<std::string> &err_msg);
        /** Add a phrase syntax.
            \param [inout] syntax The phrase syntax to be added. (moved)
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \return ID for the syntax added into the instance, or a value that is equivalent to false if no phrase syntax is added.
            \note Only the phrase syntax that contains the start condition can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note The syntax ID is unique only in this.
            \note The syntax ID is not the same even if the equivalent phrase syntax is added again.
        */
        SyntaxID_t add(DataSyntax &&syntax, const std::string &start_condition, std::vector<std::string> &err_msg);

        /** Remove a phrase syntax.
            \param [in] id The ID for the phrase syntax.
            \return true if the phrase syntax is deleted.
            \note This is an O(n) function, because it's assumed that the function is not used frequently.
            \note The ID for the removed phrase syntax may be reused by add().
        */
        bool remove(SyntaxID_t id);

        /** Clear the syntaxes and the error messages. */
        void clear();

        /** Equalize the chance to select each phrase syntax.
            \param [in] enable equalized if enable is true. If not, the chance depends on the weight of the phrase syntax. (Default)
        */
        void equalize_chance(bool enable = true);

        /** Get the number of the syntaxes in the instance.
            \return The number of the syntaxes in the instance.
        */
        std::size_t get_number_of_syntax() const;
        /** Get the sum of the weight of the syntaxes in the instance.
            \return The sum of the weight of the syntaxes in the instance.
        */
        double get_weight() const;
        /** Get the number of the possible phrases generated by the instance.
            \return The the number of the possible phrases generated by the instance.
        */
        std::size_t get_combination_number() const;

    private:
        std::vector<DataSyntax> syntaxes; /**< The syntaxes in the instance. */
        std::vector<double> weights; /**< weights[i] is the sum of weights[i-1] and the weight to select syntaxes[i]. */
        bool equalized_chance; /**< Is the chance equalized? */
        std::vector<SyntaxID_t> ids; /**< The syntax ID. */
    };

    inline
    std::size_t DataPhrase::get_number_of_syntax() const
    {
        return syntaxes.size();
    }
}

#endif // TPHRASE_SRC_DATAPHRASE_H_
