from libcpp cimport bool
from libcpp.string cimport string


cdef extern from "okFrontPanelDLL.cpp":
    pass


cdef extern from "okFrontPanelDLL.h":
    int okFrontPanelDLL_LoadLib(const char *libname)
    void okFrontPanelDLL_FreeLib()
    void okFrontPanelDLL_GetVersion(char *date, char *time)

    cdef struct okTFlashLayout:
        unsigned int sectorCount
        unsigned int sectorSize
        unsigned int pageSize
        unsigned int minUserSector
        unsigned int maxUserSector

    cdef struct okTDeviceInfo:
        char * deviceID
        char * serialNumber
        char * productName
        int productID
        int deviceInterface
        int usbSpeed
        int deviceMajorVersion
        int deviceMinorVersion
        int hostInterfaceMajorVersion
        int hostInterfaceMinorVersion
        bool isPLL22150Supported
        bool isFrontPanelEnabled
        int wireWidth
        int triggerWidth
        int pipeWidth
        int registerAddressWidth
        int registerDataWidth

        okTFlashLayout flashSystem
        okTFlashLayout flashFPGA

    cdef enum ClockSource:
        ClkSrc_Ref,
        ClkSrc_PLL0_0,
        ClkSrc_PLL0_180,
        ClkSrc_PLL1_0,
        ClkSrc_PLL1_180,
        ClkSrc_PLL2_0,
        ClkSrc_PLL2_180

    cdef cppclass okCPLL22393:
        enum ClockSource:
            ClkSrc_Ref,
            ClkSrc_PLL0_0,
            ClkSrc_PLL0_180,
            ClkSrc_PLL1_0,
            ClkSrc_PLL1_180,
            ClkSrc_PLL2_0,
            ClkSrc_PLL2_180

        okCPLL22393();

        void SetCrystalLoad(double capload)
        void SetReference(double freq)
        double GetReference()
        bool SetPLLParameters(int n, int p, int q, bool enable=True)
        bool SetPLLLF(int n, int lf);
        bool SetOutputDivider(int n, int div);
        bool SetOutputSource(int n, ClockSource clksrc)
        void SetOutputEnable(int n, bool enable)
        int GetPLLP(int n)
        int GetPLLQ(int n)
        double GetPLLFrequency(int n)
        int GetOutputDivider(int n)
        ClockSource GetOutputSource(int n)
        double GetOutputFrequency(int n)
        bool IsOutputEnabled(int n)
        bool IsPLLEnabled(int n)
        void InitFromProgrammingInfo(unsigned char *buf)
        void GetProgrammingInfo(unsigned char *buf)

    cdef enum BoardModel:
        brdUnknown,
        brdXEM3001v1,
        brdXEM3001v2,
        brdXEM3010,
        brdXEM3005,
        brdXEM3001CL,
        brdXEM3020,
        brdXEM3050,
        brdXEM9002,
        brdXEM3001RB,
        brdXEM5010,
        brdXEM6110LX45,
        brdXEM6110LX150,
        brdXEM6001,
        brdXEM6010LX45,
        brdXEM6010LX150,
        brdXEM6006LX9,
        brdXEM6006LX16,
        brdXEM6006LX25,
        brdXEM5010LX110,
        brdZEM4310,
        brdXEM6310LX45,
        brdXEM6310LX150,
        brdXEM6110v2LX45,
        brdXEM6110v2LX150

    cdef enum ErrorCode:
        NoError,
        Failed,
        Timeout,
        DoneNotHigh,
        TransferError,
        CommunicationError,
        InvalidBitstream,
        FileError,
        DeviceNotOpen,
        InvalidEndpoint,
        InvalidBlockSize,
        I2CRestrictedAddress,
        I2CBitError,
        I2CNack,
        I2CUnknownStatus,
        UnsupportedFeature,
        FIFOUnderflow,
        FIFOOverflow,
        DataAlignmentError,
        InvalidResetProfile,
        InvalidParameter

    cdef cppclass okCFrontPanel:
        okCFrontPanel()

        BoardModel GetBoardModel()
        int GetDeviceMajorVersion();
        int GetDeviceMinorVersion();
        string GetSerialNumber();
        string GetDeviceID();

        ErrorCode OpenBySerial(string)
        ErrorCode SetPLL22393Configuration(okCPLL22393& pll);

        ErrorCode ConfigureFPGA(const string strFilename)
        bool IsFrontPanelEnabled()

        ErrorCode GetWireInValue(int epAddr, unsigned int *val)
        unsigned int GetWireOutValue(int epAddr)
        ErrorCode SetWireInValue(int ep, unsigned int val, unsigned int mask)
        void UpdateWireIns()
        void UpdateWireOuts()
