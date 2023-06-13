#ifndef __INCLUDE_TIER1_HPP
#define __INCLUDE_TIER1_HPP

#include "array.hpp"
#include "backend.hpp"
#include "execution.hpp"


namespace cle::tier1
{

using ConstantList = std::vector<std::pair<std::string, int>>;
using KernelInfo = std::pair<std::string, std::string>;
using RangeArray = std::array<size_t, 3>;

auto
execute_separable_func(const Array &     src,
                       const Array &     dst,
                       const float &     sigma,
                       const int &       radius,
                       const Device::Pointer & device) -> void;
auto
gaussian_blur_func(const Array &     src,
                   const Array &     dst,
                   const float &     sigma_x,
                   const float &     sigma_y,
                   const float &     sigma_z,
                   const Device::Pointer & device) -> void;

auto
absolute_func(const Array & src, const Array & dst, const DevicePtr & device) -> void;

} // namespace cle::tier1

#endif // __INCLUDE_TIER1_HPP
