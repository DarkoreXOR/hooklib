#ifndef __SECURITY_UTILS_H
#define __SECURITY_UTILS_H

#include <Windows.h>
#include "../memory/heap.h"

BOOL
ScCreateSecurityAttributes(
    PSECURITY_ATTRIBUTES SecurityAttributes,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    PSID *Sid
);

BOOL
ScDestroySecurityAttributes(
    PSID Sid
);

#endif
