/**
 ***************************************************************************
 * \file  TimeServer.h
 * \brief TimeServer class copied from CISST library osaTimeServer
 * Used to measure time for StateTable
 * Created : 2022.07.11 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.11 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/

// Comments from CISST library
// Class for relative time
//  The osaRelativeTime class is used to manage the relationship between relative
//  and absolute time. Specifically, the GetRelativeTime method returns the number
//  of seconds that have elapsed since the time origin was set (via a call to
//  SetTimeOrigin).

//  Although it might seem possible to implement this class without OS dependencies
//  (e.g., by using osaGetAbsoluteTime), this is problematic on some platforms, such
//  as Windows, where it is necessary to synchronize multiple time sources to get
//  a high resolution time that is relative to an absolute time.  Also, even on
//  non-Windows platforms, it would not have been as efficient because the
//  native (OS-specific) "absolute time" structure would have to converted
//  to an osaAbsoluteTime structure for every call to GetRelativeTime.

//  This class has significant overlap with osaStopwatch.

#ifndef TIMESERVER_H
#define TIMESERVER_H

#include <sys/time.h>
#include <unistd.h>
#include <cmath>
#include <iostream>


const double cmn_s = 1.0;
const double cmn_ms = cmn_s / 1000.0;
const double cmn_us = cmn_ms / 1000.0;
const double cmn_ns = cmn_us / 1000.0;

struct AbsoluteTime {
    long sec;   // seconds
    long nsec;  // nano-seconds

    AbsoluteTime() : sec(0L), nsec(0L) {}
    AbsoluteTime(long seconds, long nanoseconds) : sec(seconds), nsec(nanoseconds) {}
    ~AbsoluteTime() {}

    /*! Returns absolute time in seconds relative to Jan 1, 1970. */
    double ToSeconds(void) const;

    /*! Set from time in seconds using floating point double */
    void FromSeconds(double timeInSeconds);
};

class TimeServer
{
    struct timespec TimeOrigin;

public:
    /*! Default constructor. */
    TimeServer();
    /*! Destructor */
    ~TimeServer();

    /*! Set the time origin for the relative time measurements to "now".
        Subsequent calls to GetRelativeTime will return the number of
        seconds that have elaspsed since this call was made. */
    void SetTimeOrigin(void);

    /*! Get the time origin that is in effect for the relative time
        measurements.
        \param origin Returns the current origin */
    bool GetTimeOrigin(AbsoluteTime & origin) const;

    /*! Get the number of seconds that have elapsed since the time origin.
        \returns The number of seconds */
    double GetRelativeTime(void) const;

   /*! Get the number of seconds that have elapsed since 1970.
     \returns The number of seconds */
    double GetAbsoluteTimeInSeconds(void) const;

      /*! Get the absolute time since 1970.
     \returns The number of seconds */
    AbsoluteTime GetAbsoluteTime(void) const;

    /*! Convert the specified relative time (in seconds) to an absolute
        time by adding it to the time origin.
        \param relative The relative time value in seconds (input)
        \param absolute The absolute time value (output)
    */
    void RelativeToAbsolute(double relative, AbsoluteTime & absolute) const;

    /*! Convert the specified absolute time to a relative time
        by subtracting from the time origin and converting to seconds.
        \param absolute The absolute time value (input)
        \returns The relative time value in seconds
    */
    double AbsoluteToRelative(const AbsoluteTime & absolute) const;
};

#endif // TIMESERVER_H

