/** A temporary reference holder for the universal reference.
    \file TempRefHolder.h
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
    \note It's assumed that the library users have no need to use TempRefHolder directly.
*/

#ifndef TPHRASE_COMMON_TEMPREFHOLDER_H_
#define TPHRASE_COMMON_TEMPREFHOLDER_H_

#include <utility>

namespace tphrase {

    /** A temporary reference holder for the universal reference.
        \tparam T The type of the object that referred by the reference.
        \note The instance doesn't own something to be referred by the reference, so the users must keep the referred object alive while the instance is used.
        \note The instance doesn't own the reference if it's constructed by the L-value references.
        \note It's assumed that the library users have no need to use an instance of this class directly.
    */
    template<typename T>
    class TempRefHolder {
    public:
        /** Construct from a L-value reference.
            \param [in] a The L-value reference to hold the pointer.
        */
        explicit TempRefHolder(T& a);
        /** Construct from a R-value reference.
            \param [in] a The R-value reference to move.
        */
        explicit TempRefHolder(T&& a);
        TempRefHolder(TempRefHolder &&a) = delete;
        TempRefHolder &operator=(const TempRefHolder& a) = delete;
        TempRefHolder &operator=(TempRefHolder&& a) = delete;
        /** The destructor. */
        ~TempRefHolder() noexcept;
        TempRefHolder() = delete;
        TempRefHolder(const TempRefHolder &a) = delete;

        /** Get the reference.
            \return The reference.
        */
        T &operator*();
        /** Get the const reference.
            \return The const reference.
        */
        const T &operator*() const;

    private:
        T* v; /**< The pointer to the moved reference. */
        T* p; /**< The pointer to the reference. */
    };

    template<typename T>
    TempRefHolder<T>::TempRefHolder(T& a)
        : v{nullptr}, p{&a}
    {
    }

    template<typename T>
    TempRefHolder<T>::TempRefHolder(T&& a)
        : v{new T{std::move(a)}}, p{v}
    {
    }

    template<typename T>
    TempRefHolder<T>::~TempRefHolder() noexcept
    {
        delete v;
    }

    template<typename T>
    T &TempRefHolder<T>::operator*()
    {
        return *p;
    }

    template<typename T>
    const T &TempRefHolder<T>::operator*() const
    {
        return *p;
    }
}

#endif // TPHRASE_COMMON_TEMPREFHOLDER_H_
