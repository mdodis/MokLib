#include "Test/Test.h"
#include "Time/TImerGroup.h"
#include "Thread/Thread.h"
#include "FileSystem/Extras.h"

TEST_CASE(
    "Lib/Time/TimerGroup", 
    "0.2s Repeat timer, sleep for 0.3s twice, calls twice")
{
    TimerGroup group(System_Allocator);
    int times_called = 0;

    auto callback = TimerGroup::Callback::create_lambda([&times_called](){
        times_called++;
    });
    u32 id = group.set_timer(0.2f, true, callback);

    REQUIRE(id != 0, "Timer ID must be valid");

    sleep_ms(300);
    group.update();
    sleep_ms(300);
    group.update();

    REQUIRE(times_called == 2, "Callback must be called twice");

    return MPASSED();
}