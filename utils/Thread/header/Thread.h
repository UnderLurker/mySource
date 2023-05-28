#ifndef _THREAD__
#define _THREAD_

#include "Util.h"
#include "Singleton.h"
#include <deque>
#include <functional>

NAME_SPACE_START(myUtil)


class MethodThreadPool{
    SINGLETON_DECLARE(MethodThreadPool)
    typedef void(*NoArgsFunc)();
public:

private:
    deque<NoArgsFunc> list;
};

class MemberMethodThreadPool{
    SINGLETON_DECLARE(MemberMethodThreadPool)
    typedef function<void()> ;
};

NAME_SPACE_END()

#endif //!_THREAD_