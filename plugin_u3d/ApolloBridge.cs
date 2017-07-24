using System;
using UnityEngine;

using System.IO;

using System.Collections.Generic;
using System.Runtime.InteropServices;

using System.Net;
using NetMessage ;


using RESULT_T=System.Int32;  


public class apoClient {	
	/* Interface to native implementation */
#if (UNITY_IPHONE || UNITY_XBOX360) && !UNITY_EDITOR

	const string APO_DLL_NAME = "__Internal";

#else    
    const string APO_DLL_NAME = "apollo_u3d";
#endif

    [DllImport (APO_DLL_NAME)]
	private static extern  bool apoCli_init(string workingPath, string logPath);
    
    [DllImport (APO_DLL_NAME)]
	private static extern  void apoCli_destroy();    
    
    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_open(string host, int port, string dev_udid);


    [DllImport(APO_DLL_NAME)]
    private static extern void apoCli_resetConnector();
    
    [DllImport (APO_DLL_NAME)]
    private static extern unsafe RESULT_T apoCli_sendMsg(int messageId, IntPtr messageBody, int bodySize);

    [DllImport (APO_DLL_NAME)]
    private static extern unsafe RESULT_T apoCli_send(IntPtr bufferFram, int frameSize);


    [DllImport (APO_DLL_NAME)]
    private static extern unsafe int apoCli_recvMsg(int* messageId, IntPtr msgBody, int bufsize, int timeOutMS);

    [DllImport(APO_DLL_NAME)]
    private static extern unsafe int apoCli_recv(IntPtr bufferFram, int bufsize, int timeOutMS);

    [DllImport (APO_DLL_NAME)]
	public static extern RESULT_T apoCli_GetLastError();

    [DllImport (APO_DLL_NAME)]
	public static extern int apoCli_getConnStat(); //0 unconnect ,1 connected, 2 login, 3 ingame ,4 GM

    
    [DllImport (APO_DLL_NAME)]
    public static extern unsafe RESULT_T apoCli_ReloginEx(IntPtr sessionData, int sessionSize, bool bReloginOffline);  //relogin without account name and password

    [DllImport (APO_DLL_NAME)]
    public static extern unsafe int apoCli_fetchSessionKey(IntPtr sessionBuf, int bufsize); //fetch the current session-info when login success

    //[DllImport (APO_DLL_NAME)]
	//public static extern  RESULT_T apoCli_ReloginBackground(string host, int port, string dev_udid);
    
    //[DllImport (APO_DLL_NAME)]
	//public static extern  RESULT_T apoCli_TrytoRelogin();

    [DllImport (APO_DLL_NAME)]
	public static extern  RESULT_T apoCli_LoginAccount(string account, string passwd);

    [DllImport (APO_DLL_NAME)]
	public static extern  RESULT_T apoCli_CreateAccount(string userName, string passwd, string phone, string email);

    //[DllImport (APO_DLL_NAME)]
	//public static extern  RESULT_T apoCli_testOneKeyLogin(string host, int port, string user, string passwd);

    [DllImport (APO_DLL_NAME)]
	public static extern  void apoCli_Logout();

    //[DllImport (APO_DLL_NAME)]
	//public static extern  void apoCli_ClearLoginHistory();

    [DllImport (APO_DLL_NAME)]
	public static extern  uint apoCli_GetCurAccId();

    [DllImport (APO_DLL_NAME)]
	public static extern  uint apoCli_GetCurRoleId();
        
    ///    
    [DllImport (APO_DLL_NAME)]
    private static extern IntPtr get_NDNetObject();
    

    private IntPtr m_netObject;

    private const int accType = 2;
    //private const string sessionFile = Application.persistentDataPath + "/sessionFile.session";

    public uint RoleID
    {
        get
        {
            return apoCli_GetCurRoleId();
        }
    }
   
    public bool Init() 
    {
        LogManager.Log("begin init....");

        string workPath = Application.persistentDataPath;
        return apoCli_init(workPath, workPath);
    }
    
    public void Destroy() 
    {
        apoCli_destroy() ;
    }

