### 轻量级纯c++11实现的对称协程库,其中上下文切换封装的是glibc的ucontext。

后序优化：
考虑多线程情况下的线程安全队列/无锁队列；
性能对比测试；
代码优化。

### 调用接口

```
#define co_go(func) tinyco::Schedule::getSchedule()->goNewCo(func)
#define co_yield() tinyco::Schedule::getSchedule()->resumeAnotherCoroutine()
```

### namespace msocket 命名空间
#### wrap.h 封装socket，以支持简易的网络操作

### namespace tinyco 命名空间
#### class Schedule
 协程调度相关，
```
变量：
static Schedule* scheduler_; // 静态变量scheduler_ ，Schedule::getSchedule()直接使用，不需要新建对象
static std::mutex initMutex_; // 静态变量initMutex_
bool isFirstGo_; // 用于保存调度器刚开始有协程开始工作时初始状态或协程队列空之后重新有新的协程时的上下文
std::queue<Coroutine*> coroutines_; // 协程队列
Coroutine* pLastCoroutine_; // 上一个运行的协程
Coroutine* pCurCoroutine_; // 当前运行的协程
Context mainCtx_; // 用于保存调度器刚开始有协程开始工作时初始状态的上下文

void goNewCo(std::function<void()>&& func); // 根据Func创建Coroutine协程对象，并运行该协程
void goNewCo(std::function<void()>& func);

static Schedule* getSchedule(); //获取当前协程单例，实现初始化scheduler_内存和返回scheduler_指针

Coroutine* getLastCoroutine() { return pLastCoroutine_; }; // 获取上一个运行的协程

Coroutine* getCurRunningCo() { return pCurCoroutine_; }; // 获取当前正在运行的协程

void killCurCo(); // 删除当前运行的协程pCurCoroutine_ 并释放内存

Context* getMainCtx() { return &mainCtx_; } // 获取调度器的临时上下文

void resumeAnotherCoroutine(); // 从协程队列中弹出一个协程并恢复运行，调用resume
// 如果协程队列尾空，设置isFirstGo_，并清空mainCtx_

void resume(Coroutine*); // 恢复运行指定协程
// 暂停当前协程并添加到协程队列（WAITING）
// 恢复运行指定协程（RUNNING）

```

#### class Coroutine 
协程运行相关，
```
变量：
std::function<void()> coFunc_; // 该协程封装的函数
Schedule* pMySchedule_; // 调度器
int status_; // 协程状态
Context ctx_; // 上下文

构造函数，传入函数、Schedule指针，并设置协程状态为DEAD -> READY

void resume(bool isFirstCo = false); //恢复运行当前协程
将上下文保存到上一个协程/主协程的上下文，然后切换到当前上下文；

void yield(); //暂停运行当前协程，设置协程状态为 WAITING

void startFunc() { coFunc_(); }; //运行该协程的函数

Context* getCtx() { return &ctx_; } //获取该协程的上下文

```

#### class Context 
上下文相关，
```
变量：
struct ucontext_t* pCtx_; // 上下文，描述整个处理器的状态(<ucontext.h>)
void* pStack_; // pCtx_使用到的协程栈

构造函数初始化pCtx_，pStack_为nullptr，析构函数释放pCtx_，pStack_

void makeContext(void (*func)(), Schedule*); //用函数指针设置协程的上下文入口，初始化pCtx_，pStack_

void makeCurContext(); // 保存上下文到pCtx_

void swapToMe(Context* pOldCtx); // 将上下文保存到oldCtx中，然后切换到当前上下文pCtx_

struct ucontext_t* getUCtx() { return pCtx_; }; //获取当前上下文的ucontext_t指针(pCtx_)

```

#### enum coStatus 运行状态
READY,
RUNNING,
WAITING,
DEAD


#### 程序执行逻辑：
调用co_go(func)，对静态变量scheduler_（class Schedule）的goNewCo()运行一个新协程。
此操作会创建一个协程对象(class Coroutine )并运行该协程(resume)。

如果这是运行的第一个协程，会把上下文保存到scheduler_的mainCtx_临时上下文中，并开始运行任务。
如果不是第一个协程的话，resume会将当前运行到co_yield()的协程置为WAITING并添加到协程队列中，然后运行指定的协程(Coroutine::resume)。
Coroutine::resume()会将上下文保存到上一个协程的Context上下文中并恢复此协程的上下文然后继续执行。

协程执行到co_yield()会执行resumeAnotherCoroutine()恢复运行另一个协程（从队列弹出新的协程并执行）。

如果没有线程会重新置isFirstGo_为true，并清空临时上下文mainCtx_以备从头开始。

*附：
创建协程就是创建一个Coroutine对象，coroutines_是一个队列，保存所有协程对象。
Context是一个封装了上下文处理方法的类，通过使用<ucontext.h>，封装了上下文struct ucontext_t* pCtx_和栈void* pStack_。
使用makeCurContext()来保存上下文，使用swapToMe()来切换到当前上下文（保存上下文+切换到当前协程的上下文），
使用makeContext()通过函数指针设置当前context的上下文入口（协程刚开始执行READY，运行协程函数到co_yield()然后调用切换）

## C++
### DISALLOW_COPY_MOVE_AND_ASSIGN 
限制编译器自动生成拷贝构造、移动构造函数和赋值构造函数

```
#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName)                                \
  TypeName(const TypeName &) = delete;                                         \
  TypeName(const TypeName &&) = delete;                                        \
  TypeName &operator=(const TypeName &) = delete
```

### 类中的静态变量
位于全局数据区，整个程序生存时间，可以通过类名直接调用，不需要新建对象

