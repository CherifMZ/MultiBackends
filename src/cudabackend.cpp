#include "backend.hpp"
#include "cle_preamble_cu.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <nvrtc.h>

namespace cle
{

auto
CUDABackend::getDevices(const std::string & type) const -> std::vector<Device::Pointer>
{
#if CLE_CUDA
  int  deviceCount;
  auto error = cudaGetDeviceCount(&deviceCount);
  if (error != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to get CUDA device count.");
  }
  std::vector<Device::Pointer> devices;
  for (int i = 0; i < deviceCount; i++)
  {
    devices.push_back(std::make_shared<CUDADevice>(i));
  }
  return devices;
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::getDevice(const std::string & name, const std::string & type) const -> Device::Pointer
{
#if CLE_CUDA
  auto devices = getDevices(type);
  auto ite = std::find_if(devices.begin(), devices.end(), [&name](const Device::Pointer & dev) {
    return dev->getName().find(name) != std::string::npos;
  });
  if (ite != devices.end())
  {
    return std::move(*ite);
  }
  if (!devices.empty())
  {
    return std::move(devices.back());
  }
  return nullptr;
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::getDevicesList(const std::string & type) const -> std::vector<std::string>
{
#if CLE_CUDA
  auto                     devices = getDevices(type);
  std::vector<std::string> deviceList;
  for (int i = 0; i < devices.size(); i++)
  {
    deviceList.emplace_back(devices[i]->getName());
  }
  return deviceList;
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::getType() const -> Backend::Type
{
  return Backend::Type::CUDA;
}

auto
CUDABackend::allocateMemory(const Device::Pointer & device, const size_t & size, void ** data_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaMalloc(data_ptr, size);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to allocate CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::allocateMemory(const Device::Pointer & device,
                            const size_t &          width,
                            const size_t &          height,
                            const size_t &          depth,
                            const dType &           dtype,
                            void **                 data_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  cudaExtent     extent = make_cudaExtent(width * toBytes(dtype), height, depth);
  cudaPitchedPtr devPtr = make_cudaPitchedPtr(nullptr, width * toBytes(dtype), width, height);
  err = cudaMalloc3D(&devPtr, extent);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to allocate CUDA memory.");
  }
  *data_ptr = devPtr.ptr;
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::freeMemory(const Device::Pointer & device, const mType & mtype, void ** data_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaFree(*data_ptr);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to free CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::writeMemory(const Device::Pointer & device,
                         void **                 data_ptr,
                         const size_t &          size,
                         const void *            host_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaMemcpy(*data_ptr, host_ptr, size, cudaMemcpyHostToDevice);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to write CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::writeMemory(const Device::Pointer & device,
                         void **                 data_ptr,
                         const size_t &          width,
                         const size_t &          height,
                         const size_t &          depth,
                         const size_t &          bytes,
                         const void *            host_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }

  if (depth > 1)
  {
    cudaMemcpy3DParms copyParams = { nullptr };
    copyParams.srcPtr.ptr = const_cast<void *>(host_ptr);
    copyParams.srcPtr.pitch = width * bytes;
    copyParams.srcPtr.xsize = width;
    copyParams.srcPtr.ysize = height;
    copyParams.dstPtr = make_cudaPitchedPtr(*data_ptr, width * bytes, width, height);
    copyParams.extent = make_cudaExtent(width * bytes, height, depth);
    copyParams.kind = cudaMemcpyHostToDevice;

    cudaMemcpy3D(&copyParams);
  }
  else if (height > 1)
  {
    cudaMemcpy2D(*data_ptr, width * bytes, host_ptr, width * bytes, width, height, cudaMemcpyHostToDevice);
  }
  else
  {
    cudaMemcpy(*data_ptr, host_ptr, width * bytes, cudaMemcpyHostToDevice);
  }

  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to write CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::readMemory(const Device::Pointer & device,
                        const void **           data_ptr,
                        const size_t &          size,
                        void *                  host_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaMemcpy(host_ptr, *data_ptr, size, cudaMemcpyDeviceToHost);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to read CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::readMemory(const Device::Pointer & device,
                        const void **           data_ptr,
                        const size_t &          width,
                        const size_t &          height,
                        const size_t &          depth,
                        const size_t &          bytes,
                        void *                  host_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  if (depth > 1)
  {
    cudaMemcpy3DParms copyParams = { nullptr };
    copyParams.srcPtr = make_cudaPitchedPtr(const_cast<void *>(*data_ptr), width * bytes, width, height);
    copyParams.srcPos = make_cudaPos(0, 0, 0);
    copyParams.dstPtr.ptr = host_ptr;
    copyParams.dstPtr.pitch = width * bytes;
    copyParams.dstPtr.xsize = width;
    copyParams.dstPtr.ysize = height;
    copyParams.extent = make_cudaExtent(width * bytes, height, depth);
    copyParams.kind = cudaMemcpyDeviceToHost;
    cudaMemcpy3D(&copyParams);
  }
  else if (height > 1)
  {
    cudaMemcpy2D(host_ptr, width * bytes, *data_ptr, width * bytes, width, height, cudaMemcpyDeviceToHost);
  }
  else
  {
    cudaMemcpy(host_ptr, *data_ptr, width * bytes, cudaMemcpyDeviceToHost);
  }
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to read CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::copyMemory(const Device::Pointer & device,
                        const void **           src_data_ptr,
                        const size_t &          size,
                        void **                 dst_data_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaMemcpy(*dst_data_ptr, *src_data_ptr, size, cudaMemcpyDeviceToDevice);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to write CUDA memory " + std::to_string(err));
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::copyMemory(const Device::Pointer & device,
                        const void **           src_data_ptr,
                        const size_t &          width,
                        const size_t &          height,
                        const size_t &          depth,
                        const size_t &          bytes,
                        void **                 dst_data_ptr) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }

  if (depth > 1)
  {
    cudaMemcpy3DParms copyParams = { nullptr };
    copyParams.srcPtr = make_cudaPitchedPtr(const_cast<void *>(*src_data_ptr), width * bytes, width, height);
    copyParams.srcPos = make_cudaPos(0, 0, 0);
    copyParams.dstPtr.ptr = *dst_data_ptr;
    copyParams.dstPtr.pitch = width * bytes;
    copyParams.dstPtr.xsize = width;
    copyParams.dstPtr.ysize = height;
    copyParams.extent = make_cudaExtent(width * bytes, height, depth);
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaMemcpy3D(&copyParams);
  }
  else if (height > 1)
  {
    cudaMemcpy2D(*dst_data_ptr, width * bytes, *src_data_ptr, width * bytes, width, height, cudaMemcpyDeviceToDevice);
  }
  else
  {
    cudaMemcpy(*dst_data_ptr, *src_data_ptr, width * bytes, cudaMemcpyDeviceToDevice);
  }
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to write CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::setMemory(const Device::Pointer & device,
                       void **                 data_ptr,
                       const size_t &          size,
                       const void *            value,
                       const size_t &          value_size) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  err = cudaMemset(*data_ptr, *static_cast<const int *>(value), size);
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::setMemory(const Device::Pointer & device,
                       void **                 data_ptr,
                       const size_t &          width,
                       const size_t &          height,
                       const size_t &          depth,
                       const size_t &          bytes,
                       const void *            value) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }
  if (depth > 1)
  {
    cudaExtent     extent = make_cudaExtent(width * bytes, height, depth);
    cudaPitchedPtr devPtr = make_cudaPitchedPtr(*data_ptr, width * bytes, width, height);
    err = cudaMemset3D(devPtr, *static_cast<const int *>(value), extent);
  }
  else if (height > 1)
  {
    err = cudaMemset2D(*data_ptr, width * bytes, *static_cast<const int *>(value), width, height);
  }
  else
  {
    err = cudaMemset(*data_ptr, *static_cast<const int *>(value), width * bytes);
  }
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA memory.");
  }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::loadProgramFromCache(const Device::Pointer & device, const std::string & hash, void * program) const
  -> void
{
#if CLE_CUDA
  auto     cuda_device = std::dynamic_pointer_cast<CUDADevice>(device);
  CUmodule module = nullptr;
  auto     ite = cuda_device->getCache().find(hash);
  if (ite != cuda_device->getCache().end())
  {
    module = ite->second;
  }
  program = module;
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::saveProgramToCache(const Device::Pointer & device, const std::string & hash, void * program) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<CUDADevice>(device);
  cuda_device->getCache().emplace_hint(cuda_device->getCache().end(), hash, reinterpret_cast<CUmodule>(program));
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::buildKernel(const Device::Pointer & device,
                         const std::string &     kernel_source,
                         const std::string &     kernel_name,
                         void *                  kernel) const -> void
{
#if CLE_CUDA
  auto cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  auto err = cudaSetDevice(cuda_device->getCUDADeviceIndex());
  if (err != cudaSuccess)
  {
    throw std::runtime_error("Error: Failed to set CUDA device before memory allocation.");
  }

  CUresult     error;
  nvrtcProgram prog;
  CUmodule     module;
  CUfunction   function;
  nvrtcResult compileResult;

  std::cout << kernel_source << std::endl;

  compileResult = nvrtcCreateProgram(&prog, kernel_source.c_str(), nullptr, 0, nullptr, nullptr);
  if (compileResult != NVRTC_SUCCESS)
  {
    size_t logSize;
    nvrtcGetProgramLogSize(prog, &logSize);
    std::vector<char> log(logSize);
    nvrtcGetProgramLog(prog, log.data());
    fprintf(stderr, "* Error Creating the CUDA program.\n");
  }

  compileResult = nvrtcCompileProgram(prog, 0, nullptr);
  if (compileResult != NVRTC_SUCCESS)
  {
    size_t logSize;
    nvrtcGetProgramLogSize(prog, &logSize);
    std::vector<char> log(logSize);
    nvrtcGetProgramLog(prog, log.data());
    fprintf(stderr, "* Error initializing the CUDA program.\n");
  }

  size_t ptxSize;
  nvrtcGetPTXSize(prog, &ptxSize);
  std::vector<char> ptx(ptxSize);
  nvrtcGetPTX(prog, ptx.data());

  error = cuModuleLoadData(&module, ptx.data());
  if (error != CUDA_SUCCESS)
  {
    const char * errorString;
    cuGetErrorString(error, &errorString);
    std::cerr << errorString << std::endl;
    fprintf(stderr, "* Error initializing the CUDA module.\n");
  }

  error = cuModuleGetFunction(&function, module, kernel_name.c_str());
  if (error != CUDA_SUCCESS)
  {
    const char * errorString;
    cuGetErrorString(error, &errorString);
    std::cerr << "Failed: " << errorString << std::endl;
  }

  *(reinterpret_cast<CUfunction *>(kernel)) = function;

  // std::cout << *(reinterpret_cast<CUfunction *>(kernel)) << " ";
  // printf("\n");

  std::string hash = std::to_string(std::hash<std::string>{}(kernel_source));
  // loadProgramFromCache(device, hash, module);
  // if (module == nullptr)
  // {
  //   saveProgramToCache(device, hash, module);
  // }
#else
  throw std::runtime_error("Error: CUDA backend is not enabled");
#endif
}

auto
CUDABackend::executeKernel(const Device::Pointer &       device,
                           const std::string &           kernel_source,
                           const std::string &           kernel_name,
                           const std::array<size_t, 3> & global_size,
                           const std::vector<void *> &   args,
                           const std::vector<size_t> &   sizes) const -> void
{
  // @StRigaud TODO: add cuda kernel execution
  auto       cuda_device = std::dynamic_pointer_cast<const CUDADevice>(device);
  CUresult   err;
  CUfunction cuda_kernel;
  try
  {
    buildKernel(device, kernel_source, kernel_name, &cuda_kernel);
  }
  catch (const std::exception & e)
  {
    throw std::runtime_error("Error: Failed to build kernel. \n\t > " + std::string(e.what()));
  }

  // for (size_t i = 0; i < args.size(); i++)
  // {
  //   std::cout << args.data() << " ";
  // }

  printf("%d\n", args.size());

  void* argsArray[4];
  std::copy(args.begin(), args.end(), argsArray);

  for (size_t i = 0; i < args.size(); i++)
  {
    std::cout << argsArray[i] << " ";
  }

  printf("\n");

  // dim3 blockDims(global_size[0], global_size[1], global_size[2]);
  // dim3 gridDims(x, y, z) is local

  dim3 blockDims(16, 16, 1);
  dim3 gridDims(16, 16, 1);

  // Launch the kernel
  err = cuLaunchKernel(cuda_kernel,
                       gridDims.x,
                       gridDims.y,
                       gridDims.z,
                       blockDims.x,
                       blockDims.y,
                       blockDims.z,
                       0,
                       NULL,
                       argsArray,
                       NULL);

  if (err != CUDA_SUCCESS)
  {
    const char * errorString;
    cuGetErrorString(err, &errorString);
    std::cerr << errorString << std::endl;
  }
  // cuCtxSynchronize();
}

auto
CUDABackend::getPreamble() const -> std::string
{
  return kernel::preamble_cu; // @StRigaud TODO: add cuda preamble from header file
}

} // namespace cle
