#ifndef _SPRD_RIL_H
#define _SPRD_RIL_H

#define RIL_CARD_MAX_APPS 8

typedef struct {
    int             status;     /* A RIL_DataCallFailCause, 0 which is PDP_FAIL_NONE if no error */
    int             suggestedRetryTime; /* If status != 0, this fields indicates the suggested retry
                                           back-off timer value RIL wants to override the one
                                           pre-configured in FW.
                                           The unit is miliseconds.
                                           The value < 0 means no value is suggested.
                                           The value 0 means retry should be done ASAP.
                                           The value of INT_MAX(0x7fffffff) means no retry. */
    int             cid;        /* Context ID, uniquely identifies this call */
    int             active;     /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
    char *          type;       /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6", or "PPP". If status is
                                   PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED this is the type supported
                                   such as "IP" or "IPV6" */
    char *          ifname;     /* The network interface name */
    char *          addresses;  /* A space-delimited list of addresses with optional "/" prefix length,
                                   e.g., "192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
                                   May not be empty, typically 1 IPv4 or 1 IPv6 or
                                   one of each. If the prefix length is absent the addresses
                                   are assumed to be point to point with IPv4 having a prefix
                                   length of 32 and IPv6 128. */
    char *          dnses;      /* A space-delimited list of DNS server addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty. */
    char *          gateways;   /* A space-delimited list of default gateway addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty in which case the addresses represent point
                                   to point connections. */
    char *          pcscf;    /* the Proxy Call State Control Function address
                                 via PCO(Protocol Configuration Option) for IMS client. */
    int             mtu;        /* MTU received from network
                                   Value <= 0 means network has either not sent a value or
                                   sent an invalid value */
} RIL_Data_Call_Response_v11;

/* See RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE */
typedef enum {
    PDP_FAIL_NONE = 0, /* No error, connection ok */

    /* an integer cause code defined in TS 24.008
       section 6.1.3.1.3 or TS 24.301 Release 8+ Annex B.
       If the implementation does not have access to the exact cause codes,
       then it should return one of the following values,
       as the UI layer needs to distinguish these
       cases for error notification and potential retries. */
    PDP_FAIL_OPERATOR_BARRED = 0x08,               /* no retry */
    PDP_FAIL_INSUFFICIENT_RESOURCES = 0x1A,
    PDP_FAIL_MISSING_UKNOWN_APN = 0x1B,            /* no retry */
    PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE = 0x1C,      /* no retry */
    PDP_FAIL_USER_AUTHENTICATION = 0x1D,           /* no retry */
    PDP_FAIL_ACTIVATION_REJECT_GGSN = 0x1E,        /* no retry */
    PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED = 0x1F,
    PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED = 0x20,  /* no retry */
    PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED = 0x21, /* no retry */
    PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER = 0x22,
    PDP_FAIL_NSAPI_IN_USE = 0x23,                  /* no retry */
    PDP_FAIL_REGULAR_DEACTIVATION = 0x24,          /* restart radio */
    PDP_FAIL_ONLY_IPV4_ALLOWED = 0x32,             /* no retry */
    PDP_FAIL_ONLY_IPV6_ALLOWED = 0x33,             /* no retry */
    PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED = 0x34,
    PDP_FAIL_PROTOCOL_ERRORS   = 0x6F,             /* no retry */

    /* Not mentioned in the specification */
    PDP_FAIL_VOICE_REGISTRATION_FAIL = -1,
    PDP_FAIL_DATA_REGISTRATION_FAIL = -2,

   /* reasons for data call drop - network/modem disconnect */
    PDP_FAIL_SIGNAL_LOST = -3,
    PDP_FAIL_PREF_RADIO_TECH_CHANGED = -4,/* preferred technology has changed, should retry
                                             with parameters appropriate for new technology */
    PDP_FAIL_RADIO_POWER_OFF = -5,        /* data call was disconnected because radio was resetting,
                                             powered off - no retry */
    PDP_FAIL_TETHERED_CALL_ACTIVE = -6,   /* data call was disconnected by modem because tethered
                                             mode was up on same APN/data profile - no retry until
                                             tethered call is off */

    PDP_FAIL_ERROR_UNSPECIFIED = 0xffff,  /* retry silently */
} RIL_DataCallFailCause;

