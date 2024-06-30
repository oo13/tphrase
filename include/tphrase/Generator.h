/** Translatable Phrase Generator
    \file Generator.h
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

#ifndef TPHRASE_GENERATOR_H_
#define TPHRASE_GENERATOR_H_

#include <cstddef>
#include <string>
#include <utility>

#include "common/InputIterator.h"
#include "common/ext_context.h"
#include "common/gsub_func.h"
#include "common/random_func.h"

/** The namespace for Translatable Phrase Generator. */
namespace tphrase {
    class Syntax;

    /** A translatable phrase generator class */
    class Generator {
    public:
        /** The default constructor. */
        Generator();
        /** The constructor to create an instance that has a syntax.
            \param [in] syntax The phrase syntax.
            \note Only the phrase syntax that contains the nonterminal "main" can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note get_error_message() will return an empty string if no errors are detected.
            \note An empty generator is created if some errors are detected.
        */
        explicit Generator(const Syntax &syntax);
        /** The constructor to create an instance that has a syntax.
            \param [inout] syntax The phrase syntax. (moved)
            \note Only the phrase syntax that contains the nonterminal "main" can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note All the errors in syntax is moved.
            \note get_error_message() will return an empty string if no errors are detected and moved.
            \note An empty generator is created if some errors are detected.
        */
        explicit Generator(Syntax &&syntax);
        /** The constructor to create an instance that has a syntax.
            \param [in] syntax The phrase syntax.
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \note Only the phrase syntax that contains the start condition can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note get_error_message() will return an empty string if no errors are detected.
            \note An empty generator is created if some errors are detected.
        */
        Generator(const Syntax &syntax, const std::string &start_condition);
        /** The constructor to create an instance that has a syntax.
            \param [inout] syntax The phrase syntax. (moved)
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \note Only the phrase syntax that contains the start condition can add.
            \note The recursive reference to a nonterminal is not allowed.
            \note All the errors in syntax is moved.
            \note get_error_message() will return an empty string if no errors are detected and moved.
            \note An empty generator is created if some errors are detected.
        */
        Generator(Syntax &&syntax, const std::string &start_condition);
        /** The copy constructor.
            \param [in] a The source.
        */
        Generator(const Generator &a);
        /** The move constructor.
            \param [inout] a The source. (moved)
        */
        Generator(Generator &&a);

        /** The destructor. */
        ~Generator() noexcept;


        /** The assignment.
            \param [in] a The source.
            \return *this
        */
        Generator &operator=(const Generator &a);
        /** The move assignment.
            \param [inout] a The source. (moved)
            \return *this
        */
        Generator &operator=(Generator &&a);

        /** Generate a phrase.
            \return A phrase.
            \note The empty generator returns "nil".
        */
        std::string generate() const;
        /** Generate a phrase.
            \param [in] ext_context The external context that has some nonterminals and the substitutions.
            \return A phrase.
            \note The empty generator returns "nil".
        */
        std::string generate(const ExtContext_t &ext_context) const;

        /** Add a phrase syntax.
            \param [in] syntax The phrase syntax to be copied and added.
            \return true if no errors are detected.
            \note Only the phrase syntax that contains the nonterminal "main" can be added.
            \note The recursive reference to a nonterminal is not allowed.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(const Syntax &syntax);
        /** Add a phrase syntax.
            \param [inout] syntax The phrase syntax to be added. (moved)
            \return true if no errors are detected and added.
            \note Only the phrase syntax that contains the nonterminal "main" can be added.
            \note The recursive reference to a nonterminal is not allowed.
            \note All the errors in syntax is added.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(Syntax &&syntax);
        /** Add a phrase syntax.
            \param [in] syntax The phrase syntax to be copied and added.
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \return true if no errors are detected.
            \note Only the phrase syntax that contains the start condition can be added.
            \note The recursive reference to a nonterminal is not allowed.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(const Syntax &syntax, const std::string &start_condition);
        /** Add a phrase syntax.
            \param [inout] syntax The phrase syntax to be added. (moved)
            \param [in] start_condition The name of the nonterminal where is the start condition.
            \return true if no errors are detected and added.
            \note Only the phrase syntax that contains the start condition can be added.
            \note The recursive reference to a nonterminal is not allowed.
            \note All the errors in syntax is added.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(Syntax &&syntax, const std::string &start_condition);

        /** Get the error messages.
            \return The error messages that have been generated after creating the instance or clearing the previous error messages.
        */
        const std::string &get_error_message() const;

