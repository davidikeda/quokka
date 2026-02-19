//
// Created by Ikeda, David on 2/18/26.
//

#ifndef COMPAT_H
#define COMPAT_H

#ifdef TARGET_OS_WIN32
    #include <string.h>
    #define strcasecmp _stricmp
#else
    #include <strings.h>
#endif

#endif //COMPAT_H
