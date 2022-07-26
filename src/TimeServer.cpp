
#include "TimeServer.h"

TimeServer::TimeServer()
{
    //CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    struct timespec ts;
    clock_getres(CLOCK_REALTIME, &ts);
    std::cout << "constructor: clock resolution is " << ts.tv_nsec << " nsec." << std::endl;
    if (ts.tv_sec) {
        std::cout << "constructor: clock resolution in seconds is " << ts.tv_sec << " sec." << std::endl;
    }
    this->TimeOrigin.tv_sec = 0L;
    this->TimeOrigin.tv_nsec = 0L;
}

TimeServer::~TimeServer()   {           }

void TimeServer::SetTimeOrigin(void)
{
    if (clock_gettime(CLOCK_REALTIME, &TimeOrigin) != 0) {
        std::cout << "SetTimeOrigin: error return from clock_gettime." << std::endl;
    }
}

bool TimeServer::GetTimeOrigin(AbsoluteTime & origin) const
{
    origin.sec = TimeOrigin.tv_sec;
    origin.nsec = TimeOrigin.tv_nsec;
    return (origin.sec != 0) || (origin.nsec != 0);
}

double TimeServer::GetRelativeTime(void) const
{
    double answer;
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    answer = (currentTime.tv_sec-TimeOrigin.tv_sec)
            + (currentTime.tv_nsec-TimeOrigin.tv_nsec)*cmn_ns;
    return answer;
}

double TimeServer::GetAbsoluteTimeInSeconds(void) const {
    return GetAbsoluteTime().ToSeconds();
}

AbsoluteTime TimeServer::GetAbsoluteTime(void) const {
    AbsoluteTime absTime;
    RelativeToAbsolute(GetRelativeTime(), absTime);
    return absTime;
}

void TimeServer::RelativeToAbsolute(double relative, AbsoluteTime & absolute) const
{
    if (!GetTimeOrigin(absolute)) {
        std::cout << "RelativeToAbsolute: time origin not valid!" << std::endl;
    }
    double delta_sec, delta_nsec;
    delta_nsec = modf(relative, &delta_sec)/cmn_ns;
    absolute.sec += (long) delta_sec;
    absolute.nsec += (long) delta_nsec;
    if (absolute.nsec >= 1000000000L) {
        absolute.sec++;
        absolute.nsec -= 1000000000L;
    }
}

double TimeServer::AbsoluteToRelative(const AbsoluteTime & absolute) const
{
    AbsoluteTime origin;
    if (!GetTimeOrigin(origin)) {
        std::cout << "AbsoluteToRelative: time origin not valid!" << std::endl;
    }
    long delta_sec = absolute.sec - origin.sec;
    long delta_nsec = absolute.nsec - origin.nsec;
    if (delta_nsec < 0) {
        delta_sec--;
        delta_nsec += 1000000000L;
    }
    else if (delta_nsec >= 1000000000L) {
        delta_sec++;
        delta_nsec -= 1000000000L;
    }
    double answer = delta_sec + delta_nsec*cmn_ns;
    return answer;
}

double AbsoluteTime::ToSeconds(void) const
{
    return static_cast<double>(sec) + static_cast<double>(nsec) * cmn_ns;
}

void AbsoluteTime::FromSeconds(double timeInSeconds)
{
    sec = static_cast<long>(floor(timeInSeconds));
    nsec = static_cast<long>((timeInSeconds - sec) * 1000000000); // nano seconds, 10^9
}