        /** Clear the previous error messages. */
        void clear_error_message();
        /** Clear the syntaxes and the error messages. */
        void clear();

        /** Equalize the chance to select each phrase syntax.
            \param [in] enable equalized if enable is true. If not, the chance depends on the weight of the phrase syntax. (Default)
        */
        void equalize_chance(bool enable = true);

        /** Get the number of the syntaxes in the instance.
            \return The number of the syntaxes in the instance.
        */
        std::size_t get_number_of_syntax() const;
        /** Get the sum of the weight of the syntaxes in the instance.
            \return The sum of the weight of the syntaxes in the instance.
        */
        double get_weight() const;
        /** Get the number of the possible phrases generated by the instance.
            \return The the number of the possible phrases generated by the instance.
        */
        std::size_t get_combination_number() const;

        /** Set the function to create the gsub functions.
            \param [in] creator The function to create the gsub functions.
            \note It's used when parsing the source text.
            \note The default gsub creator uses std::regex that is no character encoding support.
            \note It causes a parse error that the creator function throw an std::runtime_error at creating a gsub function. The exception handles and suppresses by the parser.
            \note The generator doesn't catch the exception that the created gsub function throws. (The default gsub function doesn't throw the std::runtime_error and generates an error string.)
            \note You should tell the phrase creators that you changed the gsub creator function because it affects the grammar of the gsub.
        */
        static void set_gsub_function_creator(const GsubFuncCreator_t &creator);
        /** Get the current gsub function creator.
            \return The current gsub function creator.
        */
        static GsubFuncCreator_t get_gsub_function_creator();

        /** Set the function to create the random numbers.
            \param [in] rand The random function that generate a real value [ 0.0 1.0).
            \note It's used when generating the phrase.
            \note The default random function uses std::default_random_engine and std::uniform_real_distribution.
            \note This function is useful for a unit test.
        */
        static void set_random_function(const RandomFunc_t &rand);
        /** Get the current random function.
            \return The current random function.
        */
        static RandomFunc_t get_random_function();

    private:
        struct Impl;
        /** The private data. */
        Impl *pimpl;
    };


    class DataSyntax;

    /** The phrase syntax class

        The phrase syntax consists of assignments that define a nonterminal assigned to a production rule.
    */
    class Syntax {
        friend class Generator;
    public:
        /** The default constructor. */
        Syntax();
        /** The constructor to create an instance that has a syntax.
            \tparam T The type of an input iterator. The dereference of a value of T can be convertible to a value of char.
            \tparam S The type of the end for T.
            \param [inout] begin The iterator to point the beginning of the source text of a phrase syntax. (Universal reference; accessed by the reference or moved)
            \param [inout] end The end iterator. (Universal reference; accessed by the reference or moved)
            \note The instance doesn't use begin, end, and something referred by begin after returning from the constructor.
            \note Some parse errors may be detected if src has errors.
            \note get_error_message() will return an empty string if no errors are detected.
            \note An empty phrase syntax is created if some errors are detected.
            \attention std::istream_iterator skips any white spaces by default, so you should configure the target input stream not to skip the spaces (stream.unsetf(std::ios_base::skipws)) if you want to use an istream_iterator.
        */
        template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
        Syntax(T &&begin, S &&end);
        /** The constructor to create an instance that has a syntax.
            \param [in] src The source text of a phrase syntax.
            \note Some parse errors may be detected if src has errors.
            \note get_error_message() will return an empty string if no errors are detected.
            \note An empty phrase syntax is created if some errors are detected.
        */
        Syntax(const std::string &src);
        /** The constructor to create an instance that has a syntax.
            \param [in] src The source text of a phrase syntax.
            \note Some parse errors may be detected if src has errors.
            \note get_error_message() will return an empty string if no errors are detected.
            \note An empty phrase syntax is created if some errors are detected.
        */
        Syntax(const char *src);
        /** The copy constructor.
            \param [in] a The source.
        */
        Syntax(const Syntax &a);
        /** The move constructor.
            \param [inout] a The source. (moved)
        */
        Syntax(Syntax &&a) noexcept;

        /** The destructor. */
        ~Syntax() noexcept;

        /** The assignment.
            \param [in] a The source.
            \return *this
        */
        Syntax &operator=(const Syntax &a);
        /** The move constructor.
            \param [inout] a The source. (moved)
        */
        Syntax &operator=(Syntax &&a) noexcept;

