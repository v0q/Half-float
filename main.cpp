#include <iostream>
#include <vector>
#include "half.h"

int main()
{
  std::vector<half_float::half> halfVector;
  std::vector<float> floatVector;

  float scale = 1.f;
  float error = 0.f;
  unsigned int samps = 1000000;
  const int tmp = std::pow(10, 4);

  for(unsigned int i = 0; i < samps; ++i)
  {
    float rndfloat = std::roundf((rand()/static_cast<float>(RAND_MAX) * scale)*tmp)/tmp;
    floatVector.push_back(rndfloat);
    halfVector.push_back(half_float::half(rndfloat));

    if(floatVector[i] >= 0.0001f)
      error += std::abs(100.f - halfVector[i] / floatVector[i] * 100.f);

//    std::cout << i << " - float: " << floatVector[i] << ", half: " << halfVector[i] << "\n";
  }

  std::cout << "\n--------------\n";
  std::cout << "Statistics:\n";
  std::cout << "  Samples: " << samps << "\n";
  std::cout << "  Scale: " << scale << "\n";
  std::cout << "  Avg error: " << error/samps << "%\n";
  std::cout << "  Avg error margin: " << (error/(samps * 100)) * scale << "\n";
  std::cout << "  Orig vector size: " << sizeof(float)*floatVector.size() << " bytes\n";
  std::cout << "  Half vector size: " << sizeof(half_float::half)*halfVector.size() << " bytes\n";
  std::cout << "--------------\n";

	return EXIT_SUCCESS;
}
