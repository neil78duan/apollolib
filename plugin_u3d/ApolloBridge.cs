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
#if UNITY_IPHONE || UNITY_XBOX360

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

    
    [DllImport (APO_DLL_NAME)]
    private static extern unsafe RESULT_T apoCli_sendMsg(int messageId, IntPtr messageBody, int bodySize);

    [DllImport (APO_DLL_NAME)]
    private static extern unsafe RESULT_T apoCli_send(IntPtr bufferFram, int frameSize);


    [DllImport (APO_DLL_NAME)]
    private static extern unsafe int apoCli_recvMsg(int* messageId, IntPtr msgBody, int bufsize, int timeOutMS);

    [DllImport(APO_DLL_NAME)]
    private static extern unsafe int apoCli_recv(IntPtr bufferFram, int bufsize, int timeOutMS);

    [DllImport (APO_DLL_NAME)]
	private static extern RESULT_T apoCli_GetLastError();

    
    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_ReloginBackground(string host, int port, string dev_udid);
    
    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_TrytoRelogin();

    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_LoginAccount(string account, string passwd);

    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_CreateAccount(string userName, string passwd, string phone, string email);

    [DllImport (APO_DLL_NAME)]
	private static extern  RESULT_T apoCli_testOneKeyLogin(string host, int port, string user, string passwd);

    [DllImport (APO_DLL_NAME)]
	private static extern  void apoCli_Logout();

    [DllImport (APO_DLL_NAME)]
	private static extern  void apoCli_ClearLoginHistory();

    [DllImport (APO_DLL_NAME)]
	private static extern  uint apoCli_GetCurAccId();

    [DllImport (APO_DLL_NAME)]
	private static extern  uint apoCli_GetCurRoleId();
        
    ///    
    [DllImport (APO_DLL_NAME)]
    private static extern IntPtr get_NDNetObject();
    

    private IntPtr m_netObject;

    private const int accType = 2;
    //private const string sessionFile = Application.persistentDataPath + "/sessionFile.session";
   
    public bool Init() 
    {
        string workPath = Application.persistentDataPath;
        Debug.Log("begin init....");
        return apoCli_init(workPath, workPath);
    }
    
    public void Destroy() 
    {
        apoCli_destroy() ;
    }

    public unsafe int Send(int messageId, byte[]body) 
    {
        fixed (byte* pAddr = &body[0])
        {
            return apoCli_sendMsg(messageId, (IntPtr)pAddr, body.Length);
        }
    }
    public unsafe int Send(int messageId, NDMsgStream body) 
    {
        return Send(messageId, body.ToArray());
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
            //Marshal.Copy(bufPtr, body, 0, res);
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

    // 
//     public static void Login(string host, int port, string user, string passwd)
//     {
//         string sessionFile = Application.persistentDataPath + "/sessionFile.session";
//         int result = atls_login(host, port, accType, user, passwd, sessionFile);
//         Debug.Log("Login---> result=" + result);
//         //send test 
//         test_loginRequest();
//     }
// 
//     public static void Relogin(string host, int port)
//     {
//         string sessionFile = Application.persistentDataPath + "/sessionFile.session";
//         int result = atls_relogin(host, port, sessionFile);
//         Debug.Log("Relogin---> result=" + result);
//     }
// 
//     public static void Register(string host, int port, string user, string passwd)
//     {
//         string sessionFile = Application.persistentDataPath + "/sessionFile.session";
//         int result = atls_register(host, port, user, passwd, sessionFile);
//         Debug.Log("Register---> result=" + result);
//     }
// 
//     public static void CloseConnet()
//     {
// 
//     }
// 
//     public static void SendMsg()
//     {
// 
//     }
// 
//     public static void QueryMsg()
//     {
// 
//     }
// 
//     public static int test_loginRequest()
//     {
//         const string reqString = @"hello world";
//         IntPtr intPtr = Marshal.StringToHGlobalAnsi(reqString);
//         //int ret = atls_echo_test( intPtr, 11);
//         int ret = atls_send(2, intPtr, 11, 0);
//         Marshal.FreeHGlobal(intPtr);
// 
//         //ndMsgData recv_buf = new ndMsgData() ;
//         IntPtr bufPtr = Marshal.AllocHGlobal(0xffff);
//         ret = ndWaitMsg(bufPtr,0x10000, 1000);
//         if (ret > 0)
//         {
//             int len = IPAddress.NetworkToHostOrder(Marshal.ReadInt16(bufPtr, 0));
//             int msgID = IPAddress.NetworkToHostOrder(Marshal.ReadInt16(bufPtr, 2));
//             //read text
//         }
// 
//         Marshal.FreeHGlobal(bufPtr);
//         return ret;
//     }

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
