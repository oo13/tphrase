/** An abstraction layer for the input iterator for char.
    \file InputIterator.h
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
    \note It's assumed that the library users have no need to use InputIterator directly.
*/

#ifndef TPHRASE_COMMON_INPUTITERATOR_H_
#define TPHRASE_COMMON_INPUTITERATOR_H_

#ifdef __cpp_concepts
#include <concepts>
#include <iterator>
#include <type_traits>
#endif

namespace tphrase {
    /** An abstraction class for an input iterator for char.
        \note The instance doesn't own something to be referred by the input iterator, so the users must keep the referred object alive until the instance is unused.
        \note It's assumed that the library users have no need to use an instance of this class directly.
    */
    class InputIteratorBase {
    public:
        /** The destructor. */
        virtual ~InputIteratorBase() noexcept {}
        /** The dereference operator.
            \return The value of the current position.
        */
        virtual char operator*() = 0;
        /** The forward operator.
            \return *this
        */
        virtual InputIteratorBase &operator++() = 0;
        /** Is the position at the end?
            \return the position is at the end.
        */
        virtual bool is_end() const = 0;
    };

#ifdef __cpp_concepts
    /** A concept expressing the input iterator for char.
        \tparam T The type of an input iterator. The dereference of a value of T can be convertible to a value of char.
        \tparam S The type of the end for T.
        \note It's used only when compiling on C++20 or later.
    */
    template <typename T, typename S>
    concept CharInputIteratorConcept = requires (T x) {
        {*x} -> std::convertible_to<char>;
        requires std::input_iterator<std::remove_reference_t<T>>;
        requires std::sentinel_for<std::remove_reference_t<S>, std::remove_reference_t<T>>;
    };
/** A requirement for the input iterator for char. */
#define REQUIRES_CharInputIteratorConcept(T, S) \
    requires CharInputIteratorConcept<T, S>
#else
#define REQUIRES_CharInputIteratorConcept(T, S)
#endif

    /** An abstraction layer class for an input iterator for char.
        \tparam T The type of an input iterator. The dereference of a value of T can be convertible to a value of char.
        \tparam S The type of the end for T.
        \note The instance doesn't own the iterators and something to be referred by the input iterator, so the users must keep the referred object alive while the instance is used.
        \note It's assumed that the library users have no need to use an instance of this class directly.
    */
    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    class InputIterator : public InputIteratorBase {
    public:
        /** The constructor.
            \param [inout] p The iterator.
            \param [inout] e The end iterator.
        */
        InputIterator(T &p, S &e);
        InputIterator() = delete;
        InputIterator(const InputIterator &a) = delete;
        InputIterator<T, S> &operator=(const InputIterator &a) = delete;
        virtual ~InputIterator() noexcept;
        virtual char operator*();
        virtual InputIterator<T, S> &operator++();
        virtual bool is_end() const;

    private:
        T &cur; /**< The current position */
        S &end; /**< The end position */
    };

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    InputIterator<T, S>::InputIterator(T &p, S &e)
        : cur{p}, end{e}
    {
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    InputIterator<T, S>::~InputIterator() noexcept
    {
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    char InputIterator<T, S>::operator*()
    {
        return *cur;
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    InputIterator<T, S> &InputIterator<T, S>::operator++()
    {
        ++cur;
        return *this;
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    bool InputIterator<T, S>::is_end() const
    {
        return cur == end;
    }
}

#endif // TPHRASE_COMMON_INPUTITERATOR_H_
