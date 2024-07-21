/** Translatable Phrase Generator
    \file Generator.cpp
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

#include "tphrase/common/ext_context.h"
#include "tphrase/Generator.h"
#include "DataGsubs.h"
#include "DataPhrase.h"
#include "random.h"

namespace {
    /** The predefined empty external context. */
    const tphrase::ExtContext_t empty_context;

    /** The default start condition. */
    const std::string default_start_condition{"main"};
}

namespace tphrase {

    /** The type of the private data of the class Generator. */
    struct Generator::Impl {
        std::string err_msg; /**< The holder of the error messages. */
        DataPhrase data; /**< The data structure for the phrase generator. */

        /** The default constructor. */
        Impl() = default;
        /** The constructor to copy error messages.
            \param [in] err The error messages.
        */
        Impl(const std::string &err);
        /** The constructor to move error messages.
            \param [inout] err The error messages. (moved)
        */
        Impl(std::string &&err);
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

    Generator::Impl::Impl(const std::string &err)
        : err_msg{err}, data{}
    {
    }

    Generator::Impl::Impl(std::string &&err)
        : err_msg{std::move(err)}, data{}
    {
    }

    Generator::Generator()
        : pimpl{new Impl}
    {
    }

    Generator::Generator(const Syntax &syntax)
        : Generator{syntax, default_start_condition}
    {
    }

    Generator::Generator(Syntax &&syntax)
        : Generator{std::move(syntax), default_start_condition}
    {
    }

    Generator::Generator(const Syntax &syntax,
                         const std::string &start_condition)
        : pimpl{new Impl{syntax.get_error_message()}}
    {
        if (pimpl->err_msg.empty()) {
            pimpl->data.add(syntax.get_syntax_data(),
                            start_condition,
                            pimpl->err_msg);
        }
    }

    Generator::Generator(Syntax &&syntax, const std::string &start_condition)
        : pimpl{new Impl{std::move(syntax).move_error_message()}}
    {
        if (pimpl->err_msg.empty()) {
            pimpl->data.add(std::move(syntax).move_syntax_data(),
                            start_condition,
                            pimpl->err_msg);
        }
    }

    Generator::Generator(const Generator &a)
        : pimpl{new Impl{*a.pimpl}}
    {
    }

    Generator::Generator(Generator &&a)
        : pimpl{a.pimpl}
    {
        a.pimpl = nullptr;
    }

    Generator::~Generator() noexcept
    {
        delete pimpl;
    }

    Generator &Generator::operator=(const Generator &a)
    {
        *pimpl = *a.pimpl;

        return *this;
    }

    Generator &Generator::operator=(Generator &&a)
    {
        delete pimpl;
        pimpl = a.pimpl;
        a.pimpl = nullptr;

        return *this;
    }

    std::string Generator::generate() const
    {
        return pimpl->data.generate(empty_context);
    }

    std::string Generator::generate(const ExtContext_t &ext_context) const
    {
        return pimpl->data.generate(ext_context);
    }

    SyntaxID_t Generator::add(const Syntax &syntax)
    {
        return add(syntax, default_start_condition);
    }

    SyntaxID_t Generator::add(Syntax &&syntax)
    {
        return add(std::move(syntax), default_start_condition);
    }

    SyntaxID_t Generator::add(const Syntax &syntax,
                              const std::string &start_condition)
    {
        const std::size_t prev_len{pimpl->err_msg.size()};
        pimpl->err_msg += syntax.get_error_message();
        if (prev_len != pimpl->err_msg.size()) {
            return 0;
        }
        return pimpl->data.add(syntax.get_syntax_data(),
                               start_condition,
                               pimpl->err_msg);
    }

    SyntaxID_t Generator::add(Syntax &&syntax,
                              const std::string &start_condition)
    {
        const std::size_t prev_len{pimpl->err_msg.size()};
        pimpl->err_msg += std::move(syntax).move_error_message();
        if (prev_len != pimpl->err_msg.size()) {
            return 0;
        }
        return pimpl->data.add(std::move(syntax).move_syntax_data(),
                               start_condition,
                               pimpl->err_msg);
    }

    bool Generator::remove(SyntaxID_t id)
    {
        return pimpl->data.remove(id);
    }

    const std::string &Generator::get_error_message() const
    {
        return pimpl->err_msg;
    }

    void Generator::clear_error_message()
    {
        pimpl->err_msg.clear();
    }

    void Generator::clear()
    {
        pimpl->err_msg.clear();
        pimpl->data.clear();
    }

    void Generator::equalize_chance(const bool enable)
    {
        pimpl->data.equalize_chance(enable);
    }

    std::size_t Generator::get_number_of_syntax() const
    {
        return pimpl->data.get_number_of_syntax();
    }

    std::size_t Generator::get_combination_number() const
    {
        return pimpl->data.get_combination_number();
    }

    double Generator::get_weight() const
    {
        return pimpl->data.get_weight();
    }

    void Generator::set_gsub_function_creator(const GsubFuncCreator_t &creator)
    {
        DataGsubs::set_gsub_function_creator(creator);
    }

    GsubFuncCreator_t Generator::get_gsub_function_creator()
    {
        return DataGsubs::get_gsub_function_creator();
    }

    void Generator::set_random_function(const RandomFunc_t &rand)
    {
        random = rand;
    }

    RandomFunc_t Generator::get_random_function()
    {
        return random;
    }
}
