#ifndef __USBCORE_ZOFFY_DEFINE_H__
#define __USBCORE_ZOFFY_DEFINE_H__

#include "USBESL.h"

typedef struct {                          // 0000h
    volatile uint32_t GOTGCTL;            /* USB Control and Status Register     0000h */
    volatile uint32_t GOTGINT;            /* USB Interrupt Register              0004h */
    volatile uint32_t GAHBCFG;            /* Core AHB Configuration Register     0008h */
    volatile uint32_t GUSBCFG;            /* Core USB Configuration Register     000Ch */
    volatile uint32_t GRSTCTL;            /* Core Reset Register                 0010h */
    volatile uint32_t GINTSTS;            /* Core Interrupt Register             0014h */
    volatile uint32_t GINTMSK;            /* Core Interrupt Mask Register        0018h */
    volatile uint32_t GRXSTSR;            /* Receive Sts Q Read Register         001Ch */
    volatile uint32_t GRXSTSP;            /* Receive Sts Q Read & POP Register   0020h */
    volatile uint32_t GRXFSIZ;            /* Receive FIFO Size Register          0024h */
    volatile uint32_t GNPTXFSIZ;          /* Non Periodic Tx FIFO Size Register  0028h */
    volatile uint32_t GNPTXSTS;           /* Non Periodic Tx FIFO/Queue Sts reg  002Ch */
    volatile uint32_t GI2CCTL;            /* Reserved                            0030h */
    volatile uint32_t GPVNDCTL;           /* Reserved                            0034h */
    volatile uint32_t GCFG;               /* General Configuration IO Register   0038h */
    volatile uint32_t GUID;               /* User ID Register                    003Ch */
    volatile uint32_t GSID;               /* System ID Register                  0040h */
    volatile uint32_t GHWCFG1;            /* Hardware Configuration Register     0044h */
    volatile uint32_t GHWCFG2;            /* Hardware Configuration Register     0048h */
    volatile uint32_t GHWCFG3;            /* Hardware Configuration Register     004Ch */
    volatile uint32_t GHWCFG4;            /* Hardware Configuration Register     0050h */
    volatile uint32_t GLPMCFG;            /*                                     0054h */
    volatile uint32_t GPWRDN;             /*                                     0058h */
    volatile uint32_t GDFIFOCFG;          /*                                     005Ch */
    volatile uint32_t GADPCTL;            /*                                     0060h */
    volatile uint32_t GREFCLK;            /*                                     0064h */
    volatile uint32_t GINTMSK2;           /*                                     0068h */
    volatile uint32_t GINTSTS2;           /*                                     006Ch */
    volatile uint32_t Reserved0070[0x24]; /* Reserved                    0070h - 00FFh */
    volatile uint32_t HPTXFSIZ;           /* Host Periodic Tx FIFO Size Reg      0100h */
    volatile uint32_t DIEPTXF[15];        /* dev Periodic Transmit FIFO          0104h */
    volatile uint32_t Reserved0140[0xB0]; /* Reserved                    0140h - 03FFh */
} USBGREGS;

typedef struct {                      // 400h
    volatile uint32_t HCFG;           /* host Configuration Register         0400h */
    volatile uint32_t HFIR;           /* host Frame Interval Register        0404h */
    volatile uint32_t HFNUM;          /* host Frame Number Register          0408h */
    volatile uint32_t Reserved0C;     /* Reserved                            040Ch */
    volatile uint32_t HPTXSTS;        /* host Periodic Transmit FIFO         0410h */
    volatile uint32_t HAINT;          /* host All Channels Interrupt         0414h */
    volatile uint32_t HAINTMSK;       /* host All Channels Interrupt Mask    0418h */
    volatile uint32_t HFLBADDR;       /*                                     041Ch */
    volatile uint32_t Reserved20[8];  /* Reserved                    0420h - 043Fh */
    volatile uint32_t HPRT;           /* host Port Control and Status        0440h */
    volatile uint32_t Reserved44[47]; /* Reserved                    0444h - 04FFh */
} USBHREGS;

typedef struct {
    volatile uint32_t HCCHAR;     /* Host Channel Characteristics Register    500h + (channel * 20h) + 00h */
    volatile uint32_t HCSPLT;     /* Host Channel Split Control Register      500h + (channel * 20h) + 04h */
    volatile uint32_t HCINT;      /* Host Channel Interrupt Register          500h + (channel * 20h) + 08h */
    volatile uint32_t HCINTMSK;   /* Host Channel Interrupt Mask Register     500h + (channel * 20h) + 0Ch */
    volatile uint32_t HCTSIZ;     /* Host Channel Transfer Size Register      500h + (channel * 20h) + 10h */
    volatile uint32_t HCDMA;      /* Host Channel DMA Address Register        500h + (channel * 20h) + 14h */
    uint32_t          Reserved18; /* Reserved                                 500h + (channel * 20h) + 18h */
    volatile uint32_t HCDMAB;     /* Host Channel DMA Buffer Address Register 500h + (channel * 20h) + 1Ch */
} USBCHREGS;

typedef struct {                       // 800h
    volatile uint32_t DCFG;            /* dev Configuration Register          0800h */
    volatile uint32_t DCTL;            /* dev Control Register                0804h */
    volatile uint32_t DSTS;            /* dev Status Register (RO)            0808h */
    volatile uint32_t Reserved0C;      /* Reserved                            080Ch */
    volatile uint32_t DIEPMSK;         /* dev IN Endpoint Mask                0810h */
    volatile uint32_t DOEPMSK;         /* dev OUT Endpoint Mask               0814h */
    volatile uint32_t DAINT;           /* dev All Endpoints Itr Reg           0818h */
    volatile uint32_t DAINTMSK;        /* dev All Endpoints Itr Mask          081Ch */
    volatile uint32_t Reserved20;      /* Reserved                            0820h */
    volatile uint32_t Reserved24;      /* Reserved                            0824h */
    volatile uint32_t DVBUSDIS;        /* dev VBUS discharge Register         0828h */
    volatile uint32_t DVBUSPULSE;      /* dev VBUS Pulse Register             082Ch */
    volatile uint32_t DTHRCTL;         /* dev thr                             0830h */
    volatile uint32_t DIEPEMPMSK;      /* dev empty msk                       0834h */
    volatile uint32_t DEACHINT;        /* dev each ep int                     0838h */
    volatile uint32_t DEACHINTMSK;     /* dev each ep int mask                083Ch */
    volatile uint32_t DIEPEACHMSK[16]; /* dev in each ep int mask             0840h */
    volatile uint32_t DOEPEACHMSK[16]; /* dev out each ep int mask            0880h */
    uint32_t          ReservedC0[16];  /* Reserved                    08C0h - 08FFh */
} USBDREGS;