typedef enum {
    RIL_REG_STATE_NOT_REG                           = 0, /*Not registered, MT is not currently searching*/
                                                         /*a new operator to register*/
    RIL_REG_STATE_HOME                              = 1, /*Registered, home network*/
    RIL_REG_STATE_SEARCHING                         = 2, /*Not registered, but MT is currently searching*/
                                                          /*a new operator to register*/
    RIL_REG_STATE_DENIED                            = 3, /*Registration denied*/
    RIL_REG_STATE_UNKNOWN                           = 4, /*Unknown*/
    RIL_REG_STATE_ROAMING                           = 5, /*Registered, roaming*/
    RIL_REG_STATE_NOT_REG_EMERGENCY_CALL_ENABLED    = 10, /*Same as 0, but indicates that emergency calls*/
                                                          /*are enabled.*/
    RIL_REG_STATE_SEARCHING_EMERGENCY_CALL_ENABLED  = 12, /*Same as 2, but indicates that emergency calls*/
                                                          /*are enabled.*/
    RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED     = 13, /*Same as 3, but indicates that emergency calls*/
                                                          /*are enabled.*/
    RIL_REG_STATE_UNKNOWN_EMERGENCY_CALL_ENABLED    = 14 /*Same as 4, but indicates that emergency calls*/
                                                         /*are enabled.*/
} RIL_RegState;

typedef enum {
    RADIO_TECH_UNKNOWN = 0,
    RADIO_TECH_GPRS = 1,
    RADIO_TECH_EDGE = 2,
    RADIO_TECH_UMTS = 3,
    RADIO_TECH_IS95A = 4,
    RADIO_TECH_IS95B = 5,
    RADIO_TECH_1xRTT =  6,
    RADIO_TECH_EVDO_0 = 7,
    RADIO_TECH_EVDO_A = 8,
    RADIO_TECH_HSDPA = 9,
    RADIO_TECH_HSUPA = 10,
    RADIO_TECH_HSPA = 11,
    RADIO_TECH_EVDO_B = 12,
    RADIO_TECH_EHRPD = 13,
    RADIO_TECH_LTE = 14,
    RADIO_TECH_HSPAP = 15, // HSPA+
    RADIO_TECH_GSM = 16, // Only supports voice
    RADIO_TECH_TD_SCDMA = 17,
    RADIO_TECH_IWLAN = 18,
    RADIO_TECH_LTE_CA = 19
} RIL_RadioTechnology;

typedef enum{
    RADIO_STATE_OFF = 0,
    RADIO_STATE_UNAVAILABLE = 1,
    RADIO_STATE_ON = 10
}RIL_RadioState;

typedef enum {
    RIL_CARDSTATE_ABSENT   = 0,
    RIL_CARDSTATE_PRESENT  = 1,
    RIL_CARDSTATE_ERROR    = 2
} RIL_CardState;

typedef enum {
    RIL_PINSTATE_UNKNOWN              = 0,
    RIL_PINSTATE_ENABLED_NOT_VERIFIED = 1,
    RIL_PINSTATE_ENABLED_VERIFIED     = 2,
    RIL_PINSTATE_DISABLED             = 3,
    RIL_PINSTATE_ENABLED_BLOCKED      = 4,
    RIL_PINSTATE_ENABLED_PERM_BLOCKED = 5
} RIL_PinState;

