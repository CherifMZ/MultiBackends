#include "array.hpp"
#include "backend.hpp"
#include "device.hpp"
#include "execution.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include <variant>


// I still believe there are some errors in reading/writing into the arrays
// The error :
// Populating the array with -> cle::Array gpu_arr2(h, w, d, cle::dType::Float, cle::mType::Buffer, data, device);
// Sol: Initilize an empty array then write in it with -> gpu_arr2.write(data);

void
run_test()
{
  cle::BackendManager & backendManager = cle::BackendManager::getInstance();
  auto                  device = backendManager.getBackend().getDevice("TX", "all");
  device->initialize();

  int          h = 8;
  int          w = 8;
  int          d = 2;
  int          N = 8;
  CUmodule     cuModule;
  CUfunction   cuFunction;
  CUresult     err;
  nvrtcResult  errN;
  nvrtcResult  compileResult;
  nvrtcProgram prog;
  size_t       ptxSize;
  std::string  mykernel = cle::loadFile("C:/Users/cherif/Desktop/backends/MultiBackends/myKernel.cu");

  static const size_t size = h * w * d;
  int *               data = new int[size];

  // Populating gpu_arr1
  for (int i = 0; i < size; i++)
  {
    data[i] = i;
  }
  cle::Array gpu_arr1(h, w, d, cle::dType::Int32, cle::mType::Buffer, device);
  gpu_arr1.write(data);

  // Populating gpu_arr2
  for (int i = 0; i < size; i++)
  {
    data[i] = 1;
  }
  cle::Array gpu_arr2(h, w, d, cle::dType::Int32, cle::mType::Buffer, device); // 1, 2 ...
  gpu_arr2.write(data);

  // gpu_arr3 to read back data from device
  cle::Array gpu_arr3(h, w, d, cle::dType::Int32, cle::mType::Buffer, device);

  std::vector<void *> argsV = { *gpu_arr1.get(), *gpu_arr2.get(), *gpu_arr3.get(), &N };

  compileResult = nvrtcCreateProgram(&prog, mykernel.c_str(), nullptr, 0, nullptr, nullptr);
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
    std::string s(log.data());
    std::cout << s << std::endl;
  }

  nvrtcGetPTXSize(prog, &ptxSize);
  std::vector<char> ptx(ptxSize);
  nvrtcGetPTX(prog, ptx.data());

  err = cuModuleLoadData(&cuModule, ptx.data());
  if (err != CUDA_SUCCESS)
  {
    const char * errorString;
    cuGetErrorString(err, &errorString);
    std::cerr << errorString << std::endl;
    // fprintf(stderr, "* Error initializing the CUDA module.\n");
  }

  err = cuModuleGetFunction(&cuFunction, cuModule, "myKernel");
  if (err != CUDA_SUCCESS)
  {
    const char * errorString;
    cuGetErrorString(err, &errorString);
    std::cerr << "Failed: " << errorString << std::endl;
  }

  dim3 blockDims(8, 8, 1);
  dim3 gridDims(8, 8, 1);

  // Launch the kernel
  err = cuLaunchKernel(
    cuFunction, gridDims.x, gridDims.y, gridDims.z, blockDims.x, blockDims.y, blockDims.z, 0, NULL, argsV.data(), NULL);

  if (err != CUDA_SUCCESS)
  {
    fprintf(stderr, "* Error in Launch.\n");
  }

  int * data_out = new int[size];
  gpu_arr3.read(data_out);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out[i] << " ";
  }
  std::cout << std::endl;
}

int
main(int argc, char ** argv)
{
  cle::BackendManager::getInstance().setBackend(true);
  std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
  run_test();

  return 0;
}