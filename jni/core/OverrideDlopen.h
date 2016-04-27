#ifndef OverrideDlopen_h
#define OverrideDlopen_h

void* uc_dlopen(const char* name, int flags);
void uc_dlopenEnsureInit(void);

#endif  // #ifndef OverrideDlopen_h
