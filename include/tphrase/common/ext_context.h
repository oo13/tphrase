/** The type of the external context for Generator.
    \file ext_context.h
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

#ifndef TPHRASE_COMMON_EXT_CONTEXT_H_
#define TPHRASE_COMMON_EXT_CONTEXT_H_

#include <functional>
#include <map>
#include <string>

namespace tphrase {
    /** The type of the external context for Generator. */
    using ExtContext_t = std::map<std::string, std::string>;
}

#endif // TPHRASE_COMMON_EXT_CONTEXT_H_
