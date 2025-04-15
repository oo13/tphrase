/** The data structure representing the set of the gsub functions.
    \file DataGsubs.cpp
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

#include <regex>
#include <stdexcept>

#include "DataGsubs.h"

namespace {
    /** Make a substituting function out of std::regex.
        \param [in] pattern Pattern parameter for gsub. It's copied and captured.
        \param [in] repl Replacement parameter for gsub. It's copied and captured.
        \param [in] global Global parameter for gsub. It's copied and captured.
        \return The substituting function with a string to substitute.
    */
    tphrase::GsubFunc_t create_regex_gsub(const std::string &pattern,
                                          const std::string &repl,
                                          const bool global)
    {
        // It may throw a std::runtime_error at creating because the parser catches it.
        std::regex re{pattern};

        return [=](const std::string &s) {
            const auto f = global ? std::regex_constants::match_default
                                  : std::regex_constants::format_first_only;
            try {
                return std::regex_replace(s, re, repl, f);
            } catch (const std::runtime_error &e) {
                // The user's input should not cause an exception.
                return std::string{e.what()};
            }
        };
    }

    /** The default function to create the gsub function. */
    tphrase::GsubFuncCreator_t gsub_creator = create_regex_gsub;
}

namespace tphrase {

    std::string DataGsubs::gsub(std::string &&s) const
    {
        std::string r{std::move(s)};
        for (const auto &f : gsubs_f) {
            r = f(r);
        }
        return r;
    }

    void DataGsubs::add_parameter(const std::string &pattern, const std::string &repl, const bool global)
    {
        gsubs_f.emplace_back(gsub_creator(pattern, repl, global));
    }

    void DataGsubs::set_gsub_function_creator(const GsubFuncCreator_t &creator)
    {
        gsub_creator = creator;
    }

    GsubFuncCreator_t DataGsubs::get_gsub_function_creator()
    {
        return gsub_creator;
    }
}
