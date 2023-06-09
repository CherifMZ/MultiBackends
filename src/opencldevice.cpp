#include "device.hpp"

namespace cle
{

#if CLE_OPENCL

OpenCLDevice::OpenCLDevice(const cl_platform_id & platform, const cl_device_id & device)
  : clDevice(device)
  , clPlatform(platform)
{}

OpenCLDevice::~OpenCLDevice()
{
  if (isInitialized())
  {
    finalize();
  }
}

auto
OpenCLDevice::getType() const -> Device::Type
{
  return Device::Type::OPENCL;
}

auto
OpenCLDevice::initialize() -> void
{
  if (isInitialized())
  {
    std::cerr << "OpenCL device already initialized" << std::endl;
    return;
  }
  cl_int err = CL_SUCCESS;
  clContext = clCreateContext(nullptr, 1, &clDevice, nullptr, nullptr, &err);
  if (err != CL_SUCCESS)
  {
    std::cerr << "Failed to create OpenCL context" << std::endl;
    return;
  }
  clCommandQueue = clCreateCommandQueue(clContext, clDevice, 0, &err); // clCreateCommandQueue deprecated in OpenCL 2.0+
  if (err != CL_SUCCESS)
  {
    std::cerr << "Failed to create OpenCL command queue" << std::endl;
    return;
  }
  initialized = true;
}

auto
OpenCLDevice::finalize() -> void
{
  if (!isInitialized())
  {
    std::cerr << "OpenCL device not initialized" << std::endl;
    return;
  }
  this->finish();
  clReleaseContext(clContext);
  clReleaseCommandQueue(clCommandQueue);
  clReleaseDevice(clDevice);
  initialized = false;
}

auto
OpenCLDevice::finish() -> void
{
  if (!isInitialized())
  {
    std::cerr << "OpenCL device not initialized" << std::endl;
    return;
  }
  clFinish(clCommandQueue);
}

auto
OpenCLDevice::isInitialized() const -> bool
{
  return initialized;
}

auto
OpenCLDevice::getCLPlatform() const -> const cl_platform_id &
{
  return clPlatform;
}

auto
OpenCLDevice::getCLDevice() const -> const cl_device_id &
{
  return clDevice;
}

auto
OpenCLDevice::getCLContext() const -> const cl_context &
{
  return clContext;
}

auto
OpenCLDevice::getCLCommandQueue() const -> const cl_command_queue &
{
  return clCommandQueue;
}

auto
OpenCLDevice::getName() const -> std::string
{
  std::string vendor_name;
  vendor_name.reserve(256);
  clGetDeviceInfo(clDevice, CL_DEVICE_NAME, sizeof(char) * vendor_name.size(), vendor_name.data(), nullptr);
  return vendor_name;
}

auto
OpenCLDevice::getInfo() const -> std::string
{
  std::ostringstream result;
  std::string        version;
  cl_device_type     type;
  cl_uint            compute_units;
  size_t             global_mem_size;
  size_t             max_mem_size;

  // Get device information
  version.reserve(256);
  const auto & name = getName();
  clGetDeviceInfo(clDevice, CL_DEVICE_VERSION, sizeof(char) * version.size(), version.data(), nullptr);
  clGetDeviceInfo(clDevice, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, nullptr);
  clGetDeviceInfo(clDevice, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, nullptr);
  clGetDeviceInfo(clDevice, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size_t), &global_mem_size, nullptr);
  clGetDeviceInfo(clDevice, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(size_t), &max_mem_size, nullptr);

  // Print device information to output string
  result << name << " (" << std::string(version) << ")\n";
  switch (type)
  {
    case CL_DEVICE_TYPE_CPU:
      result << "\tType: CPU\n";
      break;
    case CL_DEVICE_TYPE_GPU:
      result << "\tType: GPU\n";
      break;
    default:
      result << "\tType: Unknown\n";
      break;
  }
  result << "\tCompute Units: " << compute_units << '\n';
  result << "\tGlobal Memory Size: " << (global_mem_size / 1000000) << " MB\n";
  result << "\tMaximum Object Size: " << (max_mem_size / 1000000) << " MB\n";
  return result.str();
}

auto
OpenCLDevice::getCache() -> std::map<std::string, cl_program> &
{
  return this->cache;
}

#endif // CLE_OPENCL

} // namespace cle
