
#ifndef __PRXSTUB_H__
#define __PRXSTUB_H__

# include "carrier.h"
# include "CSessionManager.h"
# include "rpcerror.h"

#include <semaphore.h>

#include "eltypes.h"
#include "car.h"
#include "marshal.h"
#include "CRemoteParcel.h"

_ELASTOS_NAMESPACE_USING

#define GET_LENGTH(a) ((a) & 0x3f)
#define GET_IN_INTERFACE_MARK(a) ((a) & 0x80)


class CObjectProxy;

class CInterfaceProxy
{
public:
    static CARAPI_(PInterface) S_Probe(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI_(UInt32) S_Release(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI S_GetInterfaceID(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI BufferSize(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ UInt32 *puArgs,
            /* [out] */ UInt32 *puInSize,
            /* [out] */ UInt32 *puOutSize);

    CARAPI MarshalIn(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ UInt32 *puArgs,
            /* [in, out] */ CRemoteParcel *pParcel);

    CARAPI UnmarshalOut(
            /* [in] */ UInt32 uMethodIndex,
            /* [out] */ CRemoteParcel *pParcel,
            /* [in] */ UInt32 *puArgs);

    inline CARAPI_(UInt32) CountMethodArgs(
            /* [in] */ UInt32 uMethodIndex);

    CARAPI_(Boolean) MethodHasOutArgs(
            /* [in] */ UInt32 uMethodIndex);

    static CARAPI ProxyEntry(
            /* [in] */ UInt32 *puArgs);

public:
    PVoid                   m_pvVptr;       // must be first member
    PVoid                   m_pvProxyEntry; // must be m_pvVptr's next member

    UInt32                  m_uIndex;       // interface index in object
    CObjectProxy            *m_pOwner;
    const CIInterfaceInfo   *m_pInfo;
};

class CObjectProxy : public IProxy
{
public:
    CObjectProxy(
        /* [in] */ CSession* pSession);

    virtual ~CObjectProxy();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI GetInterface(
            /* [in] */ UInt32 uIndex,
            /* [out] */ IInterface **ppObj);

    CARAPI GetInterfaceIndex(
             /* [in] */ IInterface *pObj,
             /* [out] */UInt32* pIndex);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppInfo);

    CARAPI LinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags);

    CARAPI UnlinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags,
            /* [out] */ Boolean* result);

    CARAPI IsStubAlive(
        /* [out] */ Boolean* result);

    ECode ReceiveFromRemote(
        /* [out] */ int* type,
        /* [out] */ void** buf,
        /* [out] */ int* len);

    static CARAPI S_CreateObject(
            /* [in] */ CSession* pSession,
            /* [in] */ REMuid rclsid,
            /* [in] */ const char* stubConnName,
            /* [out] */ IProxy **ppIProxy);

private:
    class CProxyListener
        : public SessionListener
    {
    public:
        void OnSessionConnected(
            /* [in] */ CSession* pSession,
            /* [in] */ Boolean succeeded,
            /* [in] */ void* context);

        void OnSessionReceived(
            /* [in] */ CSession* pSession,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ void* context);
    };

public:

    String                  m_stubId;
    CSession                *mSession;
    CProxyListener          *mListener;
    pthread_cond_t          mCv;
    pthread_mutex_t         mWorkLock;
    DataPack*               mData;

    String              m_stubConnName;
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceProxy     *m_pInterfaces; // sizeis(m_cInterfaces)
    ICallbackConnector  *m_pICallbackConnector;

    Int32               m_cRef;

private:
    IInterface *RemoteProbe(REIID iid);
};

typedef struct InterfaceStruct {
    struct {
        UInt32 m_vTable[1];
    } *m_vPtr;
}   InterfaceStruct;

class CInterfaceStub
{
public:
    CARAPI UnmarshalIn(
            /* [in] */ const CIMethodInfo *pMethodInfo,
            /* [in] */ CRemoteParcel *pParcel,
            /* [in, out] */ MarshalHeader *pOutHeader,
            /* [in, out] */ UInt32 *puArgs);

    CARAPI MarshalOut(
            /* [in] */ UInt32 *puArgs,
            /* [in] */ MarshalHeader *pInHeader,
            /* [out] */ MarshalHeader *pOutHeader,
            /* [in] */ Boolean bOnlyReleaseIn,
            /* [in, out] */ CRemoteParcel *pParcel);

public:
    union
    {
        IInterface          *m_pObject;
        InterfaceStruct     *m_pInterface;
    };

    const CIInterfaceInfo   *m_pInfo;
};

interface IStub : public IInterface
{
    virtual CARAPI Invoke(
            /* [in] */ CSession* pSession,
            /* [in] */ void *pInData,
            /* [in] */ UInt32 uInSize,
            /* [out] */ CRemoteParcel **ppParcel) = 0;

    virtual CARAPI GetClassID(
            /* [out] */ EMuid *pClsid) = 0;

    virtual CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex) = 0;

    virtual CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo) = 0;
};

class CObjectStub : public IStub
{
public:
    CObjectStub();
    virtual ~CObjectStub();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI Invoke(
            /* [in] */ CSession* pSession,
            /* [in] */ void *pInData,
            /* [in] */ UInt32 uInSize,
            /* [out] */ CRemoteParcel **ppParcel);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo);

    static CARAPI S_CreateObject(
            /* [in] */ IInterface *pObject,
            /* [out] */ IStub **ppIStub);