typedef struct {
    volatile uint32_t DIEPCTL;    /* dev IN Endpoint Control Reg   900h + (epIndex * 20h) + 00h */
    uint32_t          Reserved04; /* Reserved                      900h + (epIndex * 20h) + 04h */
    volatile uint32_t DIEPINT;    /* dev IN Endpoint Itr Reg       900h + (epIndex * 20h) + 08h */
    uint32_t          Reserved0C; /* Reserved                      900h + (epIndex * 20h) + 0Ch */
    volatile uint32_t DIEPTSIZ;   /* IN Endpoint Txfer Size        900h + (epIndex * 20h) + 10h */
    volatile uint32_t DIEPDMA;    /* IN Endpoint DMA Reg           900h + (epIndex * 20h) + 14h */
    volatile uint32_t DTXFSTS;    /* IN Endpoint Tx FIFO Status    900h + (epIndex * 20h) + 18h */
    uint32_t          Reserved1C; /* Reserved                      900h + (epIndex * 20h) + 1Ch */
} USBIEPREGS;

typedef struct {
    volatile uint32_t DOEPCTL;    /* dev OUT Endpoint Control Reg  B00h + (epIndex * 20h) + 00h */
    uint32_t          Reserved04; /* Reserved                      B00h + (epIndex * 20h) + 04h */
    volatile uint32_t DOEPINT;    /* dev OUT Endpoint Itr Reg      B00h + (epIndex * 20h) + 08h */
    uint32_t          Reserved0C; /* Reserved                      B00h + (epIndex * 20h) + 0Ch */
    volatile uint32_t DOEPTSIZ;   /* dev OUT Endpoint Txfer Size   B00h + (epIndex * 20h) + 10h */
    volatile uint32_t DOEPDMA;    /* Device OUT DMA Address        B00h + (epIndex * 20h) + 14h */
    uint32_t          Reserved18; /* Reserved                      B00h + (epIndex * 20h) + 18h */
    uint32_t          Reserved1C; /* Reserved                      B00h + (epIndex * 20h) + 1Ch */
} USBOEPREGS;

struct USBZoffyStruct_s {
    USBGREGS          GREGS;              /* 0400h  0000h - 03FFh */
    USBHREGS          HREGS;              /* 0100h  0400h - 04FFh */
    USBCHREGS         CHREGS[16];         /* 0200h  0500h - 06FFh */
    uint32_t          Reserved0700[0x40]; /* 0100h  0600h - 07FFh */
    USBDREGS          DREGS;              /* 0100h  0800h - 08FFh */
    USBIEPREGS        IEPREGS[16];        /* 0200h  0900h - 0AFFh */
    USBOEPREGS        OEPREGS[16];        /* 0200h  0B00h - 0CFFh */
    uint32_t          Reserved0D00[0x40]; /* 0100h  0D00h - 0DFFh */
    volatile uint32_t PCGCCTL;            /* 0004h  0E00h - 0E03h */
    volatile uint32_t PCGCCTL1;           /* 0004h  0E04h - 0E07h */
    uint32_t          Reserved0E08[126];  /* 01F8h  0E08h - 0FFFh */
    volatile uint32_t FIFO[16][1024];     /* 00BCh  1000h - 4FFFh */
};

/********************  Bit definition for USB_GAHBCFG register  ********************/
#define USB_GAHBCFG_GINT_Pos     (0U)
#define USB_GAHBCFG_GINT_Msk     (0x1U << USB_GAHBCFG_GINT_Pos) /*!< 0x00000001 */
#define USB_GAHBCFG_GINT         USB_GAHBCFG_GINT_Msk           /*!< Global interrupt mask */
#define USB_GAHBCFG_HBSTLEN_Pos  (1U)
#define USB_GAHBCFG_HBSTLEN_Msk  (0xFU << USB_GAHBCFG_HBSTLEN_Pos) /*!< 0x0000001E */
#define USB_GAHBCFG_HBSTLEN      USB_GAHBCFG_HBSTLEN_Msk           /*!< Burst length/type */
#define USB_GAHBCFG_HBSTLEN_0    (0x0U << USB_GAHBCFG_HBSTLEN_Pos) /*!< Single */
#define USB_GAHBCFG_HBSTLEN_1    (0x1U << USB_GAHBCFG_HBSTLEN_Pos) /*!< INCR */
#define USB_GAHBCFG_HBSTLEN_2    (0x3U << USB_GAHBCFG_HBSTLEN_Pos) /*!< INCR4 */
#define USB_GAHBCFG_HBSTLEN_3    (0x5U << USB_GAHBCFG_HBSTLEN_Pos) /*!< INCR8 */
#define USB_GAHBCFG_HBSTLEN_4    (0x7U << USB_GAHBCFG_HBSTLEN_Pos) /*!< INCR16 */
#define USB_GAHBCFG_DMAEN_Pos    (5U)
#define USB_GAHBCFG_DMAEN_Msk    (0x1U << USB_GAHBCFG_DMAEN_Pos) /*!< 0x00000020 */
#define USB_GAHBCFG_DMAEN        USB_GAHBCFG_DMAEN_Msk           /*!< DMA enable */
#define USB_GAHBCFG_TXFELVL_Pos  (7U)
#define USB_GAHBCFG_TXFELVL_Msk  (0x1U << USB_GAHBCFG_TXFELVL_Pos) /*!< 0x00000080 */
#define USB_GAHBCFG_TXFELVL      USB_GAHBCFG_TXFELVL_Msk           /*!< TxFIFO empty level */
#define USB_GAHBCFG_PTXFELVL_Pos (8U)
#define USB_GAHBCFG_PTXFELVL_Msk (0x1U << USB_GAHBCFG_PTXFELVL_Pos) /*!< 0x00000100 */
#define USB_GAHBCFG_PTXFELVL     USB_GAHBCFG_PTXFELVL_Msk           /*!< Periodic TxFIFO empty level */