    public unsafe int Send(int messageId, NDMsgStream body) 
    {
        fixed (byte* pAddr = &body.m_buf[0])
        {
            return apoCli_sendMsg(messageId, (IntPtr)pAddr, body.ValidLength);
        }
    }
    
    public unsafe int Send(int messageId)
    {
        return apoCli_sendMsg(messageId, (IntPtr)0, 0);
    }

    unsafe public int Recv(out int messageId, ref byte[]body, int waitMs)
    {
        int MsgId = 0 ;
        
        IntPtr bufPtr = Marshal.AllocHGlobal(0x10000);

        int res = apoCli_recvMsg(&MsgId, bufPtr, 0x10000, waitMs);
        if (res > 0)
        {
            Marshal.Copy(bufPtr, body, 0, res);
        }
        messageId = MsgId;
        Marshal.FreeHGlobal(bufPtr);
        return res;
    }

    unsafe public int Recv(out int messageId, ref NDMsgStream body, int waitMs)
    {
        int MsgId = 0 ;
        
        IntPtr bufPtr = Marshal.AllocHGlobal(0x10000);

        int res = apoCli_recvMsg(&MsgId, bufPtr, 0x10000, waitMs);
        if (res > 0)
        {
            body.FromAddr(bufPtr, res);
        }
        messageId = MsgId;
        Marshal.FreeHGlobal(bufPtr);
        return res;
    }
    

    public int Open(string host, int port)
    {

        string udid = SystemInfo.deviceUniqueIdentifier;
        RESULT_T res = apoCli_open(host, port, udid); 
        if(res != 0 ) {
            return -1 ;
        }
        m_netObject = get_NDNetObject() ;
        return 0 ;
    }

    public int Login(string user, string passwd)
    {
        if(m_netObject == null) {
            return -1 ;
        }
        return apoCli_LoginAccount( user,  passwd);
    }
    public void Logout()
    {
        apoCli_Logout();
    }

    public int Register(string user, string password, string phone, string email)
    {
        if(m_netObject == null) {
            return -1 ;
        }
        return apoCli_CreateAccount(user, password, phone, email);
    }

    #region for-test-api
    /*
    public bool TestLogin()
    {
        apoClient cliNet = new apoClient(); ;
        if (!cliNet.Init())
        {
            Debug.Log("Init net error !");
            return false;
        }
        if (cliNet.Open(@"192.168.9.100", 6600) != 0)
        {
            Debug.Log("open host error !");
            return false;
        }
        Debug.Log("login server TEST begin");
        if (cliNet.Login(@"test60", @"123456") != 0)
        {
            Debug.LogError("login error \n");
            return false;
        }
        //send get role data message 

        if (0 != cliNet.Send(0x304))
        {
            Debug.LogError("send init data error\n");
            return false;
        }
        //receive role data 
        int messageId;

        NDMsgStream streamData = new NDMsgStream();

        int recvLen = cliNet.Recv(out messageId, ref streamData, 1000);
        if (recvLen <= 0)
        {
            Debug.LogError("read message error \n");
            return false;
        }

        RoleInfo roleData = new RoleInfo();

        roleData.Read(streamData);

        //send test 
        MapCompleteInfo sendMapinfo = new MapCompleteInfo();

        sendMapinfo.mapid = new AttackMap();
        sendMapinfo.mapid.mapId = 1001;
        sendMapinfo.mapid.mapType = 1;
        sendMapinfo.is_success = 0x1;
        sendMapinfo.cost_energy = 934;
        sendMapinfo.gen_energy = 651;
        sendMapinfo.usedCardCount = 2;

        sendMapinfo.usedCard = new uint[2];
        sendMapinfo.usedCard[0] = 10087;
        sendMapinfo.usedCard[1] = 10088;

        NDMsgStream sendStream = new NDMsgStream(4096);
        sendMapinfo.Write(sendStream);

        cliNet.Send(MessageIDDefine.NETMSG_ROLE_END_FIGHT_MAP_REQ, sendStream);
        return true;
    }
    */
    #endregion
    

}
