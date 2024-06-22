/** Phrase Syntax
    \file Syntax.cpp
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
#include <cstring>
#include <utility>

#include "tphrase/Generator.h"
#include "DataSyntax.h"
#include "parse.h"

namespace tphrase {

    /** The type of the private data of the class Syntax. */
    struct Syntax::Impl {
        std::string err_msg; /**< The holder of the error messages. */
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
    }

    Syntax::Impl::Impl(InputIteratorBase &&it)
        : err_msg{}, data{parse(it, err_msg)}
    {
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

    Syntax::Syntax(Syntax &&a) noexcept
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

    Syntax &Syntax::operator=(Syntax &&a) noexcept
    {
        delete pimpl;
        pimpl = a.pimpl;
        a.pimpl = nullptr;

        return *this;
    }

    bool Syntax::add(const Syntax &a)
    {
        const bool good = a.pimpl->err_msg.empty();
        if (!good) {
            pimpl->err_msg += a.pimpl->err_msg;
        }
        pimpl->data.add(DataSyntax(a.pimpl->data));
        return good;
    }

    bool Syntax::add(Syntax &&a)
    {
        const bool good = a.pimpl->err_msg.empty();
        if (!good) {
            pimpl->err_msg += a.pimpl->err_msg;
        }
        pimpl->data.add(std::move(a.pimpl->data));
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

    const std::string &Syntax::get_error_message() const
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
        pimpl->data.add(std::move(data));

        return prev_len == pimpl->err_msg.size();
    }

    const DataSyntax &Syntax::get_syntax_data() const
    {
        return pimpl->data;
    }

    DataSyntax &&Syntax::move_syntax_data()
    {
        return std::move(pimpl->data);
    }

    std::string &&Syntax::move_error_message()
    {
        return std::move(pimpl->err_msg);
    }
}