typedef enum {
  RIL_APPTYPE_UNKNOWN = 0,
  RIL_APPTYPE_SIM     = 1,
  RIL_APPTYPE_USIM    = 2,
  RIL_APPTYPE_RUIM    = 3,
  RIL_APPTYPE_CSIM    = 4,
  RIL_APPTYPE_ISIM    = 5
} RIL_AppType;

typedef enum {
    RIL_APPSTATE_ILLEGAL               = -1,
    RIL_APPSTATE_UNKNOWN               = 0,
    RIL_APPSTATE_DETECTED              = 1,
    RIL_APPSTATE_PIN                   = 2, 
    RIL_APPSTATE_PUK                   = 3, 
    RIL_APPSTATE_SUBSCRIPTION_PERSO    = 4,
    RIL_APPSTATE_READY                 = 5,
    RIL_APPSTATE_BLOCKED               = 6
} RIL_AppState;

typedef enum {
      RIL_PERSOSUBSTATE_UNKNOWN                   = 0, /* initial state */
      RIL_PERSOSUBSTATE_IN_PROGRESS               = 1, /* in between each lock transition */
      RIL_PERSOSUBSTATE_READY                     = 2, /* when either SIM or RUIM Perso is finished
                                                           since each app can only have 1 active perso
                                                           involved */
      RIL_PERSOSUBSTATE_SIM_NETWORK               = 3,
      RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET        = 4,
      RIL_PERSOSUBSTATE_SIM_CORPORATE             = 5,
      RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER      = 6,
      RIL_PERSOSUBSTATE_SIM_SIM                   = 7,
      RIL_PERSOSUBSTATE_SIM_NETWORK_PUK           = 8, /* The corresponding perso lock is blocked */
      RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK    = 9,
      RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK         = 10,
      RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK  = 11,
      RIL_PERSOSUBSTATE_SIM_SIM_PUK               = 12,
      RIL_PERSOSUBSTATE_RUIM_NETWORK1             = 13,
      RIL_PERSOSUBSTATE_RUIM_NETWORK2             = 14,
      RIL_PERSOSUBSTATE_RUIM_HRPD                 = 15,
      RIL_PERSOSUBSTATE_RUIM_CORPORATE            = 16,
      RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER     = 17,
      RIL_PERSOSUBSTATE_RUIM_RUIM                 = 18,
      RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK         = 19, /* The corresponding perso lock is blocked */
      RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK         = 20,
      RIL_PERSOSUBSTATE_RUIM_HRPD_PUK             = 21,
      RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK        = 22,
      RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK = 23,
      RIL_PERSOSUBSTATE_RUIM_RUIM_PUK             = 24,
       //Added for bug#242159 begin
      RIL_PERSOSUBSTATE_SIM_LOCK_FOREVER          = 25
      //Added for bug#242159 end
} RIL_PersoSubstate;

typedef struct
{
  RIL_AppType      app_type;
  RIL_AppState     app_state;
  RIL_PersoSubstate perso_substate; 
                                       
  char             *aid_ptr;        
                                       
  char             *app_label_ptr;  
  int              pin1_replaced;
  RIL_PinState     pin1;
  RIL_PinState     pin2;
} RIL_AppStatus;

typedef struct
{
  RIL_CardState card_state;
  RIL_PinState  universal_pin_state;             
  int           gsm_umts_subscription_app_index; 
  int           cdma_subscription_app_index;     
  int           ims_subscription_app_index;      
  int           num_applications;
  RIL_AppStatus applications[RIL_CARD_MAX_APPS];
} RIL_CardStatus;

//Add for cellinfo by wangcong
typedef struct 
{
    int rat;
    int mcc;
    int mnc;
    int lac;
    int ci;
    int pci;
    int frq;
    int rsrp;
}RIL_CellInfo_CCED;
//end
/*signal strength start*/
typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
} RIL_GW_SignalStrength;

