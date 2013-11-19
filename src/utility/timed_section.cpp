#include <bitcoin/utility/timed_section.hpp>

#include <bitcoin/utility/logger.hpp>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>

static void wrapped_clock_gettime(timespec *ts) {
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
}

#else

static void wrapped_clock_gettime(timespec *ts) {
    clock_gettime(CLOCK_REALTIME, &ts);
}

#endif

namespace libbitcoin {

timed_section::timed_section(
    const std::string& context, const std::string& section)
  : context_(context), section_(section)
{
    wrapped_clock_gettime(&start_);
}

timed_section::~timed_section()
{
    log_debug(context_) << section_ << " " << std::fixed << elapsed();
}

double timed_section::elapsed() const
{
    timespec end;
    wrapped_clock_gettime(&end);
    return 1e3 * (end.tv_sec - start_.tv_sec) +
        1e-6 * (end.tv_nsec - start_.tv_nsec);
}

} // namespace libbitcoin