/******************** Bit definition for USB_GINTSTS register ********************/
#define USB_GINTSTS_MODE_Pos              (0U)
#define USB_GINTSTS_MODE_Msk              (0x1U << USB_GINTSTS_MODE_Pos) /*!< 0x00000001 */
#define USB_GINTSTS_MODE                  USB_GINTSTS_MODE_Msk           /*!< Current mode of operation                      */
#define USB_GINTSTS_MMIS_Pos              (1U)
#define USB_GINTSTS_MMIS_Msk              (0x1U << USB_GINTSTS_MMIS_Pos) /*!< 0x00000002 */
#define USB_GINTSTS_MMIS                  USB_GINTSTS_MMIS_Msk           /*!< Mode mismatch interrupt                        */
#define USB_GINTSTS_OTGINT_Pos            (2U)
#define USB_GINTSTS_OTGINT_Msk            (0x1U << USB_GINTSTS_OTGINT_Pos) /*!< 0x00000004 */
#define USB_GINTSTS_OTGINT                USB_GINTSTS_OTGINT_Msk           /*!< OTG interrupt                                  */
#define USB_GINTSTS_SOF_Pos               (3U)
#define USB_GINTSTS_SOF_Msk               (0x1U << USB_GINTSTS_SOF_Pos) /*!< 0x00000008 */
#define USB_GINTSTS_SOF                   USB_GINTSTS_SOF_Msk           /*!< Start of frame                                 */
#define USB_GINTSTS_RXFLVL_Pos            (4U)
#define USB_GINTSTS_RXFLVL_Msk            (0x1U << USB_GINTSTS_RXFLVL_Pos) /*!< 0x00000010 */
#define USB_GINTSTS_RXFLVL                USB_GINTSTS_RXFLVL_Msk           /*!< RxFIFO nonempty                                */
#define USB_GINTSTS_NPTXFE_Pos            (5U)
#define USB_GINTSTS_NPTXFE_Msk            (0x1U << USB_GINTSTS_NPTXFE_Pos) /*!< 0x00000020 */
#define USB_GINTSTS_NPTXFE                USB_GINTSTS_NPTXFE_Msk           /*!< Nonperiodic TxFIFO empty                       */
#define USB_GINTSTS_GINAKEFF_Pos          (6U)
#define USB_GINTSTS_GINAKEFF_Msk          (0x1U << USB_GINTSTS_GINAKEFF_Pos) /*!< 0x00000040 */
#define USB_GINTSTS_GINAKEFF              USB_GINTSTS_GINAKEFF_Msk           /*!< Global IN nonperiodic NAK effective            */
#define USB_GINTSTS_GONAKEFF_Pos          (7U)
#define USB_GINTSTS_GONAKEFF_Msk          (0x1U << USB_GINTSTS_GONAKEFF_Pos) /*!< 0x00000080 */
#define USB_GINTSTS_GONAKEFF              USB_GINTSTS_GONAKEFF_Msk           /*!< Global OUT NAK effective                       */
#define USB_GINTSTS_ESUSP_Pos             (10U)
#define USB_GINTSTS_ESUSP_Msk             (0x1U << USB_GINTSTS_ESUSP_Pos) /*!< 0x00000400 */
#define USB_GINTSTS_ESUSP                 USB_GINTSTS_ESUSP_Msk           /*!< Early suspend                                  */
#define USB_GINTSTS_USBSUSP_Pos           (11U)
#define USB_GINTSTS_USBSUSP_Msk           (0x1U << USB_GINTSTS_USBSUSP_Pos) /*!< 0x00000800 */
#define USB_GINTSTS_USBSUSP               USB_GINTSTS_USBSUSP_Msk           /*!< USB suspend                                    */
#define USB_GINTSTS_USBRST_Pos            (12U)
#define USB_GINTSTS_USBRST_Msk            (0x1U << USB_GINTSTS_USBRST_Pos) /*!< 0x00001000 */
#define USB_GINTSTS_USBRST                USB_GINTSTS_USBRST_Msk           /*!< USB reset                                      */
#define USB_GINTSTS_ENUMDNE_Pos           (13U)
#define USB_GINTSTS_ENUMDNE_Msk           (0x1U << USB_GINTSTS_ENUMDNE_Pos) /*!< 0x00002000 */
#define USB_GINTSTS_ENUMDNE               USB_GINTSTS_ENUMDNE_Msk           /*!< Enumeration done                               */
#define USB_GINTSTS_ISOODRP_Pos           (14U)
#define USB_GINTSTS_ISOODRP_Msk           (0x1U << USB_GINTSTS_ISOODRP_Pos) /*!< 0x00004000 */
#define USB_GINTSTS_ISOODRP               USB_GINTSTS_ISOODRP_Msk           /*!< Isochronous OUT packet dropped interrupt       */
#define USB_GINTSTS_EOPF_Pos              (15U)
#define USB_GINTSTS_EOPF_Msk              (0x1U << USB_GINTSTS_EOPF_Pos) /*!< 0x00008000 */
#define USB_GINTSTS_EOPF                  USB_GINTSTS_EOPF_Msk           /*!< End of periodic frame interrupt                */
#define USB_GINTSTS_IEPINT_Pos            (18U)
#define USB_GINTSTS_IEPINT_Msk            (0x1U << USB_GINTSTS_IEPINT_Pos) /*!< 0x00040000 */
#define USB_GINTSTS_IEPINT                USB_GINTSTS_IEPINT_Msk           /*!< IN endpoint interrupt                          */
#define USB_GINTSTS_OEPINT_Pos            (19U)
#define USB_GINTSTS_OEPINT_Msk            (0x1U << USB_GINTSTS_OEPINT_Pos) /*!< 0x00080000 */
#define USB_GINTSTS_OEPINT                USB_GINTSTS_OEPINT_Msk           /*!< OUT endpoint interrupt                         */
#define USB_GINTSTS_IISOIXFR_Pos          (20U)
#define USB_GINTSTS_IISOIXFR_Msk          (0x1U << USB_GINTSTS_IISOIXFR_Pos) /*!< 0x00100000 */
#define USB_GINTSTS_IISOIXFR              USB_GINTSTS_IISOIXFR_Msk           /*!< Incomplete isochronous IN transfer             */
#define USB_GINTSTS_PXFR_INCOMPISOOUT_Pos (21U)
#define USB_GINTSTS_PXFR_INCOMPISOOUT_Msk (0x1U << USB_GINTSTS_PXFR_INCOMPISOOUT_Pos) /*!< 0x00200000 */
#define USB_GINTSTS_PXFR_INCOMPISOOUT     USB_GINTSTS_PXFR_INCOMPISOOUT_Msk           /*!< Incomplete periodic transfer                   */
#define USB_GINTSTS_DATAFSUSP_Pos         (22U)
#define USB_GINTSTS_DATAFSUSP_Msk         (0x1U << USB_GINTSTS_DATAFSUSP_Pos) /*!< 0x00400000 */
#define USB_GINTSTS_DATAFSUSP             USB_GINTSTS_DATAFSUSP_Msk           /*!< Data fetch suspended                           */
#define USB_GINTSTS_PORT_Pos              (24U)
#define USB_GINTSTS_PORT_Msk              (0x1U << USB_GINTSTS_PORT_Pos) /*!< 0x01000000 */
#define USB_GINTSTS_PORT                  USB_GINTSTS_PORT_Msk           /*!< Host port interrupt                            */
#define USB_GINTSTS_CHANNEL_Pos           (25U)
#define USB_GINTSTS_CHANNEL_Msk           (0x1U << USB_GINTSTS_CHANNEL_Pos) /*!< 0x02000000 */
#define USB_GINTSTS_CHANNEL               USB_GINTSTS_CHANNEL_Msk           /*!< Host channels interrupt                        */
#define USB_GINTSTS_PTXFE_Pos             (26U)
#define USB_GINTSTS_PTXFE_Msk             (0x1U << USB_GINTSTS_PTXFE_Pos) /*!< 0x04000000 */
#define USB_GINTSTS_PTXFE                 USB_GINTSTS_PTXFE_Msk           /*!< Periodic TxFIFO empty                          */
#define USB_GINTSTS_IDCHANGE_Pos          (28U)
#define USB_GINTSTS_IDCHANGE_Msk          (0x1U << USB_GINTSTS_IDCHANGE_Pos) /*!< 0x10000000 */
#define USB_GINTSTS_IDCHANGE              USB_GINTSTS_IDCHANGE_Msk           /*!< Connector ID status change                     */
#define USB_GINTSTS_DISCINT_Pos           (29U)
#define USB_GINTSTS_DISCINT_Msk           (0x1U << USB_GINTSTS_DISCINT_Pos) /*!< 0x20000000 */
#define USB_GINTSTS_DISCINT               USB_GINTSTS_DISCINT_Msk           /*!< Disconnect detected interrupt                  */
#define USB_GINTSTS_SRQINT_Pos            (30U)
#define USB_GINTSTS_SRQINT_Msk            (0x1U << USB_GINTSTS_SRQINT_Pos) /*!< 0x40000000 */
#define USB_GINTSTS_SRQINT                USB_GINTSTS_SRQINT_Msk           /*!< Session request/new session detected interrupt */
#define USB_GINTSTS_WKUPINT_Pos           (31U)
#define USB_GINTSTS_WKUPINT_Msk           (0x1U << USB_GINTSTS_WKUPINT_Pos) /*!< 0x80000000 */
#define USB_GINTSTS_WKUPINT               USB_GINTSTS_WKUPINT_Msk           /*!< Resume/remote wakeup detected interrupt        */

