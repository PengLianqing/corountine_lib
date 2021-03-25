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
#include "context.h"
#include "parameter.h"
#include <stdlib.h>

using namespace tinyco;

Context::Context()
	:pCtx_(nullptr),pStack_(nullptr)
{ }

Context::~Context()
{
	if (pCtx_)
	{
		delete pCtx_;
	}
	if (pStack_)
	{
		free(pStack_);
	}
}

/**
  * @brief          通过函数设置当前context的上下文入口
  * getcontext保存当前上下文到pCtx_中
  */
void Context::makeContext(void (*func)(), Schedule* pSche)
{
	// 为pCtx_ ，pStack_申请内存
	if (nullptr == pCtx_)
	{
		pCtx_ = new struct ucontext_t;
	}
	if (nullptr == pStack_)
	{
		pStack_ = malloc(parameter::coroutineStackSize);
	}
	::getcontext(pCtx_); // 保存上下文到pCtx_
    pCtx_->uc_stack.ss_sp = pStack_;
    pCtx_->uc_stack.ss_size = parameter::coroutineStackSize;
    pCtx_->uc_link = NULL;
	makecontext(pCtx_, func, 1, pSche); // 调用makecontext来处理pCtx_，pStack_
}

/**
  * @brief          保存上下文到pCtx_
  * 用此时的程序状态设置上下文pCtx_
  */
void Context::makeCurContext()
{
	if (nullptr == pCtx_)
	{
		pCtx_ = new struct ucontext_t;
	}
    ::getcontext(pCtx_); // 保存当前上下文到pCtx_中
}

/**
  * @brief          切换到当前上下文pCtx_
  * 将上下文保存到oldCtx中，然后切换到当前上下文
  */
void Context::swapToMe(Context* pOldCtx)
{
	if (nullptr == pOldCtx)
	{
		// 如果pOldCtx为空， 切换到当前上下文
		setcontext(pCtx_);
	}
	else
	{
		// swapcontext(__oucp,__ucp) 保存当前上下文到__oucp ， 并从__ucp设置上下文
		// 如果pOldCtx不为空， 保存上下文到pOldCtx ，并切换到当前上下文
		swapcontext(pOldCtx->getUCtx(),pCtx_);
	}
}