private:
    class CSessionManagerListener
        : public CManagerListener
    {
    public:
        void OnSessionRequest(
            /* [in] */ ICarrier* pCarrier,
            /* [in] */ const char *from,
            /* [in] */ const char *sdp,
            /* [in] */ size_t len,
            /* [in] */ void *context);

        void OnSessionReceived(
            /* [in] */ CSession* pSession,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ void *context);
    };

private:
#if defined(_arm)
    CARAPI _Invoke(
            /* [in] */ CSession* pSession,
            /* [in] */ void *pInData,
            /* [in] */ UInt32 uInSize,
            /* [out] */ CRemoteParcel **ppParcel);
#endif

    ECode HandleGetClassInfo(CSession* pSession, void const *base, int len);

    ECode HandleInvoke(CSession* pSession, void const *base, int len);

    ECode HandleRelease(CSession* pSession, void const *base, int len);

    CSessionManager             *mSessionManager;
    CSessionManagerListener     *mSessionManagerListener;

public:
    String              m_connName;
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceStub      *m_pInterfaces; // size_is(m_cInterfaces)
    pthread_t           m_serviceThread;
    Boolean             m_bRequestToQuit;

    Int32               m_cRef;
};

extern Address s_proxyEntryAddress;
#define PROXY_ENTRY_BASE s_proxyEntryAddress
#define PROXY_ENTRY_SIZE    0x10
#define PROXY_ENTRY_MASK    0x0f
#define PROXY_ENTRY_SHIFT   4

#define PROXY_ENTRY_NUM     0x80

#ifdef _x86

inline UInt32 CalcMethodIndex(UInt32 uCallerAddr)
{
    return (uCallerAddr - PROXY_ENTRY_BASE) >> PROXY_ENTRY_SHIFT;
}

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        UInt32 n;                         \
        n = *(UInt32 *)(pret);            \
        n &= PROXY_ENTRY_MASK;          \
        n += PROXY_ENTRY_BASE + ((argc) << PROXY_ENTRY_SHIFT);  \
        *(UInt32 *)(pret) = n;            \
        return (ec);                    \
    } while (0)

#ifdef _GNUC
// void GET_REG(register, UInt32 v);
//
#define GET_REG(reg, v)                 \
    ASM("mov    %%"#reg", %0;"          \
        :"=m"(v)                        \
    )

// void SET_REG(register, UInt32 v);
//
#define SET_REG(reg, v)                 \
    ASM("mov    %0, %%"#reg";"          \
        ::"m"(v)                        \
    )

#define STUB_INVOKE_METHOD(ec, argstart, addr) \
    ASM("call   *%2;"                   \
        "mov    %%eax, %0;"             \
        "mov    %1, %%esp;"             \
        :"=m"(ec)                       \
        :"m"(argstart), "m"(addr)       \
    )

#else

// void GET_REG(register, UInt32 v);
//
#define GET_REG(reg, v)                 \
    __asm mov v, reg;

// void SET_REG(register, UInt32 v);
//
#define SET_REG(reg, v)                 \
    __asm mov reg, v;

#define STUB_INVOKE_METHOD(ec, argstart, addr)    \
    do {                                          \
        __asm call addr                      \
        __asm mov ec, eax                         \
        __asm mov esp, argstart                   \
    } while(0)

#endif
//#ifdef _GNUC
#elif defined(_arm)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        return (ec);                    \
    } while (0)

#elif defined(_mips)

inline UInt32 CalcMethodIndex(UInt32 uCallerAddr)
{
    return (uCallerAddr - PROXY_ENTRY_BASE) >> PROXY_ENTRY_SHIFT;
}

#ifdef _GNUC

#define GET_REG(reg, v)                 \
    ASM("sw\t"#reg", %0;":"=m"(v));

#define SET_REG(reg, v)                 \
    ASM("lw "#reg", %0;"::"m"(v));

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
            return  (ec);               \
    } while (0);
#elif defined(_EVC)
#define GET_REG(reg, v)                 \
    ASM("sw "#reg", 0(%0);", &v)

#define SET_REG(reg, v)                 \
    ASM("move "#reg", %0;", v)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
            return  (ec);               \
    } while (0)

#define STUB_INVOKE_METHOD(ec, puArgs, addr)    \
    ASM(                                        \
        "addiu  sp, sp, -0x20;"                 \
        "sw     s0, 0x10(sp);"                  \
        "sw     s1, 0x14(sp);"                  \
        "sw     s2, 0x18(sp);"                  \
        "or     s0, a0, zero;"                  \
        "or     s1, sp, zero;"                  \
        "or     s2, a2, zero;"                  \
        "or     sp, a1, zero;"                  \
        "lw     a0, 0x0(sp);"                   \
        "lw     a1, 0x4(sp);"                   \
        "lw     a2, 0x8(sp);"                   \
        "lw     a3, 0xc(sp);"                   \
        "jal    s2;"                            \
        "sw     v0, 0(s0);"                     \
        "or     sp, s1, zero;"                  \
        "lw     s0, 0x10(sp);"                  \
        "lw     s1, 0x14(sp);"                  \
        "lw     s2, 0x18(sp);"                  \
        "addiu  sp, sp, 0x20;"                  \
        , &ec, puArgs, addr)
#else
#error unknown architecture
#endif
#endif

#endif //__PRXSTUB_H__