/********************  Bit definition for USB_GINTMSK register  ********************/
#define USB_GINTMSK_MMISM_Pos           (1U)
#define USB_GINTMSK_MMISM_Msk           (0x1U << USB_GINTMSK_MMISM_Pos) /*!< 0x00000002 */
#define USB_GINTMSK_MMISM               USB_GINTMSK_MMISM_Msk           /*!< Mode mismatch interrupt mask                        */
#define USB_GINTMSK_OTGINT_Pos          (2U)
#define USB_GINTMSK_OTGINT_Msk          (0x1U << USB_GINTMSK_OTGINT_Pos) /*!< 0x00000004 */
#define USB_GINTMSK_OTGINT              USB_GINTMSK_OTGINT_Msk           /*!< OTG interrupt mask                                  */
#define USB_GINTMSK_SOFM_Pos            (3U)
#define USB_GINTMSK_SOFM_Msk            (0x1U << USB_GINTMSK_SOFM_Pos) /*!< 0x00000008 */
#define USB_GINTMSK_SOFM                USB_GINTMSK_SOFM_Msk           /*!< Start of frame mask                                 */
#define USB_GINTMSK_RXFLVLM_Pos         (4U)
#define USB_GINTMSK_RXFLVLM_Msk         (0x1U << USB_GINTMSK_RXFLVLM_Pos) /*!< 0x00000010 */
#define USB_GINTMSK_RXFLVLM             USB_GINTMSK_RXFLVLM_Msk           /*!< Receive FIFO nonempty mask                          */
#define USB_GINTMSK_NPTXFEM_Pos         (5U)
#define USB_GINTMSK_NPTXFEM_Msk         (0x1U << USB_GINTMSK_NPTXFEM_Pos) /*!< 0x00000020 */
#define USB_GINTMSK_NPTXFEM             USB_GINTMSK_NPTXFEM_Msk           /*!< Nonperiodic TxFIFO empty mask                       */
#define USB_GINTMSK_GINAKEFFM_Pos       (6U)
#define USB_GINTMSK_GINAKEFFM_Msk       (0x1U << USB_GINTMSK_GINAKEFFM_Pos) /*!< 0x00000040 */
#define USB_GINTMSK_GINAKEFFM           USB_GINTMSK_GINAKEFFM_Msk           /*!< Global nonperiodic IN NAK effective mask            */
#define USB_GINTMSK_GONAKEFFM_Pos       (7U)
#define USB_GINTMSK_GONAKEFFM_Msk       (0x1U << USB_GINTMSK_GONAKEFFM_Pos) /*!< 0x00000080 */
#define USB_GINTMSK_GONAKEFFM           USB_GINTMSK_GONAKEFFM_Msk           /*!< Global OUT NAK effective mask                       */
#define USB_GINTMSK_ESUSPM_Pos          (10U)
#define USB_GINTMSK_ESUSPM_Msk          (0x1U << USB_GINTMSK_ESUSPM_Pos) /*!< 0x00000400 */
#define USB_GINTMSK_ESUSPM              USB_GINTMSK_ESUSPM_Msk           /*!< Early suspend mask                                  */
#define USB_GINTMSK_USBSUSPM_Pos        (11U)
#define USB_GINTMSK_USBSUSPM_Msk        (0x1U << USB_GINTMSK_USBSUSPM_Pos) /*!< 0x00000800 */
#define USB_GINTMSK_USBSUSPM            USB_GINTMSK_USBSUSPM_Msk           /*!< USB suspend mask                                    */
#define USB_GINTMSK_USBRST_Pos          (12U)
#define USB_GINTMSK_USBRST_Msk          (0x1U << USB_GINTMSK_USBRST_Pos) /*!< 0x00001000 */
#define USB_GINTMSK_USBRST              USB_GINTMSK_USBRST_Msk           /*!< USB reset mask                                      */
#define USB_GINTMSK_ENUMDNEM_Pos        (13U)
#define USB_GINTMSK_ENUMDNEM_Msk        (0x1U << USB_GINTMSK_ENUMDNEM_Pos) /*!< 0x00002000 */
#define USB_GINTMSK_ENUMDNEM            USB_GINTMSK_ENUMDNEM_Msk           /*!< Enumeration done mask                               */
#define USB_GINTMSK_ISOODRPM_Pos        (14U)
#define USB_GINTMSK_ISOODRPM_Msk        (0x1U << USB_GINTMSK_ISOODRPM_Pos) /*!< 0x00004000 */
#define USB_GINTMSK_ISOODRPM            USB_GINTMSK_ISOODRPM_Msk           /*!< Isochronous OUT packet dropped interrupt mask       */
#define USB_GINTMSK_EOPFM_Pos           (15U)
#define USB_GINTMSK_EOPFM_Msk           (0x1U << USB_GINTMSK_EOPFM_Pos) /*!< 0x00008000 */
#define USB_GINTMSK_EOPFM               USB_GINTMSK_EOPFM_Msk           /*!< End of periodic frame interrupt mask                */
#define USB_GINTMSK_EPMISM_Pos          (17U)
#define USB_GINTMSK_EPMISM_Msk          (0x1U << USB_GINTMSK_EPMISM_Pos) /*!< 0x00020000 */
#define USB_GINTMSK_EPMISM              USB_GINTMSK_EPMISM_Msk           /*!< Endpoint mismatch interrupt mask                    */
#define USB_GINTMSK_IEPINT_Pos          (18U)
#define USB_GINTMSK_IEPINT_Msk          (0x1U << USB_GINTMSK_IEPINT_Pos) /*!< 0x00040000 */
#define USB_GINTMSK_IEPINT              USB_GINTMSK_IEPINT_Msk           /*!< IN endpoints interrupt mask                         */
#define USB_GINTMSK_OEPINT_Pos          (19U)
#define USB_GINTMSK_OEPINT_Msk          (0x1U << USB_GINTMSK_OEPINT_Pos) /*!< 0x00080000 */
#define USB_GINTMSK_OEPINT              USB_GINTMSK_OEPINT_Msk           /*!< OUT endpoints interrupt mask                        */
#define USB_GINTMSK_IISOIXFRM_Pos       (20U)
#define USB_GINTMSK_IISOIXFRM_Msk       (0x1U << USB_GINTMSK_IISOIXFRM_Pos) /*!< 0x00100000 */
#define USB_GINTMSK_IISOIXFRM           USB_GINTMSK_IISOIXFRM_Msk           /*!< Incomplete isochronous IN transfer mask             */
#define USB_GINTMSK_PXFRM_IISOOXFRM_Pos (21U)
#define USB_GINTMSK_PXFRM_IISOOXFRM_Msk (0x1U << USB_GINTMSK_PXFRM_IISOOXFRM_Pos) /*!< 0x00200000 */
#define USB_GINTMSK_PXFRM_IISOOXFRM     USB_GINTMSK_PXFRM_IISOOXFRM_Msk           /*!< Incomplete periodic transfer mask                   */
#define USB_GINTMSK_FSUSPM_Pos          (22U)
#define USB_GINTMSK_FSUSPM_Msk          (0x1U << USB_GINTMSK_FSUSPM_Pos) /*!< 0x00400000 */
#define USB_GINTMSK_FSUSPM              USB_GINTMSK_FSUSPM_Msk           /*!< Data fetch suspended mask                           */
#define USB_GINTMSK_PORTM_Pos           (24U)
#define USB_GINTMSK_PORTM_Msk           (0x1U << USB_GINTMSK_PORTM_Pos) /*!< 0x01000000 */
#define USB_GINTMSK_PORTM               USB_GINTMSK_PORTM_Msk           /*!< Host port interrupt mask                            */
#define USB_GINTMSK_CHANNELM_Pos        (25U)
#define USB_GINTMSK_CHANNELM_Msk        (0x1U << USB_GINTMSK_CHANNELM_Pos) /*!< 0x02000000 */
#define USB_GINTMSK_CHANNELM            USB_GINTMSK_CHANNELM_Msk           /*!< Host channels interrupt mask                        */
#define USB_GINTMSK_PTXFEM_Pos          (26U)
#define USB_GINTMSK_PTXFEM_Msk          (0x1U << USB_GINTMSK_PTXFEM_Pos) /*!< 0x04000000 */
#define USB_GINTMSK_PTXFEM              USB_GINTMSK_PTXFEM_Msk           /*!< Periodic TxFIFO empty mask                          */
#define USB_GINTMSK_IDCHANGEM_Pos       (28U)
#define USB_GINTMSK_IDCHANGEM_Msk       (0x1U << USB_GINTMSK_IDCHANGEM_Pos) /*!< 0x10000000 */
#define USB_GINTMSK_IDCHANGEM           USB_GINTMSK_IDCHANGEM_Msk           /*!< Connector ID status change mask                     */
#define USB_GINTMSK_DISCINT_Pos         (29U)
#define USB_GINTMSK_DISCINT_Msk         (0x1U << USB_GINTMSK_DISCINT_Pos) /*!< 0x20000000 */
#define USB_GINTMSK_DISCINT             USB_GINTMSK_DISCINT_Msk           /*!< Disconnect detected interrupt mask                  */
#define USB_GINTMSK_SRQINTM_Pos         (30U)
#define USB_GINTMSK_SRQINTM_Msk         (0x1U << USB_GINTMSK_SRQINTM_Pos) /*!< 0x40000000 */
#define USB_GINTMSK_SRQINTM             USB_GINTMSK_SRQINTM_Msk           /*!< Session request/new session detected interrupt mask */
#define USB_GINTMSK_WKUPINT_Pos         (31U)
#define USB_GINTMSK_WKUPINT_Msk         (0x1U << USB_GINTMSK_WKUPINT_Pos) /*!< 0x80000000 */
#define USB_GINTMSK_WKUPINT             USB_GINTMSK_WKUPINT_Msk           /*!< Resume/remote wakeup detected interrupt mask        */