typedef struct {
    int dbm;  /* Valid values are positive integers.  This value is the actual RSSI value
               * multiplied by -1.  Example: If the actual RSSI is -75, then this response
               * value will be 75.
               */
    int ecio; /* Valid values are positive integers.  This value is the actual Ec/Io multiplied
               * by -10.  Example: If the actual Ec/Io is -12.5 dB, then this response value
               * will be 125.
               */
} RIL_CDMA_SignalStrength;

typedef struct {
    int dbm;  /* Valid values are positive integers.  This value is the actual RSSI value
               * multiplied by -1.  Example: If the actual RSSI is -75, then this response
               * value will be 75.
               */
    int ecio; /* Valid values are positive integers.  This value is the actual Ec/Io multiplied
               * by -10.  Example: If the actual Ec/Io is -12.5 dB, then this response value
               * will be 125.
               */
    int signalNoiseRatio; /* Valid values are 0-8.  8 is the highest signal to noise ratio. */
} RIL_EVDO_SignalStrength;

typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int rsrp;            /* The current Reference Signal Receive Power in dBm multipled by -1.
                          * Range: 44 to 140 dBm
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.4 */
    int rsrq;            /* The current Reference Signal Receive Quality in dB multiplied by -1.
                          * Range: 20 to 3 dB.
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.7 */
    int rssnr;           /* The current reference signal signal-to-noise ratio in 0.1 dB units.
                          * Range: -200 to +300 (-200 = -20.0 dB, +300 = 30dB).
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 8.1.1 */
    int cqi;             /* The current Channel Quality Indicator.
                          * Range: 0 to 15.
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 9.2, 9.3, A.4 */
    int timingAdvance;   /* timing advance in micro seconds for a one way trip from cell to device.
                          * Approximate distance can be calculated using 300m/us * timingAdvance.
                          * Range: 0 to 0x7FFFFFFE
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP 36.321 section 6.1.3.5
                          * also: http://www.cellular-planningoptimization.com/2010/02/timing-advance-with-calculation.html */
} RIL_LTE_SignalStrength_v8;

typedef struct {
    int rscp;    /* The Received Signal Code Power in dBm multipled by -1.
                  * Range : 25 to 120
                  * INT_MAX: 0x7FFFFFFF denotes invalid value.
                  * Reference: 3GPP TS 25.123, section 9.1.1.1 */
} RIL_TD_SCDMA_SignalStrength;

typedef struct {
    RIL_GW_SignalStrength       GW_SignalStrength;
    RIL_CDMA_SignalStrength     CDMA_SignalStrength;
    RIL_EVDO_SignalStrength     EVDO_SignalStrength;
    RIL_LTE_SignalStrength_v8   LTE_SignalStrength;
    RIL_TD_SCDMA_SignalStrength TD_SCDMA_SignalStrength;
} RIL_SignalStrength_v10;
/*signal strength end*/


/*CellInfo start*/
typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
} RIL_SignalStrengthWcdma;

/** RIL_CellIdentityGsm */
typedef struct {
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 16-bit GSM Cell Identity described in TS 27.007, 0..65535, INT_MAX if unknown  */
} RIL_CellIdentityGsm;

/** RIL_CellIdentityWcdma */
typedef struct {
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int psc;    /* 9-bit UMTS Primary Scrambling Code described in TS 25.331, 0..511, INT_MAX if unknown */
} RIL_CellIdentityWcdma;

/** RIL_CellIdentityCdma */
typedef struct {
    int networkId;      /* Network Id 0..65535, INT_MAX if unknown */
    int systemId;       /* CDMA System Id 0..32767, INT_MAX if unknown  */
    int basestationId;  /* Base Station Id 0..65535, INT_MAX if unknown  */
    int longitude;      /* Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
                         * It is represented in units of 0.25 seconds and ranges from -2592000
                         * to 2592000, both values inclusive (corresponding to a range of -180
                         * to +180 degrees). INT_MAX if unknown */

    int latitude;       /* Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
                         * It is represented in units of 0.25 seconds and ranges from -1296000
                         * to 1296000, both values inclusive (corresponding to a range of -90
                         * to +90 degrees). INT_MAX if unknown */
} RIL_CellIdentityCdma;

