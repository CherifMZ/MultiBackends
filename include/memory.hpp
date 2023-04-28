#ifndef __INCLUDE_MEMORY_HPP
#define __INCLUDE_MEMORY_HPP

#include "array.hpp"
#include "backend.hpp"
#include "device.hpp"

namespace cle
{
    namespace memory
    {
        using DevicePtr = std::shared_ptr<cle::Device>;

        template <typename T>
        auto create(size_t width, size_t height, size_t depth, const DevicePtr &device) -> Array
        {
            return Array{width, height, depth, Array::toType<T>(), device};
        }

        auto create_like(const Array &arr) -> Array
        {
            return Array{arr.width(), arr.height(), arr.depth(), arr.dtype(), arr.device()};
        }

        template <typename T>
        auto push(const T *host_data, size_t width, size_t height, size_t depth, const DevicePtr &device) -> Array
        {
            return Array{width, height, depth, Array::toType<T>(), host_data, device};
        }

        template <typename T>
        auto pull(const Array &arr, T *host_arr) -> void
        {
            arr.read(host_arr);
        }

        template <typename T>
        auto copy(const Array &src) -> const Array &
        {
            Array dst{src.width(), src.height(), src.depth(), Array::toType<T>(), src.device_};
            src.copy(dst);
            return dst;
        }

    } // namespace memory
} // namespace cle

#endif // __INCLUDE_MEMORY_HPP
