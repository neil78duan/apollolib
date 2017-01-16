using System;
using UnityEngine;

using System.IO;

using System.Collections.Generic;
using System.Runtime.InteropServices;

using System.Net;

public class NetBridge {

	
	/* Interface to native implementation */
#if UNITY_IPHONE || UNITY_XBOX360
	
	// On iOS and Xbox 360 plugins are statically linked into
	// the executable, so we have to use __Internal as the
	// library name.

    
    [DllImport ("__Internal")]
	private static extern  bool apoCli_init(string workingPath, string logPath);
    
    [DllImport ("__Internal")]
	private static extern  void apoCli_destroy();
    
    
    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_open(string host, int port, string dev_udid);
    
    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_ReloginBackground(string host, int port, string dev_udid);
    
    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_TrytoRelogin();

    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_LoginAccount(string account, string passwd);

    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_CreateAccount(string userName, string passwd, string phone, string email);

    [DllImport ("__Internal")]
	private static extern  RESULT_T apoCli_testOneKeyLogin(string host, int port, string user, string passwd);

    [DllImport ("__Internal")]
	private static extern  void apoCli_Logout();

    [DllImport ("__Internal")]
	private static extern  void apoCli_ClearLoginHistory();

    [DllImport ("__Internal")]
	private static extern  bool apoCli_Update();

    [DllImport ("__Internal")]
	private static extern  NDUINT32 apoCli_GetCurAccId();

    [DllImport ("__Internal")]
	private static extern  NDUINT32 apoCli_GetCurRoleId();

    [DllImport ("__Internal")]
	private static extern  time_t apoCli_getServerTime();
    
    ///    
    [DllImport ("__Internal")]
    private static extern IntPtr get_NDNetObject();

    //wait net message untill timeout, when you get the data ,you need handle it yourself
    [DllImport ("__Internal")]
    private static extern int ndWaitMsg(IntPtr ndNetObj, IntPtr streamBuf, int bufsize, int timeOutMS);


    [DllImport ("__Internal")]
	private static extern  int ndGetLastError(IntPtr ndNetObj);

    [DllImport ("__Internal")]
	private static extern  string ndGetLastErrorDesc(IntPtr ndNetObj);
    

#else

    // Other platforms load plugins dynamically, so pass the name
	// of the plugin's dynamic library.
        
    [DllImport ("apollo_u3d")]
	private static extern  bool apoCli_init(string workingPath, string logPath);
    
    [DllImport ("apollo_u3d")]
	private static extern  void apoCli_destroy();
    
    
    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_open(string host, int port, string dev_udid);
    
    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_ReloginBackground(string host, int port, string dev_udid);
    
    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_TrytoRelogin();

    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_LoginAccount(string account, string passwd);

    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_CreateAccount(string userName, string passwd, string phone, string email);

    [DllImport ("apollo_u3d")]
	private static extern  RESULT_T apoCli_testOneKeyLogin(string host, int port, string user, string passwd);

    [DllImport ("apollo_u3d")]
	private static extern  void apoCli_Logout();

    [DllImport ("apollo_u3d")]
	private static extern  void apoCli_ClearLoginHistory();

    [DllImport ("apollo_u3d")]
	private static extern  bool apoCli_Update();

    [DllImport ("apollo_u3d")]
	private static extern  NDUINT32 apoCli_GetCurAccId();

    [DllImport ("apollo_u3d")]
	private static extern  NDUINT32 apoCli_GetCurRoleId();

    [DllImport ("apollo_u3d")]
	private static extern  time_t apoCli_getServerTime();
    
    ///    
    [DllImport ("apollo_u3d")]
    private static extern IntPtr get_NDNetObject();

    //wait net message untill timeout, when you get the data ,you need handle it yourself
    [DllImport ("apollo_u3d")]
    private static extern int ndWaitMsg(IntPtr ndNetObj, IntPtr streamBuf, int bufsize, int timeOutMS);


    [DllImport ("apollo_u3d")]
	private static extern  int ndGetLastError(IntPtr ndNetObj);

    [DllImport ("apollo_u3d")]
	private static extern  string ndGetLastErrorDesc(IntPtr ndNetObj);

////////////////

	#endif

    #region for-test-api
    private const int accType = 2;
    //private const string sessionFile = Application.persistentDataPath + "/sessionFile.session";
   

