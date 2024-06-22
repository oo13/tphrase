/** random function
    \file random.cpp
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

#include <functional>
#include <random>
#include "random.h"

namespace {
    /** The default random function generator.
        \return The default random function.
    */
    std::function<double()> gen_default_random_func()
    {
        std::default_random_engine engine;
        std::uniform_real_distribution<> dist(0.0, 1.0);
        return [=]() mutable -> double { return dist(engine); };
    }
}

namespace tphrase {
    RandomFunc_t random = gen_default_random_func();
}
