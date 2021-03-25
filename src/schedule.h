/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       schedule.h
  * @brief      协程调度类
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
#include <queue>
#include <mutex>
#include "context.h"

#include "coroutine.h"

//运行一个新协程
#define co_go(func) tinyco::Schedule::getSchedule()->goNewCo(func)

//暂停当前协程
#define co_yield() tinyco::Schedule::getSchedule()->resumeAnotherCoroutine()

namespace tinyco
{

	class Schedule
	{
	public:
		DISALLOW_COPY_MOVE_AND_ASSIGN(Schedule);

		//获取当前协程单例
		static Schedule* getSchedule();

		//运行一个新协程，该协程的函数是func
		void goNewCo(std::function<void()>&& func);
		void goNewCo(std::function<void()>& func);

		//恢复运行另一个协程
		void resumeAnotherCoroutine();

		//清除当前正在运行的协程
		void killCurCo();

		//获取上一个运行的协程
		Coroutine* getLastCoroutine() { return pLastCoroutine_; };

		//获取当前正在运行的协程
		Coroutine* getCurRunningCo() { return pCurCoroutine_; };

		Context* getMainCtx() { return &mainCtx_; }

	private:
		Schedule();

		~Schedule();

		void resume(Coroutine*); //恢复运行指定协程

		bool isFirstGo_; // 协程队列为空，调度时会使用到mainCtx_

		std::queue<Coroutine*> coroutines_; // 协程队列

		Coroutine* pLastCoroutine_; // 上一个运行的协程

		Coroutine* pCurCoroutine_; // 当前运行的协程

		Context mainCtx_; // 用于保存调度器刚开始有协程开始工作时初始状态或协程队列空之后重新有新的协程时的上下文

		static Schedule* scheduler_; // 静态变量scheduler_ ，Schedule::getSchedule()直接使用，不需要新建对象
		
		static std::mutex initMutex_; // 静态变量initMutex_

	};

}