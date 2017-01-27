#define QUICK_MIN
#include	<sys/types.h>
#include	<sys/times.h>

unsigned	myclock()

{
struct tms hold;

(void) times(&hold);
return(hold.tms_utime);
}
