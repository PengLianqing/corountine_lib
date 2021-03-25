/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       coroutine.h
  * @brief      协程类
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
#include <functional>
#include "context.h"
#include "utils.h"

namespace tinyco
{

	enum coStatus
	{
		READY = 0,
		RUNNING,
		WAITING,
		DEAD
	};

	class Schedule;

	class Coroutine
	{
	public:
		Coroutine(Schedule*, std::function<void()>&&);
		Coroutine(Schedule*, std::function<void()>&);
		~Coroutine();

		DISALLOW_COPY_MOVE_AND_ASSIGN(Coroutine);

		//恢复运行当前协程
		void resume(bool isFirstCo = false);

		//暂停运行当前协程
		void yield();

		//运行该协程的函数
		void startFunc() { coFunc_(); };

		//获取该协程的上下文
		Context* getCtx() { return &ctx_; }

	private:

		std::function<void()> coFunc_;

		Schedule* pMySchedule_;

		int status_;

		Context ctx_;

	};

}