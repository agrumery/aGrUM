#include <time.h>
#include <windows.h> 

const __int64 GUM_DELTA_EPOCH_IN_MICROSECS= 11644473600000000;

int gettimeofday(struct timeval *tv/*in*/, void *tz/*in*/)
{
  FILETIME ft;
  __int64 tmpres = 0;
  TIME_ZONE_INFORMATION tz_winapi;

   ZeroMemory(&ft,sizeof(ft));
   ZeroMemory(&tz_winapi,sizeof(tz_winapi));

    GetSystemTimeAsFileTime(&ft);

    tmpres = ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= GUM_DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (__int32)(tmpres*0.000001);
    tv->tv_usec =(tmpres%1000000);


    //int rez=0;
    //_tzset(),don't work properly, so we use GetTimeZoneInformation
    //rez=GetTimeZoneInformation(&tz_winapi);
    //tz->tz_dsttime=(rez==2)?true:false;
    //tz->tz_minuteswest = tz_winapi.Bias + ((rez==2)?tz_winapi.DaylightBias:0);

  return 0;
}