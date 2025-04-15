/** The data structure representing the text.
    \file DataText.h
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

#ifndef TPHRASE_SRC_DATATEXT_H_
#define TPHRASE_SRC_DATATEXT_H_

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "tphrase/common/ext_context.h"

namespace tphrase {
    class DataProductionRule;
    class DataSyntax;

    /** The data structure representing the text.
        \note The instance bound on a syntax doesn't own the syntax, so the users must keep the syntax alive until the instance is unused.
        \note The copied instance is unbound.
    */
    class DataText {
    public:
        /** The default constructor. */
        DataText();
        /** The copy constructor.
            \param [in] a The source.
            \note The new instance is unbound.
        */
        DataText(const DataText &a);
        /** The move constructor.
            \param [inout] a The source.
        */
        DataText(DataText &&a) = default;

        /** The destructor. */
        ~DataText() noexcept;

        /** The assignment.
            \param [in] a The source.
            \return *this
            \note The destination instance is unbound.
        */
        DataText &operator=(const DataText &a);
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
        */
        DataText &operator=(DataText &&a) = default;

        /** Generate a text.
            \param [in] ext_context The external context that has some nonterminals and the substitutions.
            \return A text.
        */
        std::string generate(const ExtContext_t &ext_context) const;

        /** Get the weight of the texts.
            \return The weight.
            \note The return value is meaningless when the instance is bound on no syntax.
        */
        double get_weight() const;
        /** Get the number of the possible texts generated by the instance.
            \return The the number of the possible texts generated by the instance.
        */
        std::size_t get_combination_number() const;

        /** Add a string that is a part of the text.
            \param [in] s The string.
        */
        void add_string(const std::string &s);
        /** Add a string that is a part of the text.
            \param [inout] s The string. (moved)
        */
        void add_string(std::string &&s);
        /** Add an expansion name that is a part of the text.
            \param [inout] name The expansion name. (moved)
        */
        void add_expansion(std::string &&name);
        /** Add an anonymous rule that is a part of the text.
            \param [inout] r The anonymous rule. (moved)
        */
        void add_anonymous_rule(DataProductionRule &&r);

        /** Set the weight of the text manually.
            \param [in] w The weight. The default value is used if weight is NaN.
            \note It disable the automatic calculation of the weight if w is not NaN.
        */
        void set_weight(double w);

        /** Bind the instance on a syntax.
            \param [inout] syntax The syntax to be bound on.
            \param [in] epoch The current binding epoch.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note An error message is added to err_msg if this instance detects a recursive expansion.
        */
        void bind_syntax(DataSyntax &syntax,
                         int epoch,
                         std::vector<std::string> &err_msg);

        /** Fix the reference to the local nonterminal.
            \param [inout] syntax The syntax to be fixed.
            \param [inout] err_msg The error messages are added if some errors are detected.
            \note An error is caused if the local nonterminal that is referred by a production rule doesn't exists.
        */
        void fix_local_nonterminal(DataSyntax& syntax, std::vector<std::string> &err_msg);

    private:
        /** Copy another DataText to parts.
            \param [in] a The source.
            \note It doesn't copy the production rule except the anonymous rule, it means the destination DataText is unbound.
        */
        void copy_parts(const DataText &a);

        /** Clear the anonymous rule in this. */
        void clear_parts() noexcept;

        /** A part of the text.
            \note The instance doesn't own the instance of DataProductionRule.
        */
        struct Part_t {
            /** The type of the kind. */
            enum class Kind_t {
                STRING, /**< The part is a string. */
                EXPANSION, /**< The part is an expansion. */
                ANONYMOUS_RULE /**< The part is an anonymous rule. */
            } kind; /**< The kind of the part. */
            const std::string s; /**< The string, or the name of the expansion. */
            DataProductionRule *r; /**< The anonymous rule, or the production rule assigned to the expansion. */

            /** The default constructor. */
            Part_t();
            /** Constructor for a string or an expansion.
                \param [in] k The kind of the part.
                \param [in] v The string, or the name of the expansion.
            */
            Part_t(Kind_t k, const std::string &v);
            /** Constructor for a string or an expansion.
                \param [in] k The kind of the part.
                \param [inout] v The string, or the name of the expansion. (moved)
            */
            Part_t(Kind_t k, std::string &&v);
            /** Constructor for an anonymous rule.
                \param [in] v The pointer to the anonymous rule.
                \note The instance doesn't own the anonymous rule.
            */
            explicit Part_t(DataProductionRule *v);
            Part_t(const Part_t &a) = delete;
            /** The move constructor.
                \param [inout] a The source.
            */
            Part_t(Part_t &&a) = default;
            Part_t &operator=(const Part_t &a) = delete;
            /** The move assignment.
                \param [inout] a The source. (moved)
                \return *this
                \note This kind must be EXPANSION.
            */
            Part_t &operator=(Part_t &&a);
        };

        std::vector<Part_t> parts; /**< The parts of the text. */
        std::size_t comb; /**< The number of the combination. */
        double weight; /**< The weight of the text. */
        bool weight_by_user; /**< Was the weight manually set? */
    };

    inline
    double DataText::get_weight() const
    {
        return weight;
    }

    inline
    std::size_t DataText::get_combination_number() const
    {
        return comb;
    }
}

#endif // TPHRASE_SRC_DATATEXT_H_
