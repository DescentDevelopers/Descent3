/*---------------------------------------------------------------------------
 *
 * $Workfile: Ia3dutil.h $
 *
 *---------------------------------------------------------------------------
 *$NoKeywords: $
 */

#ifndef _IA3DUTIL_H_
#define _IA3DUTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

extern HRESULT A3dInitialize(void);
extern void    A3dUninitialize(void);
extern HRESULT A3dCreate(GUID *,void **,IUnknown FAR *,DWORD); 
extern HRESULT A3dRegister(void);

#ifdef __cplusplus
};
#endif

#endif // _IA3DUTIL_H_
