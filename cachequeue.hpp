/*
 * =====================================================================================
 *
 *       Filename: cachequeue.hpp
 *        Created: 02/25/2016 01:01:40
 *    Description:
 *
 *                 fixed size linear cache queue
 *                 may use Boost.Circular Buffer in furture
 *
 *                 1. support push/pop at head and back
 *                    push to a full cache queue would override the data
 *                        a. void PushHead(ValueType &&)/PopHead()
 *                        b. void PushBack(ValueType &&)/PopBack()
 *
 *                 2. support access the first and last element
 *                    undefined behavior if current cache queue is zero-capacity/zero-length
 *                        a. ValueType& Head()
 *                        b. ValueType& Back()
 *
 *                 3. to iterate the queue:
 *                    
 *                    for(int nIndex = 0; nIndex < stQ.Length(); ++nIndex){
 *                       func(stQ[nIndex]);
 *                    }
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: g++ -std=c++11
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#ifndef __CACHE_QUEUE_HPP__
#define __CACHE_QUEUE_HPP__

#include <array>
#include <algorithm>

template<typename ValueType, size_t CacheSize> class CacheQueue final
{
    private:
        std::array<ValueType, CacheSize> m_CircleQ;

    private:
        size_t m_Head;
        size_t m_CurrSize;

    public:
        CacheQueue()
            : m_CircleQ()
            , m_Head(0)
            , m_CurrSize(0)
        {}

    public:
        ~CacheQueue() = default;

    public:
        constexpr size_t Capacity() const
        {
            return m_CircleQ.size();
        }

    public:
        size_t Length() const
        {
            return Size();
        }

    public:
        bool Empty() const
        {
            return m_CurrSize == 0;
        }

        bool Full() const
        {
            return m_CurrSize == Capacity();
        }

    public:
        ValueType &Head()
        {
            return m_CircleQ[m_Head];
        }

        const ValueType &Head() const
        {
            return m_CircleQ[m_Head];
        }

        ValueType &Back()
        {
            return m_CircleQ[(m_Head + Length() + Capacity() - 1) % Capacity()];
        }

        const ValueType &Back() const
        {
            return m_CircleQ[(m_Head + Length() + Capacity() - 1) % Capacity()];
        }

    public:
        template<typename... U> void PushHead(U&&... u)
        {
            if(Empty()){
                m_Head       = 0;
                m_CircleQ[0] = ValueType(std::forward<U>(u)...);
                m_CurrSize   = 1;
            }else{
                m_Head = ((m_Head + Capacity() - 1) % Capacity());
                m_CircleQ[m_Head] = ValueType(std::forward<U>(u)...);
                m_CurrSize = std::min<size_t>(m_CurrSize + 1, Capacity());
            }
        }

        template<typename... U> void PushBack(U&&... u)
        {
            if(Empty()){
                m_Head       = 0;
                m_CircleQ[0] = ValueType(std::forward<U>(u)...);
                m_CurrSize   = 1;
            }else if(Full()){
                m_CircleQ[m_Head] = ValueType(std::forward<U>(u)...);
                m_Head = (m_Head + 1) % Capacity();
            }else{
                m_CircleQ[(m_Head + Length()) % Capacity()] = ValueType(std::forward<U>(u)...);
                m_CurrSize = std::min<size_t>(m_CurrSize + 1, Capacity());
            }
        }

    public:
        void PopHead()
        {
            if(!Empty()){
                m_Head = (m_Head + 1) % Capacity();
                m_CurrSize--;
            }
        }

        void PopBack()
        {
            if(!Empty()){
                m_CurrSize--;
            }
        }

    public:
        void Clear()
        {
            m_Head = 0;
            m_CurrSize = 0;
        }

    public:
        void Reset()
        {
            std::rotate(m_CircleQ.begin(), m_CircleQ.end(), m_CircleQ.begin() + m_Head);
        }

    public:
        void Rotate(int nRotate)
        {
            if(!Empty()){
                if(auto nCount = std::abs<int>(nRotate)){
                    if(nRotate > 0){
                        for(auto nIndex = 0; nIndex < nCount; ++nIndex){
                            auto stHead = Head();
                            PopHead();
                            PushBack(stHead);
                        }
                    }else{
                        for(auto nIndex = 0; nIndex < nCount; ++nIndex){
                            auto stBack = Back();
                            PopBack();
                            PushHead(stBack);
                        }
                    }
                }
            }
        }

    public:
        ValueType & operator [] (size_t nIndex)
        {
            return m_CircleQ[(m_Head + nIndex + Capacity()) % Capacity()];
        }

        const ValueType & operator [] (size_t nIndex) const
        {
            return m_CircleQ[(m_Head + nIndex + Capacity()) % Capacity()];
        }

    public:
        size_t HeadOff() const
        {
            return m_Head;
        }

        size_t BackOff() const
        {
            return (m_Head + Length() - 1 + Capacity()) % Capacity();
        }
};

#endif
