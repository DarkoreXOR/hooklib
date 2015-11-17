#include "security_utils.h"

BOOL
ScCreateSecurityAttributes(PSECURITY_ATTRIBUTES SecurityAttributes,
                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                           PSID *Sid)
{
    BOOL result;
    DWORD sidSize = 0;
    WCHAR domainName[0x100];
    DWORD domainNameLength = 0x100;
    WCHAR userName[0x400];
    DWORD userNameSize = sizeof(WCHAR) * 0x100;
    SID_NAME_USE sidNameUse;

    if (!Sid)
    {
        return FALSE;
    }

    result = GetUserNameW(userName, &userNameSize);

    if (!result)
    {
        return FALSE;
    }

    result = LookupAccountNameW(NULL,
                                userName,
                                NULL,
                                &sidSize,
                                domainName,
                                &domainNameLength,
                                &sidNameUse);

    if (result || GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return FALSE;
    }

    *Sid = (PSID)AllocMem(sidSize);

    if (!(*Sid))
    {
        return FALSE;
    }

    result = LookupAccountNameW(NULL,
                                userName,
                                *Sid,
                                &sidSize,
                                domainName,
                                &domainNameLength,
                                &sidNameUse);

    if (!result)
    {
        DeallocMem(*Sid);
        return FALSE;
    }

    result = InitializeSecurityDescriptor(SecurityDescriptor,
                                          SECURITY_DESCRIPTOR_REVISION);

    if (!result)
    {
        DeallocMem(*Sid);
        return FALSE;
    }

    result = SetSecurityDescriptorOwner(SecurityDescriptor,
                                        *Sid,
                                        TRUE);

    if (!result)
    {
        DeallocMem(*Sid);
        return FALSE;
    }

    result = SetSecurityDescriptorDacl(SecurityDescriptor,
                                       TRUE,
                                       NULL,
                                       TRUE);

    if (!result)
    {
        DeallocMem(*Sid);
        return FALSE;
    }

    SecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
    SecurityAttributes->lpSecurityDescriptor = SecurityDescriptor;
    SecurityAttributes->bInheritHandle = FALSE;

    return TRUE;
}

BOOL
ScDestroySecurityAttributes(PSID Sid)
{
    DeallocMem(Sid);
    return TRUE;
}

