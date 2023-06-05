#ifndef __INCLUDE_BACKEND_HPP
#define __INCLUDE_BACKEND_HPP

#include "device.hpp"
#include "gem.hpp"
#include "utils.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace cle
{
class Backend
{
public:
  enum class Type
  {
    CUDA,
    OPENCL
  };

  using DevicePtr = std::shared_ptr<cle::Device>;

  Backend() = default;
  virtual ~Backend() = default;

  [[nodiscard]] virtual auto
  getType() const -> Backend::Type = 0;
  [[nodiscard]] virtual auto
  getDevicesList(const std::string & type) const -> std::vector<std::string> = 0;
  [[nodiscard]] virtual auto
  getDevices(const std::string & type) const -> std::vector<DevicePtr> = 0;
  [[nodiscard]] virtual auto
  getDevice(const std::string & name, const std::string & type) const -> DevicePtr = 0;

  [[nodiscard]] virtual auto
  getPreamble() const -> std::string = 0;

  virtual auto
  allocateMemory(const DevicePtr & device, const size_t & size, void ** data_ptr) const -> void = 0;
  virtual auto
  allocateMemory(const DevicePtr & device,
                 const size_t &    width,
                 const size_t &    height,
                 const size_t &    depth,
                 const dType &     dtype,
                 void **           data_ptr) const -> void = 0;
  virtual auto
  freeMemory(const DevicePtr & device, const mType & mtype, void ** data_ptr) const -> void = 0;
  virtual auto
  writeMemory(const DevicePtr & device, void ** data_ptr, const size_t & size, const void * host_ptr) const -> void = 0;
  virtual auto
  writeMemory(const DevicePtr & device,
              void **           data_ptr,
              const size_t &    width,
              const size_t &    height,
              const size_t &    depth,
              const size_t &    bytes,
              const void *      host_ptr) const -> void = 0;
  virtual auto
  readMemory(const DevicePtr & device, const void ** data_ptr, const size_t & size, void * host_ptr) const -> void = 0;
  virtual auto
  readMemory(const DevicePtr & device,
             const void **     data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void *            host_ptr) const -> void = 0;
  virtual auto
  copyMemory(const DevicePtr & device, const void ** src_data_ptr, const size_t & size, void ** dst_data_ptr) const
    -> void = 0;
  virtual auto
  copyMemory(const DevicePtr & device,
             const void **     src_data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void **           dst_data_ptr) const -> void = 0;
  virtual auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    size,
            const void *      value,
            const size_t &    value_size) const -> void = 0;
  virtual auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    width,
            const size_t &    height,
            const size_t &    depth,
            const size_t &    bytes,
            const void *      value) const -> void = 0;

  virtual auto
  buildKernel(const DevicePtr &   device,
              const std::string & kernel_source,
              const std::string & kernel_name,
              void *              kernel) const -> void = 0;
  virtual auto
  loadProgramFromCache(const DevicePtr & device, const std::string & hash, void * program) const -> void = 0;
  virtual auto
  saveProgramToCache(const DevicePtr & device, const std::string & hash, void * program) const -> void = 0;
  virtual auto
  executeKernel(const DevicePtr &             device,
                const std::string &           kernel_source,
                const std::string &           kernel_name,
                const std::array<size_t, 3> & global_size,
                const std::vector<void *> &   args,
                const std::vector<size_t> &   sizes) const -> void = 0;

  friend auto
  operator<<(std::ostream & out, const Backend::Type & backend_type) -> std::ostream &
  {
    switch (backend_type)
    {
      case Backend::Type::CUDA:
        out << "CUDA";
        break;
      case Backend::Type::OPENCL:
        out << "OpenCL";
        break;
    }
    return out;
  }

  friend auto
  operator<<(std::ostream & out, const Backend & backend) -> std::ostream &
  {
    out << backend.getType() << " backend";
    return out;
  }
};

class CUDABackend : public Backend
{
public:
  CUDABackend() = default;
  ~CUDABackend() override = default;

  [[nodiscard]] auto
  getDevices(const std::string & type) const -> std::vector<DevicePtr> override;
  [[nodiscard]] auto
  getDevice(const std::string & name, const std::string & type) const -> DevicePtr override;
  [[nodiscard]] auto
  getDevicesList(const std::string & type) const -> std::vector<std::string> override;
  [[nodiscard]] auto
  getType() const -> Backend::Type override;

  auto
  allocateMemory(const DevicePtr & device, const size_t & size, void ** data_ptr) const -> void override;
  auto
  allocateMemory(const DevicePtr & device,
                 const size_t &    width,
                 const size_t &    height,
                 const size_t &    depth,
                 const dType &     dtype,
                 void **           data_ptr) const -> void override;
  auto
  freeMemory(const DevicePtr & device, const mType & mtype, void ** data_ptr) const -> void override;
  auto
  writeMemory(const DevicePtr & device, void ** data_ptr, const size_t & size, const void * host_ptr) const
    -> void override;
  auto
  writeMemory(const DevicePtr & device,
              void **           data_ptr,
              const size_t &    width,
              const size_t &    height,
              const size_t &    depth,
              const size_t &    bytes,
              const void *      host_ptr) const -> void override;
  auto
  readMemory(const DevicePtr & device, const void ** data_ptr, const size_t & size, void * host_ptr) const
    -> void override;
  auto
  readMemory(const DevicePtr & device,
             const void **     data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void *            host_ptr) const -> void override;