/********************  Bit definition for USB_DCFG register  ********************/
#define USB_DCFG_DSPD_Pos         (0U)
#define USB_DCFG_DSPD_Msk         (0x3U << USB_DCFG_DSPD_Pos) /*!< 0x00000003 */
#define USB_DCFG_DSPD             USB_DCFG_DSPD_Msk           /*!< Device speed */
#define USB_DCFG_DSPD_HighSpeed   (0x0U << USB_DCFG_DSPD_Pos) /*!< 0x00000000 */
#define USB_DCFG_DSPD_FullSpeed   (0x1U << USB_DCFG_DSPD_Pos) /*!< 0x00000001 */
#define USB_DCFG_DSPD_LowSpeed    (0x2U << USB_DCFG_DSPD_Pos) /*!< 0x00000002 */
#define USB_DCFG_DSPD_FullSpeed48 (0x3U << USB_DCFG_DSPD_Pos) /*!< 0x00000002 */
#define USB_DCFG_NZLSOHSK_Pos     (2U)
#define USB_DCFG_NZLSOHSK_Msk     (0x1U << USB_DCFG_NZLSOHSK_Pos) /*!< 0x00000004 */
#define USB_DCFG_NZLSOHSK         USB_DCFG_NZLSOHSK_Msk           /*!< Nonzero-length status OUT handshake */
#define USB_DCFG_DAD_Pos          (4U)
#define USB_DCFG_DAD_Msk          (0x7FU << USB_DCFG_DAD_Pos) /*!< 0x000007F0 */
#define USB_DCFG_DAD              USB_DCFG_DAD_Msk            /*!< Device address */
#define USB_DCFG_PFIVL_Pos        (11U)
#define USB_DCFG_PFIVL_Msk        (0x3U << USB_DCFG_PFIVL_Pos) /*!< 0x00001800 */
#define USB_DCFG_PFIVL            USB_DCFG_PFIVL_Msk           /*!< Periodic (micro)frame interval */
#define USB_DCFG_PFIVL_0          (0x1U << USB_DCFG_PFIVL_Pos) /*!< 0x00000800 */
#define USB_DCFG_PFIVL_1          (0x2U << USB_DCFG_PFIVL_Pos) /*!< 0x00001000 */
#define USB_DCFG_PERSCHIVL_Pos    (24U)
#define USB_DCFG_PERSCHIVL_Msk    (0x3U << USB_DCFG_PERSCHIVL_Pos) /*!< 0x03000000 */
#define USB_DCFG_PERSCHIVL        USB_DCFG_PERSCHIVL_Msk           /*!< Periodic scheduling interval */
#define USB_DCFG_PERSCHIVL_0      (0x1U << USB_DCFG_PERSCHIVL_Pos) /*!< 0x01000000 */
#define USB_DCFG_PERSCHIVL_1      (0x2U << USB_DCFG_PERSCHIVL_Pos) /*!< 0x02000000 */

