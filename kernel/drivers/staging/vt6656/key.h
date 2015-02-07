

#ifndef __KEY_H__
#define __KEY_H__

#include "ttype.h"
#include "tether.h"
#include "80211mgr.h"

/*---------------------  Export Definitions -------------------------*/
#define MAX_GROUP_KEY       4
#define MAX_KEY_TABLE       11
#define MAX_KEY_LEN         32
#define AES_KEY_LEN         16


#define AUTHENTICATOR_KEY   0x10000000
#define USE_KEYRSC          0x20000000
#define PAIRWISE_KEY        0x40000000
#define TRANSMIT_KEY        0x80000000

#define GROUP_KEY           0x00000000

#define KEY_CTL_WEP         0x00
#define KEY_CTL_NONE        0x01
#define KEY_CTL_TKIP        0x02
#define KEY_CTL_CCMP        0x03
#define KEY_CTL_INVALID     0xFF


typedef struct tagSKeyItem
{
    BOOL        bKeyValid;
    unsigned long       uKeyLength;
    BYTE        abyKey[MAX_KEY_LEN];
    QWORD       KeyRSC;
    DWORD       dwTSC47_16;
    WORD        wTSC15_0;
    BYTE        byCipherSuite;
    BYTE        byReserved0;
    DWORD       dwKeyIndex;
    void *pvKeyTable;
} SKeyItem, *PSKeyItem; //64

typedef struct tagSKeyTable
{
    BYTE        abyBSSID[ETH_ALEN];  /* 6 */
    BYTE        byReserved0[2];              //8
    SKeyItem    PairwiseKey;
    SKeyItem    GroupKey[MAX_GROUP_KEY]; //64*5 = 320, 320+8=328
    DWORD       dwGTKeyIndex;            // GroupTransmitKey Index
    BOOL        bInUse;
    WORD        wKeyCtl;
    BOOL        bSoftWEP;
    BYTE        byReserved1[6];
} SKeyTable, *PSKeyTable; //352

typedef struct tagSKeyManagement
{
    SKeyTable   KeyTable[MAX_KEY_TABLE];
} SKeyManagement, *PSKeyManagement;

/*---------------------  Export Types  ------------------------------*/

/*---------------------  Export Macros ------------------------------*/

/*---------------------  Export Classes  ----------------------------*/

/*---------------------  Export Variables  --------------------------*/

/*---------------------  Export Functions  --------------------------*/

void KeyvInitTable(void *pDeviceHandler, PSKeyManagement pTable);

BOOL KeybGetKey(PSKeyManagement pTable, PBYTE pbyBSSID, DWORD dwKeyIndex,
		PSKeyItem *pKey);

BOOL KeybSetKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID,
    DWORD           dwKeyIndex,
    unsigned long           uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    );

BOOL KeybRemoveKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID,
    DWORD           dwKeyIndex
    );

BOOL KeybRemoveAllKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID
    );

void KeyvRemoveWEPKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex
    );

void KeyvRemoveAllWEPKey(
    void *pDeviceHandler,
    PSKeyManagement pTable
    );

BOOL KeybGetTransmitKey(PSKeyManagement pTable,	PBYTE pbyBSSID,	DWORD dwKeyType,
			PSKeyItem *pKey);

BOOL KeybCheckPairewiseKey(PSKeyManagement pTable, PSKeyItem *pKey);

BOOL KeybSetDefaultKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex,
    unsigned long           uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    );

BOOL KeybSetAllGroupKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex,
    unsigned long           uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    );

#endif /* __KEY_H__ */
