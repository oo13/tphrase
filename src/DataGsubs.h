/** The data structure representing the set of the gsub functions.
    \file DataGsubs.h
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

#ifndef TPHRASE_SRC_DATAGSUBS_H_
#define TPHRASE_SRC_DATAGSUBS_H_

#include <cstddef>
#include <string>
#include <vector>

#include "tphrase/common/gsub_func.h"

namespace tphrase {
    /** The data structure representing the set of the gsub functions. */
    class DataGsubs {
    public:
        /** The default constructor. */
        DataGsubs() = default;
        /** The copy constructor.
            \param [in] a The source.
        */
        DataGsubs(const DataGsubs &a) = default;
        /** The move constructor.
            \param [inout] a The source. (moved)
        */
        DataGsubs(DataGsubs &&a) = default;

        /** The assignment.
            \param [in] a The source.
            \return *this
        */
        DataGsubs &operator=(const DataGsubs &a) = default;
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
        */
        DataGsubs &operator=(DataGsubs &&a) = default;

        /** Substitute a string.
            \param [inout] s The source string. (moved)
            \return Substituted string.
        */
        std::string gsub(std::string &&s) const;

        /** Add a gsub function.
            \param [in] pattern The pattern parameter of gsub.
            \param [in] repl The replacement parameter of gsub.
            \param [in] global The global parameter of gsub.
        */
        void add_parameter(const std::string &pattern, const std::string &repl, bool global);

        /** Set the function to create the gsub functions.
            \param [in] creator The function to create the gsub functions.
            \note It causes a parse error that the creator function throw an std::runtime_error at creating a gsub function. The exception handles and suppresses by the parser.
            \note The generator doesn't catch the exception that the created gsub function throws. (The default gsub function doesn't throw the std::runtime_error and generates an error string.)
        */
        static void set_gsub_function_creator(const GsubFuncCreator_t &creator);
        /** Get the current gsub function creator.
            \return The current  gsub function creator.
        */
        static GsubFuncCreator_t get_gsub_function_creator();

    private:
        std::vector<GsubFunc_t> gsubs_f; /**< The set of the gsub functions. */
    };
}

#endif // TPHRASE_SRC_DATAGSUBS_H_