/********************  Bit definition for USB_DCTL register  ********************/
#define USB_DCTL_RWUSIG_Pos     (0U)
#define USB_DCTL_RWUSIG_Msk     (0x1U << USB_DCTL_RWUSIG_Pos) /*!< 0x00000001 */
#define USB_DCTL_RWUSIG         USB_DCTL_RWUSIG_Msk           /*!< Remote wakeup signaling */
#define USB_DCTL_DISCONNECT_Pos (1U)
#define USB_DCTL_DISCONNECT_Msk (0x1U << USB_DCTL_DISCONNECT_Pos) /*!< 0x00000002 */
#define USB_DCTL_DISCONNECT     USB_DCTL_DISCONNECT_Msk           /*!< Soft disconnect         */
#define USB_DCTL_GINSTS_Pos     (2U)
#define USB_DCTL_GINSTS_Msk     (0x1U << USB_DCTL_GINSTS_Pos) /*!< 0x00000004 */
#define USB_DCTL_GINSTS         USB_DCTL_GINSTS_Msk           /*!< Global IN NAK status    */
#define USB_DCTL_GONSTS_Pos     (3U)
#define USB_DCTL_GONSTS_Msk     (0x1U << USB_DCTL_GONSTS_Pos) /*!< 0x00000008 */
#define USB_DCTL_GONSTS         USB_DCTL_GONSTS_Msk           /*!< Global OUT NAK status   */
#define USB_DCTL_TCTL_Pos       (4U)
#define USB_DCTL_TCTL_Msk       (0x7U << USB_DCTL_TCTL_Pos) /*!< 0x00000070 */
#define USB_DCTL_TCTL           USB_DCTL_TCTL_Msk           /*!< Test control */
#define USB_DCTL_SGINAK_Pos     (7U)
#define USB_DCTL_SGINAK_Msk     (0x1U << USB_DCTL_SGINAK_Pos) /*!< 0x00000080 */
#define USB_DCTL_SGINAK         USB_DCTL_SGINAK_Msk           /*!< Set global IN NAK         */
#define USB_DCTL_CGINAK_Pos     (8U)
#define USB_DCTL_CGINAK_Msk     (0x1U << USB_DCTL_CGINAK_Pos) /*!< 0x00000100 */
#define USB_DCTL_CGINAK         USB_DCTL_CGINAK_Msk           /*!< Clear global IN NAK       */
#define USB_DCTL_SGONAK_Pos     (9U)
#define USB_DCTL_SGONAK_Msk     (0x1U << USB_DCTL_SGONAK_Pos) /*!< 0x00000200 */
#define USB_DCTL_SGONAK         USB_DCTL_SGONAK_Msk           /*!< Set global OUT NAK        */
#define USB_DCTL_CGONAK_Pos     (10U)
#define USB_DCTL_CGONAK_Msk     (0x1U << USB_DCTL_CGONAK_Pos) /*!< 0x00000400 */
#define USB_DCTL_CGONAK         USB_DCTL_CGONAK_Msk           /*!< Clear global OUT NAK      */
#define USB_DCTL_POPRGDNE_Pos   (11U)
#define USB_DCTL_POPRGDNE_Msk   (0x1U << USB_DCTL_POPRGDNE_Pos) /*!< 0x00000800 */
#define USB_DCTL_POPRGDNE       USB_DCTL_POPRGDNE_Msk           /*!< Power-on programming done */

/********************  Bit definition for USB_DAINT register  ********************/
#define USB_DAINT_IEPINT_Pos (0U)
#define USB_DAINT_IEPINT_Msk (0xFFFFU << USB_DAINT_IEPINT_Pos) /*!< 0x0000FFFF */
#define USB_DAINT_IEPINT     USB_DAINT_IEPINT_Msk              /*!< IN endpoint interrupt bits  */
#define USB_DAINT_OEPINT_Pos (16U)
#define USB_DAINT_OEPINT_Msk (0xFFFFU << USB_DAINT_OEPINT_Pos) /*!< 0xFFFF0000 */
#define USB_DAINT_OEPINT     USB_DAINT_OEPINT_Msk              /*!< OUT endpoint interrupt bits */

/********************  Bit definition for USB_DOEPCTL register  ********************/
#define USB_DOEPCTL_MPS_Pos    (0U)
#define USB_DOEPCTL_MPS_Msk    (0x7FFU << USB_DOEPCTL_MPS_Pos) /*!< 0x000007FF */
#define USB_DOEPCTL_MPS        USB_DOEPCTL_MPS_Msk             /*!< Maximum packet size */
#define USB_DOEPCTL_ACTEP_Pos  (15U)
#define USB_DOEPCTL_ACTEP_Msk  (0x1U << USB_DOEPCTL_ACTEP_Pos) /*!< 0x00008000 */
#define USB_DOEPCTL_ACTEP      USB_DOEPCTL_ACTEP_Msk           /*!< USB active endpoint */
#define USB_DOEPCTL_DPID_Pos   (16U)
#define USB_DOEPCTL_DPID_Msk   (0x1U << USB_DIEPCTL_DPID_Pos) /*!< 0x00010000 */
#define USB_DOEPCTL_DPID       USB_DIEPCTL_DPID_Msk           /*!< Even/odd frame                   */
#define USB_DOEPCTL_NAK_Pos    (17U)
#define USB_DOEPCTL_NAK_Msk    (0x1U << USB_DOEPCTL_NAK_Pos) /*!< 0x00020000 */
#define USB_DOEPCTL_NAK        USB_DOEPCTL_NAK_Msk           /*!< NAK status */
#define USB_DOEPCTL_TYPE_Pos   (18U)
#define USB_DOEPCTL_TYPE_Msk   (0x3U << USB_DOEPCTL_TYPE_Pos) /*!< 0x000C0000 */
#define USB_DOEPCTL_TYPE       USB_DOEPCTL_TYPE_Msk           /*!< Endpoint type */
#define USB_DOEPCTL_STALL_Pos  (21U)
#define USB_DOEPCTL_STALL_Msk  (0x1U << USB_DOEPCTL_STALL_Pos) /*!< 0x00200000 */
#define USB_DOEPCTL_STALL      USB_DOEPCTL_STALL_Msk           /*!< STALL handshake */
#define USB_DOEPCTL_CNAK_Pos   (26U)
#define USB_DOEPCTL_CNAK_Msk   (0x1U << USB_DOEPCTL_CNAK_Pos) /*!< 0x04000000 */
#define USB_DOEPCTL_CNAK       USB_DOEPCTL_CNAK_Msk           /*!< Clear NAK */
#define USB_DOEPCTL_SNAK_Pos   (27U)
#define USB_DOEPCTL_SNAK_Msk   (0x1U << USB_DOEPCTL_SNAK_Pos) /*!< 0x08000000 */
#define USB_DOEPCTL_SNAK       USB_DOEPCTL_SNAK_Msk           /*!< Set NAK */
#define USB_DOEPCTL_SD0PID_Pos (28U)
#define USB_DOEPCTL_SD0PID_Msk (0x1U << USB_DOEPCTL_SD0PID_Pos) /*!< 0x10000000 */
#define USB_DOEPCTL_SD0PID     USB_DOEPCTL_SD0PID_Msk           /*!< Set DATA0 PID */
#define USB_DOEPCTL_SD1PID_Pos (29U)
#define USB_DOEPCTL_SD1PID_Msk (0x1U << USB_DOEPCTL_SD1PID_Pos) /*!< 0x20000000 */
#define USB_DOEPCTL_SD1PID     USB_DOEPCTL_SD1PID_Msk           /*!< Set DATA1 PID */
#define USB_DOEPCTL_EPDIS_Pos  (30U)
#define USB_DOEPCTL_EPDIS_Msk  (0x1U << USB_DOEPCTL_EPDIS_Pos) /*!< 0x40000000 */
#define USB_DOEPCTL_EPDIS      USB_DOEPCTL_EPDIS_Msk           /*!< Endpoint disable */
#define USB_DOEPCTL_EPENA_Pos  (31U)
#define USB_DOEPCTL_EPENA_Msk  (0x1U << USB_DOEPCTL_EPENA_Pos) /*!< 0x80000000 */
#define USB_DOEPCTL_EPENA      USB_DOEPCTL_EPENA_Msk           /*!< Endpoint enable */