        /** Add the assignments from a source Syntax.
            \param [in] a The source syntax.
            \return true if no errors are added.
            \note The production rules for the existing nonterminals is overwritten if the source Syntax has the assignment for the same nonterminal.
            \note The function causes no errors, but adds  the error messages in the source syntax into this.
        */
        bool add(const Syntax &a);
        /** Add the assignments from a source Syntax.
            \param [inout] a The source syntax. (moved)
            \return true if no errors are added.
            \note The production rules for the existing nonterminals is overwritten if the source Syntax has the assignment for the same nonterminal.
            \note The function causes no errors, but adds  the error messages in the source syntax into this.
        */
        bool add(Syntax &&a);
        /** Add the assignments from a phrase syntax.
            \tparam T The type of an input iterator. The dereference of a value of T can be convertible to a value of char.
            \tparam S The type of the end for T.
            \param [inout] begin The iterator to point the beginning of the source text of a phrase syntax. (Universal reference; accessed by the reference or moved)
            \param [inout] end The end iterator. (Universal reference; accessed by the reference or moved)
            \return true if no errors are detected.
            \note The instance doesn't use begin, end, and something referred by begin after returning from the function.
            \note The production rules for the existing nonterminals is overwritten if the source Syntax has the assignment for the same nonterminal.
            \note Some parse errors may be detected if src has errors.
            \note No phrase syntax is added if some errors are detected.
            \attention std::istream_iterator skips any white spaces by default, so you should configure the target input stream not to skip the spaces (stream.unsetf(std::ios_base::skipws)) if you want to use an istream_iterator.
        */
        template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
        bool add(T &&begin, S &&end);
        /** Add the assignments from a phrase syntax.
            \param [in] src The source text of a phrase syntax.
            \return true if no errors are detected.
            \note The production rules for the existing nonterminals is overwritten if the source Syntax has the assignment for the same nonterminal.
            \note Some parse errors may be detected if src has errors.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(const std::string &src);
        /** Add the assignments from a phrase syntax.
            \param [in] src The source text of a phrase syntax.
            \return true if no errors are detected.
            \note The production rules for the existing nonterminals is overwritten if the source Syntax has the assignment for the same nonterminal.
            \note Some parse errors may be detected if src has errors.
            \note No phrase syntax is added if some errors are detected.
        */
        bool add(const char *src);

        /** Get the error messages.
            \return The error messages that have been generated after creating the instance or clearing the previous error messages.
        */
        const std::string &get_error_message() const;

        /** Clear the previous error messages. */
        void clear_error_message();
        /** Clear the syntaxes and the error messages. */
        void clear();

    private:
        /** The constructor to create an instance that has a syntax.
            \param [inout] it InputIterator of the source text of a phrase syntax.
            \note The instance doesn't use 'it' and something referred by 'it' after returning from the constructor.
            \note Some parse errors may be detected if src has errors.
            \note get_error_message() will return an empty string if no errors are detected.
        */
        Syntax(InputIteratorBase &it);
        /** Add the assignments from a phrase syntax.
            \param [inout] it InputIterator of the source text of a phrase syntax.
            \return true if no errors are detected.
            \note The instance doesn't use 'it' and something referred by 'it' after returning from the constructor.
            \note Some parse errors may be detected if src has errors.
        */
        bool add(InputIteratorBase &it);

        /** The function used by Generator.
            \return An instance of a type not to be published for the library users.
        */
        const DataSyntax &get_syntax_data() const;
        /** The function used by Generator.
            \return An instance of a type not to be published for the library users.
        */
        DataSyntax &&move_syntax_data();

        /** The function used by Generator.
            \return The error messages that have been generated after creating the instance or clearing the previous error messages.
        */
        std::string &&move_error_message();

    private:
        struct Impl;
        /** The private data. */
        Impl *pimpl;
    };

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    Syntax::Syntax(T &&begin, S &&end)
    {
        InputIterator<T, S> it{std::forward<T>(begin), std::forward<S>(end)};
        Syntax a{it};
        pimpl = a.pimpl;
        a.pimpl = nullptr;
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    bool Syntax::add(T &&begin, S &&end)
    {
        InputIterator<T, S> it{std::forward<T>(begin), std::forward<S>(end)};
        return add(it);
    }
}

#endif // TPHRASE_GENERATOR_H_
