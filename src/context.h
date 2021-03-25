/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       context
  * @brief      上下文类
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
#include "utils.h"
#include <ucontext.h>

namespace tinyco
{

    class Schedule;
	class Context
	{
	public:
		Context();
		~Context();

		Context(const Context& otherCtx) 
			: pCtx_(otherCtx.pCtx_), pStack_(otherCtx.pStack_)
		{ }

		Context(Context&& otherCtx)
			: pCtx_(otherCtx.pCtx_), pStack_(otherCtx.pStack_)
		{ }

		Context& operator=(const Context& otherCtx) = delete;

		//用函数指针设置当前context的上下文入口
		void makeContext(void (*func)(), Schedule*);

		//直接用当前程序状态设置当前context的上下文
		void makeCurContext();

		//将上下文保存到oldCtx中，然后切换到当前上下文
		void swapToMe(Context* pOldCtx);

		//获取当前上下文的ucontext_t指针，直接返回pCtx_
		struct ucontext_t* getUCtx() { return pCtx_; };

	private:

		// Context to describe whole processor state.
		struct ucontext_t* pCtx_; // 上下文，描述整个处理器的状态

		void* pStack_; // pCtx_使用到的协程栈

	};

}