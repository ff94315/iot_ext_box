#ifndef _DEBUG_H_
#define _DEBUG_H_

#include<syslog.h>

#define DEBUG

/** @brief Used to output messages.
 *The messages will include the finlname and line number, and will be sent to syslog if so configured in the config file 
 */
#define debug(level, format...)  _debug(__FILE__, __LINE__, level, format)

/** @internal */
void _debug(const char *filename, int line, int level, const char *format, ...);


#endif /* _DEBUG_H_ */

