/** An abstraction layer for the input iterator for char.
    \file InputIterator.h
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
    \note It's assumed that the library users have no need to use InputIterator directly.
*/

#ifndef TPHRASE_COMMON_INPUTITERATOR_H_
#define TPHRASE_COMMON_INPUTITERATOR_H_

#ifdef __cpp_concepts
#include <concepts>
#include <iterator>
#include <type_traits>
#endif

#include "tphrase/common/TempRefHolder.h"

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
        \note The instance doesn't own something to be referred by the input iterator, so the users must keep the referred object alive while the instance is used.
        \note The instance doesn't own the iterators if it's constructed by the L-value references.
        \note It's assumed that the library users have no need to use an instance of this class directly.
    */
    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    class InputIterator : public InputIteratorBase {
    public:
        /** The constructor.
            \tparam T2 The type of an input iterator. The dereference of a value of T2 can be convertible to a value of char.
            \tparam S2 The type of the end for T2.
            \param [inout] p The iterator. (Universal reference; accessed by the reference or moved)
            \param [inout] e The end iterator. (Universal reference; accessed by the reference or moved)
        */
        template<typename T2, typename S2> REQUIRES_CharInputIteratorConcept(T2, S2)
        InputIterator(T2 &&p, S2 &&e);
        InputIterator() = delete;
        InputIterator(const InputIterator &a) = delete;
        InputIterator<T, S> &operator=(const InputIterator &a) = delete;
        virtual ~InputIterator() noexcept;
        virtual char operator*();
        virtual InputIterator<T, S> &operator++();
        virtual bool is_end() const;

    private:
        TempRefHolder<typename std::remove_reference<T>::type> cur; /**< The current position */
        TempRefHolder<typename std::remove_reference<S>::type> end; /**< The end position */
    };

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    template<typename T2, typename S2> REQUIRES_CharInputIteratorConcept(T2, S2)
    InputIterator<T, S>::InputIterator(T2 &&p, S2 &&e)
        : cur{std::forward<T2>(p)}, end{std::forward<S2>(e)}
    {
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    InputIterator<T, S>::~InputIterator() noexcept
    {
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    char InputIterator<T, S>::operator*()
    {
        return **cur;
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    InputIterator<T, S> &InputIterator<T, S>::operator++()
    {
        ++*cur;
        return *this;
    }

    template<typename T, typename S> REQUIRES_CharInputIteratorConcept(T, S)
    bool InputIterator<T, S>::is_end() const
    {
        return *cur == *end;
    }
}

#endif // TPHRASE_COMMON_INPUTITERATOR_H_