  auto
  copyMemory(const DevicePtr & device, const void ** src_data_ptr, const size_t & size, void ** dst_data_ptr) const
    -> void override;
  auto
  copyMemory(const DevicePtr & device,
             const void **     src_data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void **           dst_data_ptr) const -> void override;
  auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    size,
            const void *      value,
            const size_t &    value_size) const -> void override;
  auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    width,
            const size_t &    height,
            const size_t &    depth,
            const size_t &    bytes,
            const void *      value) const -> void override;
  auto
  loadProgramFromCache(const DevicePtr & device, const std::string & hash, void * program) const -> void override;
  auto
  saveProgramToCache(const DevicePtr & device, const std::string & hash, void * program) const -> void override;
  auto
  buildKernel(const DevicePtr &   device,
              const std::string & kernel_source,
              const std::string & kernel_name,
              void *              kernel) const -> void override;
  auto
  executeKernel(const DevicePtr &             device,
                const std::string &           kernel_source,
                const std::string &           kernel_name,
                const std::array<size_t, 3> & global_size,
                const std::vector<void *> &   args,
                const std::vector<size_t> &   sizes) const -> void override;
  [[nodiscard]] auto
  getPreamble() const -> std::string override;
};

class OpenCLBackend : public Backend
{
public:
  OpenCLBackend() = default;
  ~OpenCLBackend() override = default;

  [[nodiscard]] auto
  getDevices(const std::string & type) const -> std::vector<DevicePtr> override;
  [[nodiscard]] auto
  getDevice(const std::string & name, const std::string & type) const -> DevicePtr override;
  [[nodiscard]] auto
  getDevicesList(const std::string & type) const -> std::vector<std::string> override;
  [[nodiscard]] auto
  getType() const -> Backend::Type override;

  auto
  allocateMemory(const DevicePtr & device, const size_t & size, void ** data_ptr) const -> void override;
  auto
  allocateMemory(const DevicePtr & device,
                 const size_t &    width,
                 const size_t &    height,
                 const size_t &    depth,
                 const dType &     dtype,
                 void **           data_ptr) const -> void override;
  auto
  freeMemory(const DevicePtr & device, const mType & mtype, void ** data_ptr) const -> void override;
  auto
  writeMemory(const DevicePtr & device, void ** data_ptr, const size_t & size, const void * host_ptr) const
    -> void override;
  auto
  writeMemory(const DevicePtr & device,
              void **           data_ptr,
              const size_t &    width,
              const size_t &    height,
              const size_t &    depth,
              const size_t &    bytes,
              const void *      host_ptr) const -> void override;
  auto
  readMemory(const DevicePtr & device, const void ** data_ptr, const size_t & size, void * host_ptr) const
    -> void override;
  auto
  readMemory(const DevicePtr & device,
             const void **     data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void *            host_ptr) const -> void override;
  auto
  copyMemory(const DevicePtr & device, const void ** src_data_ptr, const size_t & size, void ** dst_data_ptr) const
    -> void override;
  auto
  copyMemory(const DevicePtr & device,
             const void **     src_data_ptr,
             const size_t &    width,
             const size_t &    height,
             const size_t &    depth,
             const size_t &    bytes,
             void **           dst_data_ptr) const -> void override;
  auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    size,
            const void *      value,
            const size_t &    value_size) const -> void override;
  auto
  setMemory(const DevicePtr & device,
            void **           data_ptr,
            const size_t &    width,
            const size_t &    height,
            const size_t &    depth,
            const size_t &    bytes,
            const void *      value) const -> void override;
  auto
  loadProgramFromCache(const DevicePtr & device, const std::string & hash, void * program) const -> void override;
  auto
  saveProgramToCache(const DevicePtr & device, const std::string & hash, void * program) const -> void override;
  auto
  buildKernel(const DevicePtr &   device,
              const std::string & kernel_source,
              const std::string & kernel_name,
              void *              kernel) const -> void override;
  auto
  executeKernel(const DevicePtr &             device,
                const std::string &           kernel_source,
                const std::string &           kernel_name,
                const std::array<size_t, 3> & global_size,
                const std::vector<void *> &   args,
                const std::vector<size_t> &   sizes) const -> void override;
  [[nodiscard]] auto
  getPreamble() const -> std::string override;
};

// class BackendManager
// {
// public:
//   static auto
//   getInstance() -> BackendManager &
//   {
//     static BackendManager instance;
//     return instance;
//   }

//   auto
//   setBackend(bool useCUDA) -> void
//   {
//     if (useCUDA)
//     {
//       this->backend = std::make_unique<CUDABackend>();
//     }
//     else
//     {
//       this->backend = std::make_unique<OpenCLBackend>();
//     }
//   }

//   [[nodiscard]] auto
//   getBackend() const -> const Backend &
//   {
//     if (!this->backend)
//     {
//       throw std::runtime_error("Backend not selected.");
//     }
//     return *this->backend;
//   }

//   friend auto
//   operator<<(std::ostream & out, const BackendManager & backend_manager) -> std::ostream &
//   {
//     out << backend_manager.getBackend().getType() << " backend";
//     return out;
//   }

//   auto
//   operator=(const BackendManager &) -> BackendManager & = delete;
//   BackendManager(const BackendManager &) = delete;

// private:
//   std::shared_ptr<Backend> backend;
//   BackendManager() = default;
// };

} // namespace cle

#endif // __INCLUDE_BACKEND_HPP
