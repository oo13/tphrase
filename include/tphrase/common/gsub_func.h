/** The type of the gsub function and the creator for Generator.
    \file gsub_func.h
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

#ifndef TPHRASE_COMMON_GSUB_FUNC_H_
#define TPHRASE_COMMON_GSUB_FUNC_H_

#include <functional>
#include <string>

namespace tphrase {
    /** The type of the gsub function for Generator. */
    using GsubFunc_t = std::function<std::string(const std::string &)>;
    /** The type of the gsub function creator for Generator. */
    using GsubFuncCreator_t = std::function<GsubFunc_t (const std::string &, const std::string &, bool)>;
}

#endif // TPHRASE_COMMON_GSUB_FUNC_H_
