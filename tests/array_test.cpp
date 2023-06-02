#include "cobra/array.hpp"
#include "cobra/backend.hpp"
#include "cobra/device.hpp"
#include "cobra/execution.hpp"
#include "cobra/utils.hpp"

void
run_test()
{
  cle::BackendManager & backendManager = cle::BackendManager::getInstance();
  auto                  device_list = backendManager.getBackend().getDevicesList("all");
  std::cout << "Device list:" << std::endl;
  for (auto && i : device_list)
  {
    std::cout << "\t" << i << std::endl;
  }

  auto device = backendManager.getBackend().getDevice("TX", "all");
  std::cout << "Selected Device :" << device->getName() << std::endl;
  std::cout << "Device Info :" << device->getInfo() << std::endl;
  device->initialize();

  std::cout << "Allocate memory" << std::endl;
  size_t     size = 5 * 5 * 2;
  cle::Array gpu_arr1(5, 5, 2, cle::dType::Int32, cle::mType::Buffer, device);

  std::cout << "Write memory" << std::endl;
  int data[size];
  for (int i = 0; i < size; i++)
  {
    data[i] = i;
  }
  cle::Array gpu_arr2(5, 5, 2, cle::dType::Int32, cle::mType::Buffer, data, device);

  std::cout << "Read memory" << std::endl;
  int data_out[size];
  for (int i = 0; i < size; i++)
  {
    data_out[i] = -i;
    std::cout << data_out[i] << " ";
  }
  std::cout << std::endl;
  gpu_arr2.read(data_out);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Write existing memory" << std::endl;
  gpu_arr1.write(data);
  int data_out2[size];
  gpu_arr1.read(data_out2);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out2[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Copy memory" << std::endl;
  cle::Array gpu_arr3(5, 5, 2, cle::dType::Int32, cle::mType::Buffer, device);
  int        data_out3[size];
  gpu_arr1.copy(gpu_arr3);
  gpu_arr3.read(data_out3);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out3[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Fill up memory " << std::endl;
  gpu_arr1.fill(1);
  gpu_arr1.read(data_out2);
  for (int i = 0; i < size; i++)
  {
    std::cout << data_out2[i] << " ";
  }
  std::cout << std::endl;
}

int
main(int argc, char ** argv)
{
  cle::BackendManager::getInstance().setBackend(false);
  std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
  run_test();

  cle::BackendManager::getInstance().setBackend(true);
  std::cout << cle::BackendManager::getInstance().getBackend().getType() << std::endl;
  run_test();

  return 0;
}