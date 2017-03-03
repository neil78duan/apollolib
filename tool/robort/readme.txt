测试机器人食用说明。
首先要在客户端，或者服务器生成机器人行为数据（也就是记录客户真实的网络数据，然后在机器人中再次发送这些数据，来模拟真实玩家的行为）
方法：
	在客户端登录成功后，开始记录消息数据
	// log send data
	const char *msg_stream_file = "./test_robort.data";
	int length = strlen(msg_stream_file);
	if (m_pConn->ioctl(NDIOCTL_LOG_SEND_STRAM_FILE, (void*)msg_stream_file, &length) == -1) {
		nd_logmsg("log net message bin-data errror\n");
	}

	然后开始正常游戏（最好是从新的账号开始）一直玩到需要测试的等级。
然后取得客户的生成的文件，通过命令行启动测试机器人

./apollo_robort host port [connects-number] [account_start_index] [message-stream-file]

duan
2015-11-26
