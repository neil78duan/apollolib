using System;
using UnityEngine;

using System.IO;

using System.Collections.Generic;
using System.Runtime.InteropServices;

using System.Net;
using NetMessage ;


using RESULT_T=System.Int32;  

public enum ACCOUNT_TYPE{
    ACC_FACEBOOK = 0,
    ACC_UDID,
    ACC_APOLLO,
    ACC_GAME_CENTER,
    ACC_GOOGLE_PLAY,
    ACC_OTHER_3_ACCID,
    ACC_ANYSDK,		//any sdk
    ACC_NUMBER,
};


public class apoClient {	
	/* Interface to native implementation */
#if (UNITY_IPHONE || UNITY_XBOX360) && !UNITY_EDITOR

	const string APO_DLL_NAME = "__Internal";

#else    
    const string APO_DLL_NAME = "apollo_u3d";
#endif

    [DllImport (APO_DLL_NAME)]
	private static extern  bool apoCli_init(string workingPath, string logPath, string udid, string deviceInfo);		
    
	
	[DllImport (APO_DLL_NAME)]
	private static extern  bool apoCli_CheckConnValid();//检查网络是否有效

    [DllImport (APO_DLL_NAME)]
	private static extern  void apoCli_destroy();    
    
    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_open(string host, int port);

    [DllImport (APO_DLL_NAME)]
	private static extern RESULT_T apoCli_close();

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
    public static extern RESULT_T apoCli_LoginAccount(string account, string passwd, ACCOUNT_TYPE accType = ACCOUNT_TYPE.ACC_APOLLO, int channel =0, bool bSkipAuth=false);

    [DllImport (APO_DLL_NAME)]
	public static extern  RESULT_T apoCli_CreateAccount(string userName, string passwd, int channel =0);
	
    [DllImport (APO_DLL_NAME)]
	public static extern  RESULT_T apoCli_CreateAccountOnly(string userName, string passwd, int channel =0);
	
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
	
	

    [DllImport (APO_DLL_NAME)]
	public static extern void apoCli_EnableRecvLog(bool bIsEnable);

    [DllImport (APO_DLL_NAME)]
	public static extern void apoCli_EnableSendLog(bool bIsEnable);


    [DllImport (APO_DLL_NAME)]
	public static extern void apoCli_EnableStreamRecord();


    [DllImport (APO_DLL_NAME)]
    public static extern RESULT_T apoCli_LoginOnly(string account, string passwd, ACCOUNT_TYPE accType = ACCOUNT_TYPE.ACC_APOLLO,int channel =0, bool bSkipAuth = false);

    [DllImport (APO_DLL_NAME)]
	public static extern int apoCli_GetServerList(IntPtr sessionBuf, int bufsize);//xml
	
	
    [DllImport (APO_DLL_NAME)]
    public static extern RESULT_T apoCli_SelectServer(string host, int port); // use select server
	
    [DllImport (APO_DLL_NAME)]
    public static extern int apoCli_GetRoleList(IntPtr sessionBuf, int bufsize); // xml

    [DllImport (APO_DLL_NAME)]
    public static extern RESULT_T apoCli_CreateRole(string roleName);

    [DllImport (APO_DLL_NAME)]
    public static extern RESULT_T apoCli_SelectRole(int roleId);
	
	[DllImport (APO_DLL_NAME)]
    public static extern int apoCli_SetTimeout(int timeoutValMs);
    
	[DllImport (APO_DLL_NAME)]
    public static extern int apoCli_GetRoleBeloneServerId();		//get the server id that the player will enter 

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
#if UNITY_EDITOR
#else  
	apoCli_SetTimeout(10000) ;	//time out 10 seconds
#endif
        string workPath = Application.persistentDataPath;		
        string udid = SystemInfo.deviceUniqueIdentifier;
		
        string devDesc = SystemInfo.operatingSystem;
		
