/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       main
  * @brief      协程测试
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-1-2021      Peng            1. 完成
  *
  @verbatim
  ==============================================================================
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Peng****************************
  */
#include <iostream>
#include "./src/schedule.h"

int main()
{
	co_go(
		[]
		{
			std::cout << "coroutines 1 - 1 - 1" << std::endl;
			co_go(
				[]
				{
					std::cout << "coroutines 1 - 2 - 1" << std::endl;
					co_yield();
					std::cout << "coroutines 1 - 2 - 2" << std::endl;
				}
				);
			std::cout << "coroutines 1 - 1 - 2" << std::endl;
		}
	);

	co_go(
		[]
		{
			std::cout << "coroutines 2 - 1 - 1" << std::endl;
			co_go(
				[]
				{
					std::cout << "coroutines 2 - 2 - 1" << std::endl;
					co_yield();
					std::cout << "coroutines 2 - 2 - 2" << std::endl;
				}
				);
			std::cout << "coroutines 2 - 1 - 2" << std::endl;
		}
		);
	std::cout << "end" << std::endl;
}