/** RIL_CellIdentityLte */
typedef struct {
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int ci;     /* 28-bit Cell Identity described in TS ???, INT_MAX if unknown */
    int pci;    /* physical cell id 0..503, INT_MAX if unknown  */
    int tac;    /* 16-bit tracking area code, INT_MAX if unknown  */
} RIL_CellIdentityLte;

/** RIL_CellIdentityTdscdma */
typedef struct {
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int cpid;    /* 8-bit Cell Parameters ID described in TS 25.331, 0..127, INT_MAX if unknown */
} RIL_CellIdentityTdscdma;

/** RIL_CellInfoGsm */
typedef struct {
  RIL_CellIdentityGsm   cellIdentityGsm;
  RIL_GW_SignalStrength signalStrengthGsm;
} RIL_CellInfoGsm;

/** RIL_CellInfoWcdma */
typedef struct {
  RIL_CellIdentityWcdma cellIdentityWcdma;
  RIL_SignalStrengthWcdma signalStrengthWcdma;
} RIL_CellInfoWcdma;

/** RIL_CellInfoCdma */
typedef struct {
  RIL_CellIdentityCdma      cellIdentityCdma;
  RIL_CDMA_SignalStrength   signalStrengthCdma;
  RIL_EVDO_SignalStrength   signalStrengthEvdo;
} RIL_CellInfoCdma;

/** RIL_CellInfoLte */
typedef struct {
  RIL_CellIdentityLte        cellIdentityLte;
  RIL_LTE_SignalStrength_v8  signalStrengthLte;
} RIL_CellInfoLte;

/** RIL_CellInfoTdscdma */
typedef struct {
  RIL_CellIdentityTdscdma cellIdentityTdscdma;
  RIL_TD_SCDMA_SignalStrength signalStrengthTdscdma;
} RIL_CellInfoTdscdma;

// Must be the same as CellInfo.TYPE_XXX
typedef enum {
  RIL_CELL_INFO_TYPE_GSM    = 1,
  RIL_CELL_INFO_TYPE_CDMA   = 2,
  RIL_CELL_INFO_TYPE_LTE    = 3,
  RIL_CELL_INFO_TYPE_WCDMA  = 4,
  RIL_CELL_INFO_TYPE_TD_SCDMA  = 5,
} RIL_CellInfoType;

// Must be the same as CellInfo.TIMESTAMP_TYPE_XXX
typedef enum {
    RIL_TIMESTAMP_TYPE_UNKNOWN = 0,
    RIL_TIMESTAMP_TYPE_ANTENNA = 1,
    RIL_TIMESTAMP_TYPE_MODEM = 2,
    RIL_TIMESTAMP_TYPE_OEM_RIL = 3,
    RIL_TIMESTAMP_TYPE_JAVA_RIL = 4,
} RIL_TimeStampType;

typedef struct {
  RIL_CellInfoType  cellInfoType;   /* cell type for selecting from union CellInfo */
  int               registered;     /* !0 if this cell is registered 0 if not registered */
  RIL_TimeStampType timeStampType;  /* type of time stamp represented by timeStamp */
  uint64_t          timeStamp;      /* Time in nanos as returned by ril_nano_time */
  union {
    RIL_CellInfoGsm     gsm;
    RIL_CellInfoCdma    cdma;
    RIL_CellInfoLte     lte;
    RIL_CellInfoWcdma   wcdma;
    RIL_CellInfoTdscdma tdscdma;
  } CellInfo;
} RIL_CellInfo;
/*CellInfo end*/

#define IP_ADDR_SIZE 16
#define NET_INTERFACE_LENGTH       128
struct PDP_INFO {
    int status;
    int cid;
    int active;
    char type[10];
    char ifname[NET_INTERFACE_LENGTH];
    char ip[IP_ADDR_SIZE];
    char dns1addr[IP_ADDR_SIZE];
    char dns2addr[IP_ADDR_SIZE];
};

