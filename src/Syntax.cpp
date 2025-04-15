/** Phrase Syntax
    \file Syntax.cpp
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

#include <cstddef>
#include <cstring>
#include <utility>

#include "tphrase/Generator.h"
#include "DataSyntax.h"
#include "parse.h"

namespace tphrase {

    /** The type of the private data of the class Syntax. */
    struct Syntax::Impl {
        std::vector<std::string> err_msg; /**< The holder of the error messages. */
        DataSyntax data; /**< The data structure for the phrase syntax. */

        /** The default constructor. */
        Impl() = default;
        /** Parse the source text to create the data structure.
            \param [in] it The source text.
            \note Some parse errors may be detected if src has errors.
        */
        Impl(InputIteratorBase &it);
        /** Parse the source text to create the data structure.
            \param [in] it The source text.
            \note Some parse errors may be detected if src has errors.
        */
        Impl(InputIteratorBase &&it);
        /** The copy constructor.
            \param [in] a The source.
        */
        Impl(const Impl &a) = default;
        /** The assignment.
            \param [in] a The source.
            \return *this
        */
        Impl &operator=(const Impl &a) = default;
    };

    Syntax::Impl::Impl(InputIteratorBase &it)
        : err_msg{}, data{parse(it, err_msg)}
    {
        if (!err_msg.empty()) {
            data.clear();
        }
    }

    Syntax::Impl::Impl(InputIteratorBase &&it)
        : err_msg{}, data{parse(it, err_msg)}
    {
        if (!err_msg.empty()) {
            data.clear();
        }
    }


    Syntax::Syntax()
        : pimpl{new Impl}
    {
    }

    Syntax::Syntax(const std::string &src)
        : Syntax{src.begin(), src.end()}
    {
    }

    Syntax::Syntax(const char *src)
        : Syntax{src, src + std::strlen(src)}
    {
    }

    Syntax::Syntax(const Syntax &a)
        : pimpl{new Impl{*a.pimpl}}
    {
    }

    Syntax::Syntax(Syntax &&a)
        : pimpl{a.pimpl}
    {
        a.pimpl = nullptr;
    }

    Syntax::~Syntax() noexcept
    {
        delete pimpl;
    }

    Syntax &Syntax::operator=(const Syntax &a)
    {
        *pimpl = *a.pimpl;

        return *this;
    }

    Syntax &Syntax::operator=(Syntax &&a)
    {
        delete pimpl;
        pimpl = a.pimpl;
        a.pimpl = nullptr;

        return *this;
    }

    bool Syntax::add(const Syntax &a)
    {
        const auto &add_err = a.pimpl->err_msg;
        const bool good = add_err.empty();
        if (!good) {
            for (auto &err : add_err) {
                pimpl->err_msg.emplace_back(err);
            }
        }
        pimpl->data.add(DataSyntax(a.pimpl->data), pimpl->err_msg);
        return good;
    }

    bool Syntax::add(Syntax &&a)
    {
        const auto &&add_err = std::move(a.pimpl->err_msg);
        const bool good = add_err.empty();
        if (!good) {
            for (auto &&err : add_err) {
                pimpl->err_msg.emplace_back(std::move(err));
            }
        }
        pimpl->data.add(std::move(a.pimpl->data), pimpl->err_msg);
        return good;
    }

    bool Syntax::add(const std::string &src)
    {
        return add(src.begin(), src.end());
    }

    bool Syntax::add(const char *src)
    {
        return add(src, src + std::strlen(src));
    }

    const std::vector<std::string> &Syntax::get_error_message() const
    {
        return pimpl->err_msg;
    }

    void Syntax::clear_error_message()
    {
        pimpl->err_msg.clear();
    }

    void Syntax::clear()
    {
        pimpl->err_msg.clear();
        pimpl->data.clear();
    }


    Syntax::Syntax(InputIteratorBase &it)
        : pimpl{new Impl{it}}
    {
    }

    bool Syntax::add(InputIteratorBase &it)
    {
        const std::size_t prev_len{pimpl->err_msg.size()};
        DataSyntax data{parse(it, pimpl->err_msg)};
        const bool good = prev_len == pimpl->err_msg.size();
        if (good) {
            pimpl->data.add(std::move(data), pimpl->err_msg);
        }
        return good;
    }

    const DataSyntax &Syntax::get_syntax_data() const
    {
        return pimpl->data;
    }

    DataSyntax &&Syntax::move_syntax_data() &&
    {
        return std::move(pimpl->data);
    }

    std::vector<std::string> &&Syntax::move_error_message() &&
    {
        return std::move(pimpl->err_msg);
    }
}
