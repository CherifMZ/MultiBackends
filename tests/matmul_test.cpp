#include "array.hpp"
#include "backend.hpp"
#include "device.hpp"
#include "execution.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include <variant>

#include "cle_matmul.h"

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

  cle::ParameterList parameters;
  cle::ConstantList  constants;
  cle::KernelInfo    kernel = { "matmul", kernel::matmul };
  cle::RangeArray    global_rage;
  int                N = 8;

  static const size_t size = 5 * 5 * 2;
  cle::Array          arrC(5, 5, 2, cle::dType::Float, cle::mType::Image, device);

  float * data = new float[size];
  for (int i = 0; i < size; i++)
  {
    data[i] = i;
  }
  cle::Array arrA(5, 5, 2, cle::dType::Float, cle::mType::Image, data, device);

  for (int i = 0; i < size; i++)
  {
    data[i] = 1;
  }
  cle::Array arrB(5, 5, 2, cle::dType::Float, cle::mType::Image, data, device);

  parameters.push_back({ "src1", arrA.ptr() });
  parameters.push_back({ "src2", arrB.ptr() });
  parameters.push_back({ "dst", arrC.ptr() });
  parameters.push_back({ "int", N });

  cle::execute(device, kernel, parameters, constants, global_rage);

  float data_out[size];
  arrC.read(data_out);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out[i] << " ";
  }
  std::cout << std::endl;
}

int
main(int argc, char ** argv)
{
  //   cle::BackendManager::getInstance().setBackend(false);
  //   std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
  //   run_test();

  cle::BackendManager::getInstance().setBackend(true);
  std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
  run_test();

  return 0;
}