/********************  Bit definition for USB_DOEPINT register  ********************/
#define USB_DOEPINT_COMPLETED_Pos    (0U)
#define USB_DOEPINT_COMPLETED_Msk    (0x1U << USB_DOEPINT_COMPLETED_Pos) /*!< 0x00000001 */
#define USB_DOEPINT_COMPLETED        USB_DOEPINT_COMPLETED_Msk           /*!< Transfer completed interrupt */
#define USB_DOEPINT_DISABLED_Pos     (1U)
#define USB_DOEPINT_DISABLED_Msk     (0x1U << USB_DOEPINT_DISABLED_Pos) /*!< 0x00000002 */
#define USB_DOEPINT_DISABLED         USB_DOEPINT_DISABLED_Msk           /*!< Endpoint disabled interrupt */
#define USB_DOEPINT_SETUP_Pos        (3U)
#define USB_DOEPINT_SETUP_Msk        (0x1U << USB_DOEPINT_SETUP_Pos) /*!< 0x00000008 */
#define USB_DOEPINT_SETUP            USB_DOEPINT_SETUP_Msk           /*!< SETUP phase done */
#define USB_DOEPINT_TOKEN_Pos        (4U)
#define USB_DOEPINT_TOKEN_Msk        (0x1U << USB_DOEPINT_TOKEN_Pos) /*!< 0x00000010 */
#define USB_DOEPINT_TOKEN            USB_DOEPINT_TOKEN_Msk           /*!< OUT token received when endpoint disabled */
#define USB_DOEPINT_STATUS_Pos       (5U)
#define USB_DOEPINT_STATUS_Msk       (0x1U << USB_DOEPINT_STATUS_Pos) /*!< 0x00000020 */
#define USB_DOEPINT_STATUS           USB_DOEPINT_STATUS_Msk           /*!< STATUS phase done */
#define USB_DOEPINT_B2B_Pos          (6U)
#define USB_DOEPINT_B2B_Msk          (0x1U << USB_DOEPINT_B2B_Pos) /*!< 0x00000040 */
#define USB_DOEPINT_B2B              USB_DOEPINT_B2B_Msk           /*!< Back-to-back SETUP packets received */
#define USB_DOEPINT_NAK_Pos          (13U)
#define USB_DOEPINT_NAK_Msk          (0x1UL << USB_DIEPINT_NAK_Pos) /*!< 0x00002000 */
#define USB_DOEPINT_NAK              USB_DIEPINT_NAK_Msk            /*!< NAK interrupt */
#define USB_DOEPINT_NYET_Pos         (14U)
#define USB_DOEPINT_NYET_Msk         (0x1U << USB_DOEPINT_NYET_Pos) /*!< 0x00004000 */
#define USB_DOEPINT_NYET             USB_DOEPINT_NYET_Msk           /*!< NYET interrupt */
#define USB_DOEPINT_SETUP_PACKET_Pos (15U)
#define USB_DOEPINT_SETUP_PACKET_Msk (0x1U << USB_DOEPINT_SETUP_PACKET_Pos) /*!< 0x00008000 */
#define USB_DOEPINT_SETUP_PACKET     USB_DOEPINT_SETUP_PACKET_Msk           /*!< SETUP packet received */

/********************  Bit definition for USB_DIEPCTL register  ********************/
#define USB_DIEPCTL_MPS_Pos    (0U)
#define USB_DIEPCTL_MPS_Msk    (0x7FFU << USB_DIEPCTL_MPS_Pos) /*!< 0x000007FF */
#define USB_DIEPCTL_MPS        USB_DIEPCTL_MPS_Msk             /*!< Maximum packet size              */
#define USB_DIEPCTL_ACTIVE_Pos (15U)
#define USB_DIEPCTL_ACTIVE_Msk (0x1U << USB_DIEPCTL_ACTIVE_Pos) /*!< 0x00008000 */
#define USB_DIEPCTL_ACTIVE     USB_DIEPCTL_ACTIVE_Msk           /*!< USB active endpoint              */
#define USB_DIEPCTL_DPID_Pos   (16U)
#define USB_DIEPCTL_DPID_Msk   (0x1U << USB_DIEPCTL_DPID_Pos) /*!< 0x00010000 */
#define USB_DIEPCTL_DPID       USB_DIEPCTL_DPID_Msk           /*!< Even/odd frame                   */
#define USB_DIEPCTL_NAK_Pos    (17U)
#define USB_DIEPCTL_NAK_Msk    (0x1U << USB_DIEPCTL_NAK_Pos) /*!< 0x00020000 */
#define USB_DIEPCTL_NAK        USB_DIEPCTL_NAK_Msk           /*!< NAK status                       */
#define USB_DIEPCTL_TYPE_Pos   (18U)
#define USB_DIEPCTL_TYPE_Msk   (0x3U << USB_DIEPCTL_TYPE_Pos) /*!< 0x000C0000 */
#define USB_DIEPCTL_TYPE       USB_DIEPCTL_TYPE_Msk           /*!< Endpoint type                    */
#define USB_DIEPCTL_STALL_Pos  (21U)
#define USB_DIEPCTL_STALL_Msk  (0x1U << USB_DIEPCTL_STALL_Pos) /*!< 0x00200000 */
#define USB_DIEPCTL_STALL      USB_DIEPCTL_STALL_Msk           /*!< STALL handshake                  */
#define USB_DIEPCTL_TXFNUM_Pos (22U)
#define USB_DIEPCTL_TXFNUM_Msk (0xFU << USB_DIEPCTL_TXFNUM_Pos) /*!< 0x03C00000 */
#define USB_DIEPCTL_TXFNUM     USB_DIEPCTL_TXFNUM_Msk           /*!< TxFIFO number                    */
#define USB_DIEPCTL_CNAK_Pos   (26U)
#define USB_DIEPCTL_CNAK_Msk   (0x1U << USB_DIEPCTL_CNAK_Pos) /*!< 0x04000000 */
#define USB_DIEPCTL_CNAK       USB_DIEPCTL_CNAK_Msk           /*!< Clear NAK                        */
#define USB_DIEPCTL_SNAK_Pos   (27U)
#define USB_DIEPCTL_SNAK_Msk   (0x1U << USB_DIEPCTL_SNAK_Pos) /*!< 0x08000000 */
#define USB_DIEPCTL_SNAK       USB_DIEPCTL_SNAK_Msk           /*!< Set NAK */
#define USB_DIEPCTL_SD0PID_Pos (28U)
#define USB_DIEPCTL_SD0PID_Msk (0x1U << USB_DIEPCTL_SD0PID_Pos) /*!< 0x10000000 */
#define USB_DIEPCTL_SD0PID     USB_DIEPCTL_SD0PID_Msk           /*!< Set DATA0 PID                    */
#define USB_DIEPCTL_SD1PID_Pos (29U)
#define USB_DIEPCTL_SD1PID_Msk (0x1U << USB_DIEPCTL_SD1PID_Pos) /*!< 0x20000000 */
#define USB_DIEPCTL_SD1PID     USB_DIEPCTL_SD1PID_Msk           /*!< Set DATA1 PID                    */
#define USB_DIEPCTL_EPDIS_Pos  (30U)
#define USB_DIEPCTL_EPDIS_Msk  (0x1U << USB_DIEPCTL_EPDIS_Pos) /*!< 0x40000000 */
#define USB_DIEPCTL_EPDIS      USB_DIEPCTL_EPDIS_Msk           /*!< Endpoint disable                 */
#define USB_DIEPCTL_EPENA_Pos  (31U)
#define USB_DIEPCTL_EPENA_Msk  (0x1U << USB_DIEPCTL_EPENA_Pos) /*!< 0x80000000 */
#define USB_DIEPCTL_EPENA      USB_DIEPCTL_EPENA_Msk           /*!< Endpoint enable                  */