#define SETUP_DATA_PROTOCOL_IP      "IP"
#define SETUP_DATA_PROTOCOL_IPV6    "IPV6"
#define SETUP_DATA_PROTOCOL_IPV4V6  "IPV4V6"
typedef enum{
    SETUP_DATA_AUTH_NONE           = 0,
    SETUP_DATA_AUTH_PAP              = 1,
    SETUP_DATA_AUTH_CHAP            = 2,
    SETUP_DATA_AUTH_PAP_CHAP    = 3,
    SETUP_DATA_AUTH_MAX
}SETUP_DATA_AUTH_TYPE;
//Add by wangcong for Dial
/*call start*/
typedef enum {
    RIL_CALL_ACTIVE = 0,
    RIL_CALL_HOLDING = 1,
    RIL_CALL_DIALING = 2,    /* MO call only */
    RIL_CALL_ALERTING = 3,   /* MO call only */
    RIL_CALL_INCOMING = 4,   /* MT call only */
    RIL_CALL_WAITING = 5     /* MT call only */
} RIL_CallState;

/* User-to-User signaling Info activation types derived from 3GPP 23.087 v8.0 */
typedef enum {
    RIL_UUS_TYPE1_IMPLICIT = 0,
    RIL_UUS_TYPE1_REQUIRED = 1,
    RIL_UUS_TYPE1_NOT_REQUIRED = 2,
    RIL_UUS_TYPE2_REQUIRED = 3,
    RIL_UUS_TYPE2_NOT_REQUIRED = 4,
    RIL_UUS_TYPE3_REQUIRED = 5,
    RIL_UUS_TYPE3_NOT_REQUIRED = 6
} RIL_UUS_Type;
/* User-to-User Signaling Information data coding schemes. Possible values for
 * Octet 3 (Protocol Discriminator field) in the UUIE. The values have been
 * specified in section 10.5.4.25 of 3GPP TS 24.008 */
typedef enum {
    RIL_UUS_DCS_USP = 0,          /* User specified protocol */
    RIL_UUS_DCS_OSIHLP = 1,       /* OSI higher layer protocol */
    RIL_UUS_DCS_X244 = 2,         /* X.244 */
    RIL_UUS_DCS_RMCF = 3,         /* Reserved for system mangement
                                     convergence function */
    RIL_UUS_DCS_IA5c = 4          /* IA5 characters */
} RIL_UUS_DCS;

/* User-to-User Signaling Information defined in 3GPP 23.087 v8.0
 * This data is passed in RIL_ExtensionRecord and rec contains this
 * structure when type is RIL_UUS_INFO_EXT_REC */
typedef struct {
  RIL_UUS_Type    uusType;    /* UUS Type */
  RIL_UUS_DCS     uusDcs;     /* UUS Data Coding Scheme */
  int             uusLength;  /* Length of UUS Data */
  char *          uusData;    /* UUS Data */
} RIL_UUS_Info;

typedef struct {
    RIL_CallState   state;
    int             index;      /* Connection Index for use with, eg, AT+CHLD */
    int             toa;        /* type of address, eg 145 = intl */
    int            isMpty;     /* nonzero if is mpty call */
    int            isMT;       /* nonzero if call is mobile terminated */
    int             als;        /* ALS line indicator if available
                                   (0 = line 1) */
    int            isVoice;    /* nonzero if this is is a voice call */
    int            isVoicePrivacy;     /* nonzero if CDMA voice privacy mode is active */
    char *          number;     /* Remote party number */
    int             numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    char *          name;       /* Remote party name */
    int             namePresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    RIL_UUS_Info *  uusInfo;    /* NULL or Pointer to User-User Signaling Information */
} RIL_Call;
/*call end*/
#endif

