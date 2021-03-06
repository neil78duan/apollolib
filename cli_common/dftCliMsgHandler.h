/* file dftCliMsgHandler.h
 *
 * define client demo program message handler 
 *
 * create by duan 
 *
 * 2016-1-15
 */

#ifndef _DFT_CLI_MSG_HANDLER_H_
#define _DFT_CLI_MSG_HANDLER_H_

#include "ndcli/nd_iconn.h"
#ifndef WITHOUT_LOGIC_PARSER
#include "game_parser/dbldata2netstream.h"
#include "game_parser/apoGameCommon.h"
#include "logic_parser/objectBaseMgr.h"
#include "logic_parser/logicEngineRoot.h"
#endif

namespace ClientMsgHandler
{
	void InstallDftClientHandler(NDIConn *pconn) ;	
	int apollo_dft_message_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	
	int msg_get_version_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_get_rlimit_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_msg_name_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_server_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_game_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_broadcast_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_echo_time(NDIConn* pconn, nd_usermsgbuf_t *msg);

	

#ifndef WITHOUT_LOGIC_PARSER

	struct msgIDNameFormat
	{
		NDUINT16 id;
		std::string name;
		std::string format;
	};
	typedef::std::vector<msgIDNameFormat> msgIdNameFormat_vct;

	class ApoConnectScriptOwner : public apoLogicObject4Game
	{
	public:
		ApoConnectScriptOwner();
		virtual ~ApoConnectScriptOwner();
		void Destroy(int flag = 0);
		void setConn(NDIConn *conn);

		bool getOtherObject(const char*objName, LogicDataObj &val);
		virtual const char *getMsgName(int msgId);
		virtual const char *getMsgBody(int msgId);
		//bool loadDataType(const char *file);
		void LoadMsgDataTypeFromServer();


	protected:
		LogicParserEngine * getScriptHandler();
		NDIConn *m_conn;
		msgIdNameFormat_vct m_msgIdName;

	};
	
	userDefineDataType_map_t *getDataFormat(NDIConn *pconn);
	logic_print getLogFunction(NDIConn *pconn);
	std::string getWritablePath(NDIConn *pconn);
	std::string getLogPath(NDIConn *pconn);
	void* getLogFile(NDIConn *pconn);
	msgIdNameFormat_vct* getMsgIdNameFormat(NDIConn *pconn);
	ApoConnectScriptOwner *getScriptOwner(NDIConn *pconn) ;

	//read message from file, the message first file is 64bits time,
	// if time newer than @gentime pass the message to @func else send to server 
	//return true read from file ,else send to server
	bool handleMsgFirstFromFileBeforTime(NDIConn *pconn, const char *file,NDUINT64 genTime, NDUINT16 msgId, nd_conn_msg_entry func);

	int OutputMessage2File(NDIConn *pconn, const char *file, const NDIStreamMsg &inmsg);
	int ReadMessageFromFile(NDIConn *pconn, const char *file, NDOStreamMsg &omsg);
#endif


};
#endif 
