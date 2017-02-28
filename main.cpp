#include <iostream>
#include "half.h"

int main()
{
	half_float::half h = 1.5f;

	std::cout << h << "\n";

	return EXIT_SUCCESS;
}
