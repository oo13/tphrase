/** The character feeder for the parser.
    \file CharFeeder.cpp
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

#include <cstddef>

#include "CharFeeder.h"
#include "tphrase/common/InputIterator.h"

namespace tphrase {
    CharFeeder::CharFeeder(InputIteratorBase &it) noexcept
        : next_pos(&it),
          num_c(0),
          line(1),
          column(1)
    {
        for (std::size_t i = 0; i <= LOOKAHEAD; ++i) {
            if (!next_pos->is_end()) {
                c[i] = **next_pos;
                ++*next_pos;
                ++num_c;
            } else {
                c[i] = '\0';
            }
        }
    }

    void CharFeeder::next()
    {
        if (is_end()) {
            return;
        }

        if (c[0] == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }

        c[0] = c[1];
        if (!next_pos->is_end()) {
            c[1] = **next_pos;
            ++*next_pos;
        } else {
            c[1] = '\0';
            --num_c;
        }
    }
}
