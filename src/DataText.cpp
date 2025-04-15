/** The data structure representing the text.
    \file DataText.cpp
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

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <utility>

#include "DataProductionRule.h"
#include "DataSyntax.h"
#include "DataText.h"

namespace tphrase {
    DataText::Part_t::Part_t()
        : kind{Kind_t::STRING}, s{}, r{nullptr}
    {
    }

    // k should be STRING or EXPANSION.
    DataText::Part_t::Part_t(Kind_t k, const std::string &v)
        : kind{k}, s{v}, r{nullptr}
    {
    }

    DataText::Part_t::Part_t(Kind_t k, std::string &&v)
        : kind{k}, s{std::move(v)}, r{nullptr}
    {
    }

    DataText::Part_t::Part_t(DataProductionRule *v)
        : kind{Kind_t::ANONYMOUS_RULE}, s{}, r{v}
    {
    }

    DataText::Part_t &DataText::Part_t::operator=(Part_t &&a)
    {
        assert(kind == Kind_t::EXPANSION && r == nullptr);
        kind = a.kind;
        r = a.r;
        return *this;
    }

    DataText::DataText()
        : parts{},
          comb{1},
          weight{1.0},
          weight_by_user{false}
    {
    }

    void DataText::copy_parts(const DataText &a)
    {
        for (const auto &it : a.parts) {
            if (it.kind == Part_t::Kind_t::ANONYMOUS_RULE) {
                parts.emplace_back(new DataProductionRule{*it.r});
            } else {
                parts.emplace_back(it.kind, it.s);
            }
        }
    }

    void DataText::clear_parts() noexcept
    {
        for (auto &it : parts) {
            if (it.kind == Part_t::Kind_t::ANONYMOUS_RULE) {
                delete it.r;
            }
        }
        parts.clear();
    }

    DataText::DataText(const DataText &a)
        : parts{},
          comb{a.comb},
          weight{a.weight},
          weight_by_user{a.weight_by_user}
    {
        copy_parts(a);
    }

    DataText::~DataText() noexcept
    {
        clear_parts();
    }

    DataText &DataText::operator=(const DataText &a)
    {
        clear_parts();

        copy_parts(a);
        comb = a.comb;
        weight = a.weight;
        weight_by_user = a.weight_by_user;

        return *this;
    }

    std::string DataText::generate(const ExtContext_t &ext_context) const
    {
        std::string s;
        for (const auto &p : parts) {
            if (p.kind == Part_t::Kind_t::STRING) {
                s += p.s;
            } else if (p.r) {
                s += p.r->generate(ext_context);
            } else {
                const auto it = ext_context.find(p.s);
                if (it != ext_context.end()) {
                    s += it->second;
                } else {
                    s += p.s;
                }
            }
        }
        return s;
    }

    void DataText::add_string(const std::string &s)
    {
        parts.emplace_back(Part_t::Kind_t::STRING, s);
    }

    void DataText::add_string(std::string &&s)
    {
        parts.emplace_back(Part_t::Kind_t::STRING, std::move(s));
    }

    void DataText::add_expansion(std::string &&name)
    {
        parts.emplace_back(Part_t::Kind_t::EXPANSION, std::move(name));
    }

    void DataText::add_anonymous_rule(DataProductionRule &&r)
    {
        parts.emplace_back(new DataProductionRule{std::move(r)});
    }

    void DataText::set_weight(double w)
    {
        if (std::isnan(w)) {
            weight_by_user = false;
        } else {
            weight_by_user = true;
            weight = w;
        }
    }

    void
    DataText::bind_syntax(DataSyntax &syntax,
                          int epoch,
                          std::vector<std::string> &err_msg)
    {
        double tmp_weight{1.0};
        comb = 1;
        for (auto &p : parts) {
            if (p.kind == Part_t::Kind_t::ANONYMOUS_RULE) {
                p.r->bind_syntax(syntax, epoch, err_msg);
            } else if (p.kind == Part_t::Kind_t::EXPANSION) {
                if (syntax.has_nonterminal(p.s)) {
                    p.r = &syntax.get_production_rule(p.s);
                    if (!p.r->bind_syntax(syntax, epoch, err_msg)) {
                        p.r = nullptr;

                        std::string msg{"Recursive expansion of \""};
                        msg += p.s;
                        msg += "\" is detected.";
                        err_msg.emplace_back(std::move(msg));
                    }
                }
            }
            if (p.r) {
                comb *= p.r->get_combination_number();
                tmp_weight *= p.r->get_weight();
            }
        }
        if (!weight_by_user) {
            weight = tmp_weight;
        }
    }

    void
    DataText::fix_local_nonterminal(DataSyntax &syntax,
                                    std::vector<std::string> &err_msg)
    {
        for (auto &p : parts) {
            if (p.kind == Part_t::Kind_t::EXPANSION
                && syntax.is_local_nonterminal(p.s)) {
                if (syntax.has_nonterminal(p.s)) {
                    p = Part_t(new DataProductionRule{syntax.get_production_rule(p.s)});
                } else {
                    std::string msg{"The local nonterminal \""};
                    msg += p.s;
                    msg += "\" is not found.";
                    err_msg.emplace_back(std::move(msg));
                }
            }
        }
    }
}
