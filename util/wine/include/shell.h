/*
 * 				Shell Library definitions
 */

#define ERROR_SUCCESS           0L
#define ERROR_BADDB             1L
#define ERROR_BADKEY            2L
#define ERROR_CANTOPEN          3L
#define ERROR_CANTREAD          4L
#define ERROR_CANTWRITE         5L
#define ERROR_OUTOFMEMORY       6L
#define ERROR_INVALID_PARAMETER 7L
#define ERROR_ACCESS_DENIED     8L

#define REG_SZ                  1           /* string type */

#define HKEY_CLASSES_ROOT       1

typedef DWORD HKEY;
typedef HKEY FAR* LPHKEY;

typedef struct tagKEYSTRUCT {
	HKEY		hKey;
	LPSTR		lpSubKey;
	DWORD		dwType;
	LPSTR		lpValue;
	struct tagKEYSTRUCT *lpPrevKey;
	struct tagKEYSTRUCT *lpNextKey;
	struct tagKEYSTRUCT *lpSubLvl;
	} KEYSTRUCT;
typedef KEYSTRUCT *LPKEYSTRUCT;

#define SE_ERR_SHARE            26
#define SE_ERR_ASSOCINCOMPLETE  27
#define SE_ERR_DDETIMEOUT       28
#define SE_ERR_DDEFAIL          29
#define SE_ERR_DDEBUSY          30
#define SE_ERR_NOASSOC          31


