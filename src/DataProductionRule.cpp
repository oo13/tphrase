/** The data structure representing the production rule.
    \file DataProductionRule.cpp
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

#include <utility>

#include "DataProductionRule.h"

namespace tphrase {

    DataProductionRule::DataProductionRule(const DataProductionRule &a)
        : options{a.options},
          gsubs{a.gsubs},
          binding_epoch{0}
    {
    }

    DataProductionRule::DataProductionRule(DataOptions &&in_options, DataGsubs &&in_gsubs)
        : options{std::move(in_options)},
          gsubs{std::move(in_gsubs)},
          binding_epoch{0}
    {
    }

    DataProductionRule &DataProductionRule::operator=(const DataProductionRule &a)
    {
        options = a.options;
        gsubs = a.gsubs;
        binding_epoch = 0;
        return *this;
    }

    std::string DataProductionRule::generate(const ExtContext_t &ext_context) const
    {
        return gsubs.gsub(options.generate(ext_context));
    }

    bool
    DataProductionRule::bind_syntax(DataSyntax &syntax,
                                    int epoch,
                                    std::string &err_msg)
    {
        if (binding_epoch < 0) {
            // Recursion
            return false;
        } else if (binding_epoch == epoch) {
            // Already bound.
            return true;
        }

        binding_epoch = -1;
        options.bind_syntax(syntax, epoch, err_msg);
        binding_epoch = epoch;
        return true;
    }
}
