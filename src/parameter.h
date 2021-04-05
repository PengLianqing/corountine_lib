/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       parameter.h
  * @brief      
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
#pragma once
#include <stddef.h>

namespace copnano
{
	namespace parameter
	{
		//协程栈大小
		const static size_t coroutineStackSize = 32 * 1024;
	}
	
}