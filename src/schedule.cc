/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       schedule
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
#include "schedule.h"

using namespace tinyco;

Schedule* Schedule::scheduler_ = nullptr;
std::mutex Schedule::initMutex_;

/**
  * @brief          构造函数
  * 初始化isFirstGo_，pLastCoroutine_，pCurCoroutine_
  * makeCurContext 直接用当前程序状态设置当前context的上下文
  */
Schedule::Schedule() 
        : isFirstGo_(true), pLastCoroutine_(nullptr), pCurCoroutine_(nullptr)
{
	mainCtx_.makeCurContext();
}

Schedule::~Schedule()
{ }

/**
  * @brief          获取当前协程单例
  * 初始化scheduler_ 或 返回scheduler_ （Schedule对象）
  * 
  */
Schedule* Schedule::getSchedule()
{
	if (!scheduler_) // 锁定初始化，并为scheduler_分配内存
	{
		std::lock_guard<std::mutex> lock(initMutex_);
		if (!scheduler_)
		{
			scheduler_ = new Schedule();
		}
	}
	return scheduler_;
}

/**
  * @brief          恢复运行指定协程
  * 暂停当前协程并添加到协程队列（WAITING）
  * 恢复运行指定协程（RUNNING）
  */
void Schedule::resume(Coroutine* pCo)
{
	if (nullptr == pCo)
	{
		return;
	}

	// 暂停运行当前的协程并添加到协程队列中
	pLastCoroutine_ = pCurCoroutine_; 
	if (pLastCoroutine_)
	{
	    pLastCoroutine_->yield(); // 将协程状态转为WAITING
		coroutines_.push(pLastCoroutine_);
	}

	// 恢复运行指定协程
	pCurCoroutine_ = pCo; 
	if (isFirstGo_)
	{
		isFirstGo_ = false;
		pCo->resume(true);
	}
	else
	{
		pCo->resume();
	}
}

/**
  * @brief          运行一个新的协程
  * 创建Coroutine协程对象，并运行该协程
  */
void Schedule::goNewCo(std::function<void()>&& coFunc)
{
	Coroutine* pCo = new Coroutine(this, std::move(coFunc));
	resume(pCo);
}

void Schedule::goNewCo(std::function<void()>& coFunc)
{
	Coroutine* pCo = new Coroutine(this, coFunc);
	resume(pCo);
}

/**
  * @brief          删除当前运行的协程
  * 删除pCurCoroutine_（Coroutine）对象并释放内存
  */
void Schedule::killCurCo()
{
	delete pCurCoroutine_;
	pCurCoroutine_ = nullptr;
}

/**
  * @brief          恢复运行另一个协程
  * 从协程队列中弹出一个协程并恢复运行
  */
void Schedule::resumeAnotherCoroutine()
{
	if (coroutines_.empty()) // 如果协程队列尾空，设置isFirstGo_，并清空mainCtx_
	{
		isFirstGo_ = true;
		mainCtx_.swapToMe(nullptr);
	}
	Coroutine* pCo = coroutines_.front();
	coroutines_.pop();
	resume(pCo);
}