/********************  Bit definition for USB_DIEPINT register  ********************/
#define USB_DIEPINT_COMPLETED_Pos    (0U)
#define USB_DIEPINT_COMPLETED_Msk    (0x1UL << USB_DIEPINT_COMPLETED_Pos) /*!< 0x00000001 */
#define USB_DIEPINT_COMPLETED        USB_DIEPINT_COMPLETED_Msk            /*!< Transfer completed interrupt */
#define USB_DIEPINT_DISABLED_Pos     (1U)
#define USB_DIEPINT_DISABLED_Msk     (0x1UL << USB_DIEPINT_DISABLED_Pos) /*!< 0x00000002 */
#define USB_DIEPINT_DISABLED         USB_DIEPINT_DISABLED_Msk            /*!< Endpoint disabled interrupt */
#define USB_DIEPINT_AHBERR_Pos       (2U)
#define USB_DIEPINT_AHBERR_Msk       (0x1UL << USB_DIEPINT_AHBERR_Pos) /*!< 0x00000004 */
#define USB_DIEPINT_AHBERR           USB_DIEPINT_AHBERR_Msk            /*!< AHB Error (AHBErr) during an IN transaction */
#define USB_DIEPINT_TIMEOUT_Pos      (3U)
#define USB_DIEPINT_TIMEOUT_Msk      (0x1UL << USB_DIEPINT_TIMEOUT_Pos) /*!< 0x00000008 */
#define USB_DIEPINT_TIMEOUT          USB_DIEPINT_TIMEOUT_Msk            /*!< Timeout condition */
#define USB_DIEPINT_TOKEN_Pos        (4U)
#define USB_DIEPINT_TOKEN_Msk        (0x1UL << USB_DIEPINT_TOKEN_Pos) /*!< 0x00000010 */
#define USB_DIEPINT_TOKEN            USB_DIEPINT_TOKEN_Msk            /*!< IN token received when TxFIFO is empty */
#define USB_DIEPINT_MISMATCH_Pos     (5U)
#define USB_DIEPINT_MISMATCH_Msk     (0x1UL << USB_DIEPINT_MISMATCH_Pos) /*!< 0x00000004 */
#define USB_DIEPINT_MISMATCH         USB_DIEPINT_MISMATCH_Msk            /*!< IN token received with EP mismatch */
#define USB_DIEPINT_NAKSET_Pos       (6U)
#define USB_DIEPINT_NAKSET_Msk       (0x1UL << USB_DIEPINT_NAKSET_Pos) /*!< 0x00000040 */
#define USB_DIEPINT_NAKSET           USB_DIEPINT_NAKSET_Msk            /*!< IN endpoint NAK effective */
#define USB_DIEPINT_FIFOEMPTY_Pos    (7U)
#define USB_DIEPINT_FIFOEMPTY_Msk    (0x1UL << USB_DIEPINT_FIFOEMPTY_Pos) /*!< 0x00000080 */
#define USB_DIEPINT_FIFOEMPTY        USB_DIEPINT_FIFOEMPTY_Msk            /*!< Transmit FIFO empty */
#define USB_DIEPINT_FIFOUNDERRUN_Pos (8U)
#define USB_DIEPINT_FIFOUNDERRUN_Msk (0x1UL << USB_DIEPINT_FIFOUNDERRUN_Pos) /*!< 0x00000100 */
#define USB_DIEPINT_FIFOUNDERRUN     USB_DIEPINT_FIFOUNDERRUN_Msk            /*!< Transmit Fifo Underrun */
#define USB_DIEPINT_BNA_Pos          (9U)
#define USB_DIEPINT_BNA_Msk          (0x1UL << USB_DIEPINT_BNA_Pos) /*!< 0x00000200 */
#define USB_DIEPINT_BNA              USB_DIEPINT_BNA_Msk            /*!< Buffer not available interrupt */
#define USB_DIEPINT_PACKETDROP_Pos   (11U)
#define USB_DIEPINT_PACKETDROP_Msk   (0x1UL << USB_DIEPINT_PACKETDROP_Pos) /*!< 0x00000800 */
#define USB_DIEPINT_PACKETDROP       USB_DIEPINT_PACKETDROP_Msk            /*!< Packet dropped status */
#define USB_DIEPINT_BABBLE_Pos       (12U)
#define USB_DIEPINT_BABBLE_Msk       (0x1UL << USB_DIEPINT_BABBLE_Pos) /*!< 0x00001000 */
#define USB_DIEPINT_BABBLE           USB_DIEPINT_BABBLE_Msk            /*!< Babble error interrupt */
#define USB_DIEPINT_NAK_Pos          (13U)
#define USB_DIEPINT_NAK_Msk          (0x1UL << USB_DIEPINT_NAK_Pos) /*!< 0x00002000 */
#define USB_DIEPINT_NAK              USB_DIEPINT_NAK_Msk            /*!< NAK interrupt */
#define USB_DIEPINT_NYET_Pos         (14U)
#define USB_DIEPINT_NYET_Msk         (0x1UL << USB_DIEPIN_NYET_Pos) /*!< 0x00002000 */
#define USB_DIEPINT_NYET             USB_DIEPINT_NYET_Msk           /*!< NAK interrupt */

#define USB_GHWCFG2_DEPCOUNT_Pos  (10U)
#define USB_GHWCFG2_DEPCOUNT      (0xFU << USB_GHWCFG2_DEPCOUNT_Pos)
#define USB_GHWCFG2_HCHCOUNT_Pos  (14U)
#define USB_GHWCFG2_HCHCOUNT      (0xFU << USB_GHWCFG2_HCHCOUNT_Pos)
#define USB_GHWCFG2_DMATYPE_Pos   (3U)
#define USB_GHWCFG2_DMATYPE       (0x3U << USB_GHWCFG2_DMATYPE_Pos)
#define USB_GHWCFG2_OTGMODE_Pos   (0U)
#define USB_GHWCFG2_OTGMODE       (0x7U << USB_GHWCFG2_OTGMODE_Pos)
#define USB_GHWCFG3_FIFODEPTH_Pos (16U)
#define USB_GHWCFG3_FIFODEPTH     (0xFFFFU << USB_GHWCFG3_FIFODEPTH_Pos)
#define USB_GHWCFG4_DEDTFIFO_Pos  (25U)
#define USB_GHWCFG4_DEDTFIFO      (0x1U << USB_GHWCFG4_DEDTFIFO_Pos)

#endif // __USBCORE_ZOFFY_DEFINE_H__
