/** Utilities for handling errors.
    \file error_utils.h
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

#ifndef TPHRASE_ERROR_UTILS_H_
#define TPHRASE_ERROR_UTILS_H_

#include <cstddef>
#include <string>
#include <type_traits>

#include "common/InputIterator.h"

namespace tphrase {
    /** Truncate a text of a phrase syntax to help create a user-friendly error message.
        \param [inout] it InputIterator of the source text of a phrase syntax.
        \param [in] min_len The minimum length at truncating.
        \return The truncated first line (except the line with only spaces) of the phrase syntax.
        \note It's assumed that the library users have no need to use this overloaded function directly.
    */
    extern std::string
    trunc_syntax(InputIteratorBase &it, std::size_t min_len = 40);

    /** Truncate a text of a phrase syntax to help create a user-friendly error message.
        \param [in] s The source text of a phrase syntax.
        \param [in] min_len The minimum length at truncating.
        \return The truncated first line (except the line with only spaces) of the phrase syntax.

        Example:
        \code
        std::cerr << "Error in the phrase syntax \"" << trunc_syntax(s) << "\":" << std::endl;
        std::cerr << generator.get_error_message() << std::endl;
        \endcode
    */
    extern std::string
    trunc_syntax(const std::string &s, std::size_t min_len = 40);

    /** Truncate a text of a phrase syntax to help create a user-friendly error message.
        \param [in] s The source text of a phrase syntax.
        \param [in] min_len The minimum length at truncating.
        \return The truncated first line (except the line with only spaces) of the phrase syntax.
    */
    extern std::string
    trunc_syntax(const char *s, std::size_t min_len = 40);

    /** Truncate a text of a phrase syntax to help create a user-friendly error message.
        \tparam T The type of an input iterator. The dereference of a value of T can be convertible to a value of char.
        \tparam S The type of the end for T.
        \param [inout] begin The iterator to point the beginning of the source text of a phrase syntax. (Universal reference; accessed by the reference or moved)
        \param [inout] end The end iterator. (Universal reference; accessed by the reference or moved)
        \param [in] min_len The minimum length at truncating.
        \return The truncated first line (except the line with only spaces) of the phrase syntax.
        \note This function is not used even in C++17 or earlier if begin == end is invalid, therefore trunc_syntax(it, 40), trunc_syntax(std::string{"aaa"}, 40), and trunc_syntax("aaa", 40) raise no errors.
        \attention You want to use std::istreambuf_iterator, instead of std::istream_iterator, because it skips any white spaces by default. If you want to use an istream_iterator, you should configure the target input stream not to skip the spaces (stream.unsetf(std::ios_base::skipws)).
    */
    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    auto trunc_syntax(T &&begin, S &&end, const std::size_t min_len = 40) -> decltype(begin == end, std::string{})
    {
        InputIterator<T, S> it{begin, end};
        return trunc_syntax(it, min_len);
    }
}

#endif // TPHRASE_ERROR_UTILS_H_
