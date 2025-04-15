/** parse function.
    \file parse.h
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

#ifndef TPHRASE_SRC_PARSE_H_
#define TPHRASE_SRC_PARSE_H_

#include <string>
#include <vector>

#include "tphrase/common/InputIterator.h"
#include "DataSyntax.h"

namespace tphrase {
    /** Parse a phrase syntax.
        \param [inout] p The source text.
        \param [inout] err_msg The error messages are added if some errors are detected.
        \return The phrase syntax.
        \note The return value is bound on no syntax.
    */
    extern DataSyntax parse(InputIteratorBase &p, std::vector<std::string> &err_msg);
}

#endif // TPHRASE_SRC_PARSE_H_
