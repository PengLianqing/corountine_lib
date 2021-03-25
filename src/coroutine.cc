/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       coroutine
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
#include "coroutine.h"
#include "schedule.h"
#include "parameter.h"

using namespace tinyco;

/**
  * @brief          coWrapFunc
  * 
  */
static void coWrapFunc(Schedule* sche)
{
	sche->getCurRunningCo()->startFunc(); // 运行当前协程传入的函数，直到co_yield()，并没有初始化pCurCoroutine_,只有恢复执行的协程才会是pCurCoroutine_
	sche->killCurCo(); // 删除当前运行的协程（此处应该是空，后面调用resume会将pCurCoroutine_塞到队列里）
	sche->resumeAnotherCoroutine(); // 运行另一个协程
}

/**
  * @brief          构造函数
  * 协程状态设置为 READY
  */
Coroutine::Coroutine(Schedule* pMySchedule, std::function<void()>&& func)
	: coFunc_(std::move(func)), pMySchedule_(pMySchedule), status_(DEAD)//, ctx_(stack_, parameter::coroutineStackSize)
{
	status_ = READY;
}

Coroutine::Coroutine(Schedule* pMySchedule, std::function<void()>& func)
	: coFunc_(func), pMySchedule_(pMySchedule), status_(DEAD)//, ctx_(stack_, parameter::coroutineStackSize)
{
	status_ = READY;
}

Coroutine::~Coroutine()
{
}

/**
  * @brief          恢复运行当前协程
  * 如果协程运行状态为 READY ，则转换为 RUNNING
  * 如果协程运行状态为 WAITING ，则转换为 RUNNING
  */
void Coroutine::resume(bool isFirstCo)
{
	Coroutine* pLastCo = pMySchedule_->getLastCoroutine(); // 获取上一个协程
	switch (status_)
	{
	case READY:
        status_ = RUNNING;
		ctx_.makeContext((void (*)(void)) coWrapFunc, pMySchedule_); // 用函数指针设置当前context的上下文入口

		// 将上下文保存到pLastCo/MainCtx并切换到当前上下文
		// 只有ready且isFirstCo才会把上下文保存到MainCtx ， 否则保存到pLastCo
		if (isFirstCo) 
		{
			ctx_.swapToMe(pMySchedule_->getMainCtx()); 
		}
		else
		{
			ctx_.swapToMe(pLastCo ? pLastCo->getCtx() : nullptr);
		}
		break;

	case WAITING:
		status_ = RUNNING;
		ctx_.swapToMe(pLastCo ? pLastCo->getCtx() : nullptr);
		break;

	default:

		break;
	}
}

/**
  * @brief          暂停运行当前协程
  * 协程状态设置为 WAITING
  */
void Coroutine::yield()
{
	status_ = WAITING;
}