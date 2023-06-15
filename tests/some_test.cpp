#include "array.hpp"
#include "backend.hpp"
#include "device.hpp"
#include "execution.hpp"
#include "utils.hpp"

void
run_test()
{
  cle::BackendManager & backendManager = cle::BackendManager::getInstance();
  auto                  device_list = backendManager.getBackend().getDevicesList("all");
  // std::cout << "Device list:" << std::endl;
  // for (auto && i : device_list)
  // {
  //   std::cout << "\t" << i << std::endl;
  // }

  auto device = backendManager.getBackend().getDevice("TX", "all");
  // std::cout << "Selected Device :" << device->getName() << std::endl;
  // std::cout << "Device Info :" << device->getInfo() << std::endl;
  device->initialize();

  // I still believe there are some errors in reading/writing into the arrays
  static const size_t size = 5 * 5 * 1;
  cle::Array          gpu_arr1(5, 5, 1, cle::dType::Int32, cle::mType::Buffer, device);
  int * data = new int[size];
  for (int i = 0; i < size; i++)
  {
    data[i] = i;
  }
  cle::Array gpu_arr2(5, 5, 1, cle::dType::Int32, cle::mType::Buffer, data, device); // 1, 2 ...

  int * data_out = new int[size];
  for (int i = 0; i < size; i++)
  {
    data_out[i] = -i;
  }
  std::cout << std::endl;
  gpu_arr2.read(data_out); // 1, 2 ...
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out[i] << " ";
  }
  std::cout << std::endl;

  gpu_arr1.write(data); // 1, 2 ...
  int * data_out2 = new int[size];
  gpu_arr1.read(data_out2); // 1, 2 ...
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out2[i] << " ";
  }
  std::cout << std::endl;

  cle::Array gpu_arr3(5, 5, 1, cle::dType::Int32, cle::mType::Buffer, device);
  int *      data_out3 = new int[size];
  gpu_arr1.copy(gpu_arr3);
  gpu_arr3.read(data_out3);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out3[i] << " ";
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





// #include "array.hpp"
// #include "backend.hpp"
// #include "device.hpp"
// #include "execution.hpp"
// #include "memory.hpp"
// #include "utils.hpp"
// #include <variant>

// auto
// run_test() -> void
// {

//   cle::BackendManager & backendManager = cle::BackendManager::getInstance();
//   auto                  device_list = backendManager.getBackend().getDevicesList("all");
//   // std::cout << "Device list:" << std::endl;
//   // for (auto && i : device_list)
//   // {
//   //   std::cout << "\t" << i << std::endl;
//   // }

//   auto device = backendManager.getBackend().getDevice("TX", "all");
//   // std::cout << "Selected Device :" << device->getName() << std::endl;
//   // std::cout << "Device Info :" << device->getInfo() << std::endl;
//   device->initialize();

//   std::string mykernel = cle::loadFile("C:/Users/cherif/Desktop/backends/MultiBackends/myKernel.cu");

//   int         N = 4;
//   CUmodule    cuModule;
//   CUfunction  cuFunction;
//   CUresult    err;
//   nvrtcResult errN;
//   nvrtcResult compileResult;


//   static const size_t size = 6 * 5 * 4;
//   int *               data1 = new int[size];
//   int *               data2 = new int[size];
//   for (int i = 0; i < size; i++)
//   {
//     data1[i] = i;
//     data2[i] = 2;
//   }
//   cle::Array gpu_arr1(6, 5, 4, cle::dType::Int32, cle::mType::Buffer, device);
//   gpu_arr1.write(data1);

//   cle::Array gpu_arr2(6, 5, 4, cle::dType::Int32, cle::mType::Buffer, device);
//   gpu_arr2.write(data2);

//   float data_out[size];
//   gpu_arr1.read(data_out);
//   for (int i = 0; i < size; i++)
//   {
//     std::cout << data_out[i] << " ";
//   }
//   std::cout << std::endl;


//   // std::vector<void *> argsV = { *gpu_arr1.get(), *gpu_arr2.get(), *gpu_arr3.get(), &N };
//   // void * args[] = { *gpu_arr1.get(), *gpu_arr2.get(), *gpu_arr3.get(), &N};

//   // nvrtcProgram prog;
//   // compileResult = nvrtcCreateProgram(&prog, mykernel.c_str(), nullptr, 0, nullptr, nullptr);
//   // if (compileResult != NVRTC_SUCCESS)
//   // {
//   //   size_t logSize;
//   //   nvrtcGetProgramLogSize(prog, &logSize);
//   //   std::vector<char> log(logSize);
//   //   nvrtcGetProgramLog(prog, log.data());
//   //   fprintf(stderr, "* Error Creating the CUDA program.\n");
//   // }

//   // compileResult = nvrtcCompileProgram(prog, 0, nullptr);
//   // if (compileResult != NVRTC_SUCCESS)
//   // {
//   //   size_t logSize;
//   //   nvrtcGetProgramLogSize(prog, &logSize);
//   //   std::vector<char> log(logSize);
//   //   nvrtcGetProgramLog(prog, log.data());
//   //   fprintf(stderr, "* Error initializing the CUDA program.\n");
//   //   std::string s(log.data());
//   //   std::cout << s << std::endl;
//   // }

//   // size_t ptxSize;
//   // nvrtcGetPTXSize(prog, &ptxSize);
//   // std::vector<char> ptx(ptxSize);
//   // nvrtcGetPTX(prog, ptx.data());

//   // err = cuModuleLoadData(&cuModule, ptx.data());
//   // if (err != CUDA_SUCCESS)
//   // {
//   //   const char * errorString;
//   //   cuGetErrorString(err, &errorString);
//   //   std::cerr << errorString << std::endl;
//   //   // fprintf(stderr, "* Error initializing the CUDA module.\n");
//   // }

//   // err = cuModuleGetFunction(&cuFunction, cuModule, "myKernel");
//   // if (err != CUDA_SUCCESS)
//   // {
//   //   const char * errorString;
//   //   cuGetErrorString(err, &errorString);
//   //   std::cerr << "Failed: " << errorString << std::endl;
//   // }

//   // dim3 blockDims(4, 4, 1);
//   // dim3 gridDims(4, 4, 1);

//   // // Launch the kernel
//   // err = cuLaunchKernel(
//   //   cuFunction, gridDims.x, gridDims.y, gridDims.z, blockDims.x, blockDims.y, blockDims.z, 0, NULL, args, NULL);

//   // if (err != CUDA_SUCCESS)
//   // {
//   //   fprintf(stderr, "* Error in Launch.\n");
//   // }
//   // // cuCtxSynchronize();

//   // float data_out[size];

//   // gpu_arr3.read(data_out);

//   // for (int i = 0; i < size; i++)
//   // {
//   //   std::cout << data_out[i] << " ";
//   // }
//   // std::cout << std::endl;
// }

// int
// main(int argc, char ** argv)
// {
//   cle::BackendManager::getInstance().setBackend(true);
//   std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
//   run_test();

//   return 0;
// }