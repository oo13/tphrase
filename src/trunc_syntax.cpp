/** Function to truncate syntax.
    \file trunc_syntax.cpp
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

#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include "tphrase/error_utils.h"

namespace tphrase {
    std::string trunc_syntax(InputIteratorBase &it, const std::size_t min_len)
    {
        // Skip unimportant characters.
        while (!it.is_end() && !std::isgraph(static_cast<unsigned int>(*it))) {
            ++it;
        }

        // Pick up some characters from the first line.
        std::string s;
        std::size_t count = 0;
        while (!it.is_end()) {
            const char c = *it;
            if (c == '\n') {
                break;
            }
            s += c;
            ++it;
            ++count;

            if (count >= min_len
                && (c == ' '
                    || c == '\t'
                    || c == '='
                    || c == '|'
                    || c == '~')) {
                break;
            }
        }

        // Truncate unimportant characters.
        while (!s.empty() && !std::isgraph(s.back())) {
            s.pop_back();
        }

        if (!it.is_end()) {
            s += "...";
        }
        return s;
    }

    std::string trunc_syntax(const std::string &s, const std::size_t min_len)
    {
        return trunc_syntax(s.cbegin(), s.cend(), min_len);
    }

    std::string trunc_syntax(const char *s, const std::size_t min_len)
    {
        return trunc_syntax(s, s + std::strlen(s), min_len);
    }
}