        if( apoCli_init(workPath, workPath,udid, devDesc) ) {
			//apoCli_EnableStreamRecord() ;
			return true ;
		}
		return false;
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
		if (res > 0) {
			Marshal.Copy (bufPtr, body, 0, res);
		} else {
			//Debug.LogError ("res"+ res+"  error" + apoCli_GetLastError ().ToString ()+"apoCli_CheckConnValid"+apoCli_CheckConnValid());
		}
        messageId = MsgId;
        Marshal.FreeHGlobal(bufPtr);
        return res;
    }

    //this is a debug func please do no use it 
    public static void Crash()
    {
        IntPtr bufPtr = Marshal.AllocHGlobal(10);
        bufPtr = IntPtr.Zero;
        //IntPtr sessionBuf, int bufsize
        //int* messageId, IntPtr msgBody, int bufsize, int timeOutMS
        apoCli_fetchSessionKey(bufPtr, 1000);
    }

    unsafe public int Recv(out int messageId, ref NDMsgStream body, int waitMs)
    {
        int MsgId = 0 ;
        
        IntPtr bufPtr = Marshal.AllocHGlobal(0x10000);

        int res = apoCli_recvMsg(&MsgId, bufPtr, 0x10000, waitMs);
        if (res > 0)
        {
            body.FromAddr(bufPtr, res);
		}else {
			//Debug.LogError ("res"+ res+"     error" + apoCli_GetLastError ().ToString ()+"apoCli_CheckConnValid"+apoCli_CheckConnValid());
		}
        messageId = MsgId;
        Marshal.FreeHGlobal(bufPtr);
        return res;
    }
    

    public int Open(string host, int port)
    {
        RESULT_T res = apoCli_open(host, port); 
        if(res != 0 ) {
            return -1 ;
        }
        //m_netObject = get_NDNetObject() ;
        return 0 ;
    }

    public int Login(string user, string passwd, ACCOUNT_TYPE accType = ACCOUNT_TYPE.ACC_APOLLO, int channel =0, bool bSkipAuth = false)
    {
        if(m_netObject == null) {
            return -1 ;
        }
        string accName = buildAccountName(user, accType);

        int ret = apoCli_LoginAccount(accName, passwd, accType,channel, bSkipAuth);
		if(ret == 0) {			
			//apoCli_EnableStreamRecord() ;
			m_netObject = get_NDNetObject() ;
		}
		
		return ret ;
    }
    public void Logout()
    {
        apoCli_Logout();
    }

    public int Register(string user, string password, string phone, string email, int channel= 0)
    {
        if(m_netObject == null) {
            return -1 ;
        }

        int ret = apoCli_CreateAccount(user, password, channel);
		
		if(ret == 0) {			
			//apoCli_EnableStreamRecord() ;
			m_netObject = get_NDNetObject() ;
		}
		return ret ;
    }

	public int GetConnStat()
	{
		return apoCli_getConnStat ();
	}
	public int CheckConnValid()
	{
		if (apoCli_CheckConnValid ())
			return 0;
		else
			return 1;
	}
	IntPtr tokenBufPtr ;
	int tokenBuffSize;
	public int ReConnectLogin()
	{
		int result = apoCli_ReloginEx (tokenBufPtr, tokenBuffSize, true);
		//LogManager.Error("_________________ReConnectLogin________________"+result);
		if (result == 0) {
			SaveLoginToken ();
		}
		return result;
	}
	public void SaveLoginToken()
	{
		//LogManager.Error("_________________SaveLoginToken");
		tokenBufPtr = Marshal.AllocHGlobal(1000);
		tokenBuffSize =  apoCli_fetchSessionKey(tokenBufPtr, 1000);
	}
    private string buildAccountName(string inputName, ACCOUNT_TYPE accType)
    {
        string outName;
        switch (accType)
        {
            case ACCOUNT_TYPE.ACC_FACEBOOK:
                outName = @"fb&" + inputName;
                break;
            case ACCOUNT_TYPE.ACC_UDID:
                outName = @"udid&" + inputName;
                break;
            case ACCOUNT_TYPE.ACC_APOLLO:
                outName = inputName;
                break;
            case ACCOUNT_TYPE.ACC_GAME_CENTER:
                outName = @"apl&" + inputName;
                break;
            case ACCOUNT_TYPE.ACC_GOOGLE_PLAY:

                outName = @"gle&" + inputName;
                break;
            case ACCOUNT_TYPE.ACC_OTHER_3_ACCID:

                outName = @"othe&" + inputName;
                break;
            default:
                outName = inputName;
                break;
        }
        return outName;
    }
    #region for-test-api
    /*
    public bool TestLogin()
    {
        apoClient cliNet = new apoClient(); ;
        if (!cliNet.Init())
        {
            LogManager.Log("Init net error !");
            return false;
        }
        if (cliNet.Open(@"192.168.9.100", 6600) != 0)
        {
            LogManager.Log("open host error !");
            return false;
        }
        LogManager.Log("login server TEST begin");
        if (cliNet.Login(@"test60", @"123456") != 0)
        {
            LogManager.Error("login error \n");
            return false;
        }
        //send get role data message 

        if (0 != cliNet.Send(0x304))
        {
            LogManager.Error("send init data error\n");
            return false;
        }
        //receive role data 
        int messageId;

        NDMsgStream streamData = new NDMsgStream();

        int recvLen = cliNet.Recv(out messageId, ref streamData, 1000);
        if (recvLen <= 0)
        {
            LogManager.Error("read message error \n");
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