    public static void Login(string host, int port, string user, string passwd)
    {
        string sessionFile = Application.persistentDataPath + "/sessionFile.session";
        int result = atls_login(host, port, accType, user, passwd, sessionFile);
        Debug.Log("Login---> result=" + result);
        //send test 
        test_loginRequest();
    }

    public static void Relogin(string host, int port)
    {
        string sessionFile = Application.persistentDataPath + "/sessionFile.session";
        int result = atls_relogin(host, port, sessionFile);
        Debug.Log("Relogin---> result=" + result);
    }

    public static void Register(string host, int port, string user, string passwd)
    {
        string sessionFile = Application.persistentDataPath + "/sessionFile.session";
        int result = atls_register(host, port, user, passwd, sessionFile);
        Debug.Log("Register---> result=" + result);
    }

    public static void CloseConnet()
    {

    }

    public static void SendMsg()
    {

    }

    public static void QueryMsg()
    {

    }

    public static int test_loginRequest()
    {
        const string reqString = @"hello world";
        IntPtr intPtr = Marshal.StringToHGlobalAnsi(reqString);
        //int ret = atls_echo_test( intPtr, 11);
        int ret = atls_send(2, intPtr, 11, 0);
        Marshal.FreeHGlobal(intPtr);

        //ndMsgData recv_buf = new ndMsgData() ;
        IntPtr bufPtr = Marshal.AllocHGlobal(0xffff);
        ret = ndWaitMsg(bufPtr,0x10000, 1000);
        if (ret > 0)
        {
            int len = IPAddress.NetworkToHostOrder(Marshal.ReadInt16(bufPtr, 0));
            int msgID = IPAddress.NetworkToHostOrder(Marshal.ReadInt16(bufPtr, 2));
            //read text
        }

        Marshal.FreeHGlobal(bufPtr);
        return ret;
    }

    //»Øµ÷
    #endregion
    // 
// 	[AOT.MonoPInvokeCallback (typeof (ndNetFunc))]
// 	static int ndNetMessageHandler(void *netObj, byte *msgData, int datLen )
// 	{
// 		void * msgWrapper = ndMsgInputWrapperCreate (msgData, datLen);
// 		if (msgWrapper != null) {
// 			uint ival = ndMsgWrapperReadInt32(msgWrapper) ;
// 			float fval = ndMsgWrapperReadFloat(msgWrapper) ;
// 			byte[] textArr = new byte[20] ;
// 			byte[] binArr = new byte[20] ;
// 			
// 			//ndMsgWrapperReadText(msgWrapper, textArr, 20) ;
// 			//ndMsgWrapperReadBin(msgWrapper, binArr, 20) ;
// 
// 			ndMsgInputWrapperDestroy(msgWrapper, 0) ;
// 
// 			
// 			//Console.WriteLine("get int val=", ival, " get float val = ", fval);
// 			//Console.WriteLine(" get text ", textArr);
// 			//Console.WriteLine(" get bin ", binArr);
// 
// 		}
// 
// 		//send again 
// 		SendTest(netObj) ;
// 
// 		return 0;
// 	}
// 
// 	public static int SendTest(void *netObj) 
// 	{
// 		void * oMsg = ndMsgOutputWrapperCreate (0, 2);
// 
// 		string textArr = "hello world!"; //{'h','e','l','l','o', ' ', 'w','o','r','l','d','!', 0};
// 
// 		ndMsgWrapperWriteInt32 (oMsg, 123456);
// 		ndMsgWrapperWriteFloat (oMsg, 3.1415f);
// 		//ndMsgWrapperWriteBin (oMsg, textArr, 13);
// 
// 		return ndSendWrapMsg( netObj, oMsg, 0) ;
// 
// 	}
// 
// 
// 	//test nd net connector
// 	public static int TestNetConnect(string host, int port)
// 	{
// 
// 		if (-1 == ndInitNet ()) {
// 			return -1;
// 		}
// 		void* netObj = ndOpenConnect (host, port);
// 
// 		if (netObj == null) {
// 			Debug.Log ("error connect to host !");
// 			return -1;
// 		}
// 
// 		ndNetFuncInstall(netObj,ndNetMessageHandler, 0,2) ;
// 
// 		trytoLogin (netObj);
// 		/*
// 		if (SendTest(netObj) > 0) {
// 			for (int i = 0; i < 10; i ++ ) {
// 				ndUpdateConnect(netObj,3000) ;
// 			}
// 		}*/
// 
// 		//close net 
// 		ndClostConnect (netObj);
// 		ndDeinitNet() ;
// 		return 0;
// 	}

}
