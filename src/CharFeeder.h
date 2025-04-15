/** The character feeder for the parser.
    \file CharFeeder.h
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

#ifndef TPHRASE_SRC_CHARFEEDER_H_
#define TPHRASE_SRC_CHARFEEDER_H_

#include <cstddef>

namespace tphrase {
    class InputIteratorBase;

    /** The class to feed characters to parser().

        It's able to look ahead one character.

        \note The instance doesn't own the iterator and something to be referred by it, so the users must keep the iterator and the referred object alive until the instance is unused.
    */
    class CharFeeder {
    public:
        CharFeeder() = delete;
       /** The constructor.
            \param [inout] it An input iterator.
        */
        CharFeeder(InputIteratorBase &it);
        CharFeeder(const CharFeeder &a) = delete;
        CharFeeder &operator=(const CharFeeder &a) = delete;

        /** Get the character at the current position.
            \return The character.
            \note return '\0' if is_end() is true.
        */
        char getc() const;
        /** Get the character at the next position.
            \return The character at the next position.
            \note return '\0' if next_is_end() is true.
        */
        char get_nextc() const;

        /** Is the position at the end?
            \return the position is at the end.
        */
        bool is_end() const;
        /** Is the next position at the end?
            \return the next position is at the end.
        */
        bool next_is_end() const;

        /** Step forward.
            \note It does nothing if is_end() is true.
        */
        void next();

        /** Get the line number of the current position.
            \return The line number.
        */
        std::size_t get_line_number() const;
        /** Get the column number of the current position.
            \return The column number.
            \note The unit of the column number is byte.
        */
        std::size_t get_column_number() const;

    private:
        /** The number of the lookahead character. */
        static constexpr std::size_t LOOKAHEAD = 1;

        /** The iterator to point to the next position.
            \note The instance doesn't own the iterator.
        */
        InputIteratorBase *next_pos;
        char c[LOOKAHEAD + 1]; /**< The character buffer. */
        std::size_t num_c; /**< The number of the valid characters in c. */
        std::size_t line; /**< The line number at the current position. */
        std::size_t column; /**< The column number at the current position. */
    };

    inline
    char CharFeeder::getc() const
    {
        return c[0];
    }

    inline
    char CharFeeder::get_nextc() const
    {
        return c[1];
    }

    inline
    bool CharFeeder::is_end() const
    {
        return num_c == 0;
    }

    inline
    bool CharFeeder::next_is_end() const
    {
        return num_c == 1;
    }

    inline
    std::size_t CharFeeder::get_line_number() const
    {
        return line;
    }

    inline
    std::size_t CharFeeder::get_column_number() const
    {
        return column;
    }
}

#endif // TPHRASE_SRC_CHARFEEDER_H_
