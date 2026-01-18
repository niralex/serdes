#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief Iterator for descriptor list

    @details

    @todo

    @author Niraleks
*/
//-----------------------------------------------------------------------------
#include <iterator>
#include "Descriptor.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    class DescriptorIterator
    {
        private:
            const uint8_t *_id;

        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = int;
            using value_type = Descriptor;
            using pointer = const Descriptor *;
            using reference = const Descriptor &;

            constexpr
            DescriptorIterator(const uint8_t *id) : _id(id) {}

            constexpr inline
            DescriptorIterator &operator++()
            {
                  _id = _id + Descriptor(_id).GetSize();
                  return *this;
            }

            constexpr inline
            DescriptorIterator operator++(int)
            {
                DescriptorIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            constexpr inline
            bool operator==(const DescriptorIterator &it) const
            {
                return _id == it._id;
            }

            constexpr inline
            bool operator!=(const DescriptorIterator &it) const
            {
                return _id != it._id;
            }

            constexpr inline
            Descriptor operator*()const
            {
                return Descriptor(_id);
            }

    };

} // serdes

//------------------------------------------------------------------------------
