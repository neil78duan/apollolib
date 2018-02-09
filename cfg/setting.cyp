<?xml version="1.0" encoding="utf8" ?>
<alias kinds="hide">
	<aim>目标</aim>
	<index>编号</index>
	<val>数值</val>
	<var>初始值</var>
	<type>类型</type>
	<num>数量</num>
	<comp_type>比较类型</comp_type>
	<func_name>函数名</func_name>
	<event_id>事件名</event_id>
	<comment>注释说明</comment>
</alias>
	
<data_type_define kinds="hide" desc="define enum data type">
	<!-- type_operate_dest kinds="enum" enum_value="无,角色属性,背包,任务,基地,卡牌,附加属性" enum_type="text" enum_text="none,attr,package,task,home,card,addition"/ -->
	<type_compare_sub_entry kinds="enum" enum_value="其他,等于,小于,大于,小于等于,大于等于"/>
	<type_data_type kinds="enum" enum_value="整数,浮点,文本,8bits,16bits,64bits,bool,数组,time,变量,参数,当前结果,user_def,compile_time,func_name, mod_name,bin_data,user_def_array,auto,orgMsgStream"/>	
	<type_netmsg_privilege kinds="enum" enum_value="不限制,连接,登录,进入游戏,GM"/>
	<type_encode kinds="enum" enum_value="ansi,gbk,utf8"/>
	<type_time_unit kinds="enum" enum_value="秒,分钟,小时,天,月,年,星期"/>
	<type_time_cmd kinds="enum" enum_value="获得,增加,减少,计数间隔"/>
	<type_week kinds="key_value" key="星期1,星期2,星期3,星期4,星期5,星期6,星期天" value="1,2,3,4,5,6,0"/>
</data_type_define>

<blueprint_node_name kinds="hide"  desc="define exe-node and blue-printf ui relation">
	<default>Common</default>
	<func_node>FuncEntry</func_node>
	<event_callback_node>FuncEntry</event_callback_node>
	<msg_handler_node>FuncEntry</msg_handler_node>	
	<function_init_block>ModuleInitEntry</function_init_block>
	<exception_catch_block>ExceptionEntry</exception_catch_block>
	<op_make_var>NewVar</op_make_var>
	<op_global_var>NewVar</op_global_var>
	
	<op_build_json_data>NewVar</op_build_json_data>	
	<!--op_build_json_data>NewJson</op_build_json_data>
	<op_create_struct_type>NewStruct</op_create_struct_type>
	<op_assignin>Assignment</op_assignin -->
	<op_short_jump>Break</op_short_jump>
	<op_success_short_jump>TrueBreak</op_success_short_jump>
	<op_error_short_jump>FalseBreak</op_error_short_jump>
	<op_exit>Quit</op_exit>
	<op_loop_entry>Loop</op_loop_entry>
	<op_bool_entry>Bool</op_bool_entry>
	<op_compound_test>CompoundTest</op_compound_test>
	
	<steps_block_valcomp>ValueComp</steps_block_valcomp>
	<op_throw>Throw</op_throw>
	<steps_switch_case>Switch</steps_switch_case>
	<steps_bt_selector>Selector</steps_bt_selector>
	<steps_bt_sequence>Selector</steps_bt_sequence>
	
</blueprint_node_name>


<blueprint_show_action kinds="hide" >
	<moduleInfo>detail</moduleInfo>
	<func_node>function</func_node>
	<msg_handler_node>function</msg_handler_node>
	<event_callback_node>function</event_callback_node>
</blueprint_show_action>

<compile_setting kinds="hide" desc="编译设定,关键字和指令的对照表而已 instruction=1是命令 0是变量 2分支,3循环,4变量集合,5user-define,6function-info,7错误处理 100注释">
	<list instruction="1" ins_id="0" size="8" >op_exit</list>
	<list instruction="1" ins_id="1" size="0" >op_read</list>
	<list instruction="1" ins_id="2" size="0" >op_write</list>
	<list instruction="1" ins_id="3" size="0" >op_add</list>
	<list instruction="1" ins_id="4" size="0" >op_sub</list>
	<list instruction="1" ins_id="5" size="0" >op_operate</list>
	<list instruction="1" ins_id="6" size="0" >op_make_var</list>
	<list instruction="1" ins_id="7" size="0" >op_read_table</list>
	<list instruction="1" ins_id="8" size="0" >op_comp</list>
	<list instruction="1" ins_id="9" size="0" >op_calc</list>
	<list instruction="1" ins_id="10" size="0" record_param_num="1">op_waitevent</list>
	<list instruction="1" ins_id="11" size="0" >op_getobj</list>
	<list instruction="1" ins_id="12" size="8" >op_set_error_continue</list>
	<list instruction="1" ins_id="13" size="0" record_param_num="1">op_call_func</list>	
	<list instruction="1" ins_id="14" size="8" >op_short_jump</list>	
	<list instruction="1" ins_id="16" size="8" >op_error_short_jump</list>
	<list instruction="1" ins_id="18" size="8" >op_success_short_jump</list>
	<!-- list instruction="1" ins_id="19" size="0" >op_save_register</list>	
	<list instruction="1" ins_id="18" size="0" >op_set_result</list -->
	<list instruction="1" ins_id="21" size="0" record_param_num="1">op_print</list>	
	<list instruction="1" ins_id="22" size="0" record_param_num="1">op_log</list>
	<list instruction="1" ins_id="23" size="8" >op_set_error</list>
	<list instruction="1" ins_id="24" size="0" >op_set_count_register</list>
	<list instruction="1" ins_id="26" size="0" record_param_num="1">op_sendevent</list>
	<list instruction="1" ins_id="27" size="4" >op_get_time</list>
	<list instruction="1" ins_id="19" size="4" >op_commit_affair</list>
	<list instruction="1" ins_id="29" size="4" >op_begin_affair</list>
	<list instruction="1" ins_id="30" size="4" >op_get_last_changed_time</list>
	<list instruction="1" ins_id="31" size="0" record_param_num="1">op_time_function</list>
	<list instruction="1" ins_id="33" size="8" >op_throw</list>
	<list instruction="1" ins_id="34" size="0" >op_install_event</list>
	<list instruction="1" ins_id="35" size="0" >op_remove_event</list>
	<list instruction="1" ins_id="36" size="0" record_param_num="1">op_add_timer</list>
	<list instruction="1" ins_id="37" size="0" >op_del_timer</list>
	<list instruction="1" ins_id="38" size="4" >op_check_valide</list>
	<list instruction="1" ins_id="39" size="8" >op_type_transfer</list>
	<list instruction="1" ins_id="40" size="0" record_param_num="1" >op_build_json_data</list>
	<list instruction="1" ins_id="41" size="0" record_param_num="1" >op_create_struct_type</list>
	<list instruction="1" ins_id="42" size="0"  >op_math_operate</list>
	<list instruction="1" ins_id="43" size="0" desc="赋值操作" >op_assignin</list>	
	<list instruction="1" ins_id="44" size="0"  >op_chdir</list>
	<list instruction="1" ins_id="45" size="0"  >op_rmfile</list>
	<list instruction="1" ins_id="46" size="0"  >op_mkdir</list>
	<list instruction="1" ins_id="47" size="0" >op_test</list>
	<list instruction="1" ins_id="48" size="8" >op_skip_error</list>
	<list instruction="1" ins_id="49" size="0" >op_get_arrsize</list>
	<list instruction="1" ins_id="50" size="8" >op_set_loop_index</list>
	<list instruction="1" ins_id="51" size="4" >op_get_last_error</list>
	<list instruction="1" ins_id="52" size="4" >op_get_simulate_stat</list>
	<list instruction="1" ins_id="54" size="4" >op_global_var</list>
	<list instruction="1" ins_id="57" size="4" >op_check_user_error</list>
	<list instruction="1" ins_id="58" size="4" >op_clear_error</list>
	<list instruction="1" ins_id="59" size="4" >op_idle</list>
	<list instruction="1" ins_id="60" size="0" >op_bit_operate</list>
	<list instruction="1" ins_id="61" size="4" >op_check_debug</list>
	<list instruction="1" ins_id="62" size="4" >op_check_host_debug</list>
	<list instruction="1" ins_id="63" size="4" >op_process_exit</list>
	<list instruction="1" ins_id="64" size="4" >op_process_abort</list>
	
	<!-- list instruction="1" ins_id="24" size="0" >op_set_loops</list>	
	<list instruction="1" ins_id="25" size="0" >op_count_jump</list -->	
	
	<list instruction="2" >op_bool_entry</list>
	<list instruction="2" >op_comp_entry</list>
	<list instruction="3" >op_loop_entry</list>
	
	<!-- param type description 'need_type_stream' will wirte 16 bit data type before data-value  -->
	<!-- datatype 0 int 1 float 2 string-->
	<list instruction="0" size="0" datatype="2" >aim</list>
	<list instruction="0" size="4" datatype="0" >index</list>
	<list instruction="0" size="4" datatype="1" >val</list>
	<list instruction="0" size="4" datatype="0" >num</list>
	<list instruction="0" size="0" datatype="2" >param</list>
	<list instruction="0" size="0" datatype="2" >tablename</list>
	<list instruction="0" size="0" datatype="2" >varname</list>
	<list instruction="0" size="0" datatype="2" need_type_stream="1">func_name</list>
	<list instruction="0" size="0" datatype="2" need_type_stream="1" >var</list>
	<list instruction="0" size="0" datatype="0" need_type_stream="1" >msgtype</list>
	<list instruction="0" size="0" datatype="0" need_type_stream="1" >varInt</list>
	<list instruction="0" size="0" datatype="1" need_type_stream="1" >varFloat</list>	
	<list instruction="0" size="0" datatype="0" need_refill_offset="1">jumpOffset</list>
	
	<list instruction="4" size="0">param_collect</list>
	<list instruction="3" size="0">formula_text</list>
	<list instruction="100" size="0">comment</list>
	<list instruction="100" size="0">apoEditorPos</list>
	<list instruction="100" size="0">breakPointInfo</list>	
	<list instruction="100" size="0">func_params</list>	
	<list instruction="5" size="0">user_define_function</list>
	<list instruction="6" size="0" datatype="2">function_info</list>	
	<list instruction="6" size="0" datatype="2">is_open_role_affair</list>
	<list instruction="7" size="0" >exception_catch_block</list>
	<list instruction="8" size="0" >function_init_block</list>
	<list instruction="9" >steps_block</list>
	<list instruction="9" >steps_block_valcomp</list>
	<list instruction="9" >op_compound_test</list>	
	<list instruction="9" >steps_switch_case</list>
	<list instruction="9" >steps_bt_selector</list>
	<list instruction="9" >steps_bt_sequence</list>
	<list instruction="9" >steps_compound_test</list>
	
	<!-- list instruction="9" >steps_compound_test_debug</list>
	<list instruction="9" >steps_compound_test_host_debug</list -->
		
	<list instruction="9" >function_filter</list>
	<list instruction="10" desc="if-else sub entry" >op_sub_comp_entry</list>	
	<list instruction="11" desc="function" >func_node</list>	
	<list instruction="11" desc="net message function" >msg_handler_node</list>	
	<list instruction="11" desc="event handler" >event_callback_node</list>	
	<list instruction="12" desc="reference from currentstep node " >ref_from</list>	
	
	<list instruction="13" desc="switch -default ,else" >default_collection</list>	
	<list instruction="13" desc="switch -default ,else" >last_run_collection</list>	
	<list instruction="14" desc="switch -default ,else" >steps_collection</list>
	
	<list instruction="15" desc="label for goto " >op_set_label</list>
	<list instruction="15" desc="inter label in exe-node" >internal_label</list>
	<list instruction="16" desc="goto cmd" >op_goto_label</list>
	<list instruction="16" desc="internal goto" >internal_goto</list>
</compile_setting>

<create_template kinds="hide" desc="新建节点的创建模板">
	<!-- create function -->
	<create_func name="func_node" index="2" auto_index="0" create_type="0">
		<attribute>
			<a1 name="name" value="new_function"/>
			<a4 name="create_template" value="create_list1"/>
			<a2 name="auto_index" value="0"/>
			<a5 name="expand_list" value="comment" />
			<a6 name="enable_drag" value="yes"/>
			<a7 name="var_name_index" value="1"/>
			<a8 name="name_auto_index" value="auto_index"/>
		</attribute>
		<sub_node>			
			<!-- n1 name="op_setflag">
				<attribute>
					<a1 name="name" value="节点_出错退出"/>
					<a2 name="expand" value="yes"/>
				</attribute>
				<sub_node>
					<n1 name="index" value="0">
						<attribute>
							<a1 name="name" value="错误退出"/>
							<a2 name="kinds" value="bool"/>
						</attribute>
					</n1>
				</sub_node>
			</n1 -->
			<n2 name="comment" value="函数说明"></n2>
		</sub_node>
	</create_func>
	<create_function_filter name="标签页" create_type="1">
		<function_filter name="类型_消息类"  create_template="create_msg_handler" auto_index="1" expand_stat="1" accept_drag_in="msg_handler_node">
		</function_filter>
	</create_function_filter>
	
	<create_event_function_filter name="标签页" create_type="1">
		<function_filter name="事件分类"  create_template="create_event_callback" auto_index="1" expand_stat="1" accept_drag_in="event_callback_node">
		</function_filter>
	</create_event_function_filter>
	
	<create_common_function_filter name="标签页" create_type="1">
		<function_filter name="函数分类"  create_template="create_func" auto_index="1" expand_stat="1" accept_drag_in="func_node">
		</function_filter>
	</create_common_function_filter>
	
	<create_msg_handler name="消息处理函数" create_type="1">		
		<msg_handler_node name="message_handler" enable_drag="yes" name_auto_index="auto_index" create_template="create_list2" auto_index="0" expand_list="comment" expand_stat="1" var_name_index="1"  preFillCmd="_localinit_msg_entry">
			<comment name="函数说明" expand="yes">
				<desc>消息处理handle(player, message)</desc>
				<isGlobal kinds="bool">1</isGlobal>
			</comment>
			<function_init_block name="block_加载时运行" create_template="create_list2" expand_list="condition" auto_index="0" >
				<comment name="功能说明" rw_stat="read">加载时运行</comment>
				<op_call_func name="节点_安装消息处理器" expand="yes" expand_list="comment">
					<comment name="功能说明" rw_stat="read">function_install_msg_handler(netModuleName, scriptName,maxId, minId,privilege)</comment>
					<func_name name="函数" kinds="hide" delete="no" >apollo_set_message_handler</func_name>
					<var name="网络接收器" kinds="string" delete="no">listener</var>
					<param_collect name="脚本名" kinds="hide" delete="no">
						<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">14</type>
						<var name="值" kinds="string" delete="no" restrict="type">none</var>
					</param_collect>			
					<msgtype name="主消息" kinds="numeral"  delete="no">0</msgtype>	
					<msgtype name="次消息" kinds="numeral"  delete="no">0</msgtype>					
					<varInt name="权限" kinds="enum" enum_value="none,connect,login,ingame,hight" delete="no">2</varInt>
				</op_call_func>
			</function_init_block>
			<exception_catch_block name="节点_异常处理" create_template="create_list1" expand_list="comment" auto_index="2" expand_stat="1">
				<comment name="功能说明" rw_stat="read">catch{}</comment>
				<op_get_last_error name="节点_获得错误号" expand="yes">
					<comment name="功能说明" rw_stat="read">get_last_error()</comment>
				</op_get_last_error>
				<op_call_func name="节点_调用: send_error()" create_template="create_input_param" auto_index="1" expand_list="comment,func_name" expand_stat="0">
					<comment name="功能说明" rw_stat="read">call_function(name,...)</comment>
					<func_name kinds="user_define" user_param="func_list">_realval=CPP.gscript_send_client_error&amp;_dispname=发送错误通知(errorId)</func_name>
					<param_collect name="参数_错误号" expand="yes" expand_stat="1">
						<type kinds="reference" reference_type="type_data_type" delete="no">11</type>
						<var kinds="string" delete="no" restrict="type">0</var>
					</param_collect>
				</op_call_func>
			</exception_catch_block>
			
			<op_skip_error name="节点_忽略错误_for_test"  kinds="hide" expand="yes">
				<comment name="功能说明" rw_stat="read">skip_error(value)</comment>
				<num name="错误类型" kinds="user_define" user_param="error_list" delete="no">_realval=47&amp;_dispname=system(NDERR_VARIANT_NOT_EXIST):variant not found</num>
			</op_skip_error>		
			<op_begin_affair name="节点_出错时自动恢复数据" expand="yes" expand_stat="1">
				<comment name="功能说明" rw_stat="read">player_affair_rollback()</comment>
			</op_begin_affair>
						
			<op_call_func name="节点_读DataType消息:$none$" expand="yes" expand_list="comment,var" expand_stat="1">
				<comment name="功能说明" rw_stat="read">function_read_fromat_message(data_type_name)</comment>
				<func_name name="函数" kinds="hide" delete="no">apollo_func_read_userData_from_msg</func_name>
				<param_collect name="参数1_输入消息对象" kinds="hide" delete="no">
					<type kinds="reference" reference_type="type_data_type" delete="no">10</type>
					<var kinds="string" delete="no" restrict="type">2</var>
				</param_collect>
				<var name="类型名字" kinds="string" replace_val="../.name" >none</var>
			</op_call_func>
				
			<op_make_var name="节点_申明变量:$none$" expand="yes" expand_stat="1">
				<comment name="功能说明" rw_stat="read">create_variant(name,value)</comment>
				<param name="名字" kinds="string" delete="no" replace_val="../.name" >none</param>
				<type kinds="reference" reference_type="type_data_type" delete="no">11</type>
				<var name="初始值" kinds="string" delete="no" restrict="type" >none</var>
			</op_make_var>
		
			<op_call_func name="节点_发送处理结果" create_template="create_input_param" auto_index="4" expand_list="comment,func_name" expand_stat="1">
				<comment name="功能说明" rw_stat="read">call_function(name,...)</comment>
				<func_name kinds="user_define" user_param="func_list">send_common_reply_msg</func_name>
				<param_collect name="参数_主消息号" expand="yes" delete="no">
					<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var kinds="string" delete="no" restrict="type">0</var>
				</param_collect>
				<param_collect name="参数_次消息号" expand="yes" delete="no">
					<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var kinds="string" delete="no" restrict="type">0</var>
				</param_collect>
				<param_collect name="参数_是否成功" expand="yes" delete="no">
					<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var kinds="string" delete="no" restrict="type">0</var>
				</param_collect>
				<param_collect name="参数_处理参数" expand="yes" delete="no">
					<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var kinds="string" delete="no" restrict="type">none</var>
				</param_collect>
			</op_call_func>
		</msg_handler_node>
	</create_msg_handler>
	
	<create_event_callback name="事件处理函数" create_type="1">		
		<event_callback_node name="event_callback" enable_drag="yes" create_template="create_list2" name_auto_index="auto_index" auto_index="0" expand_list="comment,event_id" expand_stat="1" var_name_index="1">
			<comment name="函数说明" expand="yes">
				<desc>事件处理函数</desc>
				<isGlobal kinds="bool">1</isGlobal>
			</comment>

			<function_init_block name="block_加载时运行" create_template="create_list2" expand_list="condition" auto_index="0" >
				<comment name="功能说明" rw_stat="read">加载时运行</comment>
				<op_call_func name="节点_安装事件处理器" expand="yes" expand_list="comment">
					<comment name="功能说明" rw_stat="read">function_install_event_handler(scriptName, eventiId)</comment>
					<func_name name="函数" kinds="hide" delete="no" >apollo_func_install_event_handler</func_name>
					<param_collect name="脚本名" kinds="hide" delete="no">
						<type name="参数1类型" kinds="reference" reference_type="type_data_type" delete="no">14</type>
						<var name="参数1值" kinds="string" delete="no" restrict="type">none</var>
					</param_collect>
					<varInt name="处理事件" kinds="user_define" user_param="event_list" delete="no">none</varInt>
				</op_call_func>
			</function_init_block>
			
			
			<exception_catch_block name="节点_异常处理" create_template="create_list1" expand_list="comment" auto_index="0"  >
				<comment name="功能说明" rw_stat="read">catch{}</comment>
			</exception_catch_block>
		
		</event_callback_node>
	</create_event_callback>
	
	<!-- create create func argment lists -->
	
	<!-- create step in function -->
	<create_step_exit name="退出节点" create_type="1" expand="yes" >
		<op_exit name="节点_退出($errno$)" expand="yes">			
			<comment name="功能说明" rw_stat="read">op_exit(value)</comment>
					
			<param_collect name="exitCode" expand="yes">
				<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var kinds="string" delete="no" restrict="type" replace_val="../../.name">0</var>			
			</param_collect>
		</op_exit>
	</create_step_exit>
	
	<create_step_process_exit name="进程退出" create_type="1" expand="yes" >
		<op_process_exit name="节点_process_exit" expand="yes" />	
	</create_step_process_exit>
	
	<create_step_process_abort name="进程abort" create_type="1" expand="yes" >
		<op_process_abort name="节点_process_abort" expand="yes" />	
	</create_step_process_abort>
	
	<op_idle name="idle"> </op_idle>
	<breakPointInfo kinds="hide">yes</breakPointInfo>
	
	<create_get_last_error name="获得错误id" create_type="1" expand="yes" >
		<op_get_last_error name="节点_获得错误号" expand="yes" create_label="create_internal_label">			
			<comment name="功能说明" rw_stat="read">get_last_error()</comment>
		</op_get_last_error>
	</create_get_last_error>
	
	<create_step_set_error name="设置错误" create_type="1" expand="yes" >
		<op_set_error name="节点_设置错误" expand="yes">			
			<comment name="功能说明" rw_stat="read">set_error(id)</comment>			
			<param_collect name="错误号" expand="yes">
				<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var kinds="string" delete="no" restrict="type">0</var>			
			</param_collect>
		</op_set_error>
	</create_step_set_error>	
	
	
	<create_set_loop_index name="设置循环索引" create_type="1" expand="yes" >
		<op_set_loop_index name="节点_设置循环索引" expand="yes">			
			<comment name="功能说明" rw_stat="read">set-count-index</comment>
			<param_collect name="value" expand="yes">
				<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var kinds="string" delete="no" restrict="type">0</var>			
			</param_collect>
		</op_set_loop_index>
	</create_set_loop_index>
	
	<create_error_continue name="出错继续执行" create_type="1" expand="yes" >
		<op_set_error_continue name="节点_出错继续" expand="yes">			
			<comment name="功能说明" rw_stat="read">on_error_continue</comment>
			<index name="设置出错后继续执行" kinds="enum" enum_value="false,true" delete="no">1</index>
		</op_set_error_continue>
	</create_error_continue>
		
	
	<create_skip_error name="节点忽略错误" create_type="1" expand="yes" >
		<op_skip_error name="节点_忽略错误($errno$)" expand="yes">			
			<comment name="功能说明" rw_stat="read">skip_error(value)</comment>
			<num name="错误类型" kinds="user_define" user_param="error_list" delete="no" replace_val="../.name">0</num>
		</op_skip_error>
	</create_skip_error>
	
	
	<create_step_throw name="抛出异常" create_type="1" expand="yes" >
		<op_throw name="节点_抛出异常($except$)" expand="yes">			
			<comment name="功能说明" rw_stat="read">op_throw(value)</comment>		
			<param_collect name="exceptId" expand="yes">
				<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var kinds="string" delete="no" restrict="type">0</var>			
			</param_collect>
		</op_throw>
	</create_step_throw>
	
	<create_step_error_catch_block name="捕获异常" create_type="1" >
		<exception_catch_block name="节点_异常处理" create_template="create_list1" expand_list="comment" auto_index="0"  >
			<comment name="功能说明" rw_stat="read">catch{}</comment>
		</exception_catch_block>
	</create_step_error_catch_block>
	
	
	<create_step_get_time name="得到时间" create_type="1" expand="yes" >
		<op_get_time name="节点_时间" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">get_time()</comment>
		</op_get_time>
	</create_step_get_time>
	
	
	<create_step_changed_time name="得到脚本修改时间" create_type="1" expand="yes" >
		<op_get_last_changed_time name="节点_脚本修改时间" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">get_compile_time()</comment>
		</op_get_last_changed_time>
	</create_step_changed_time>
	
	
	<create_step_time_function name="时间函数" create_type="1" expand="yes">
		<op_time_function name="节点_时间计算"  expand="yes"  create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">time_fun(cmd, unit, time1, val)</comment>			
			<varInt name="操作类型"  kinds="reference" reference_type="type_time_cmd" delete="no">0</varInt>
			<varInt name="时间单位"  kinds="reference" reference_type="type_time_unit" delete="no">0</varInt>			
			<param_collect name="参数1" expand="yes" expand_stat="1" delete="no">
				<type name="参数1类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="参数1值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<param_collect name="参数2" expand="yes" expand_stat="1" delete="no">
				<type name="参数2类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="参数2值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
		</op_time_function>
	</create_step_time_function>
	
	
	<create_step_begin_affair name="开始角色数据事务" create_type="1" expand="yes" >
		<op_begin_affair name="节点_出错时自动恢复数据" expand="yes">
			<comment name="功能说明" rw_stat="read">player_affair_rollback()</comment>
		</op_begin_affair>
	</create_step_begin_affair>
	
	<create_step_commit_affair name="提交角色事务" create_type="1" expand="yes" >
		<op_commit_affair name="节点_提交数据改动" expand="yes">
			<comment name="功能说明" rw_stat="read">player_commit_rollback()</comment>
		</op_commit_affair>
	</create_step_commit_affair>
	
	<create_step_read name="对象读取节点" create_type="1" expand="yes" >
		<op_read name="节点_读取_$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_read(aim,index_name)</comment>
			<aim kinds="user_define" user_param="internal_object" replace_val="../.name">none</aim>
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			
		</op_read>
	</create_step_read>
	<create_step_write name="对象写入节点" create_type="1" desc="创建写入节点">
		<op_write name="节点_写入_$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_write(aim,index_name,value)</comment>
			<aim kinds="user_define" user_param="internal_object" delete="no" replace_val="../.name">none</aim>			
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<type name="结果类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="结果" kinds="string" delete="no" restrict="type">none</var>
		</op_write>
	</create_step_write>
	<create_step_add name="对象增加节点" create_type="1" >
		<op_add name="节点_增加_$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_add(aim,index_name,value)</comment>
			<aim kinds="user_define" user_param="internal_object" delete="no" replace_val="../.name">none</aim>	
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<type name="结果类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="结果" kinds="string" delete="no" restrict="type">none</var>
		</op_add>
	</create_step_add>
			
	<create_step_sub name="对象减少节点" create_type="1" >
		<op_sub name="节点_减少_$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_sub(aim,index_name,value)</comment>
			<aim kinds="user_define" user_param="internal_object" delete="no" replace_val="../.name">none</aim>	
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<type name="结果类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="结果" kinds="string" delete="no" restrict="type">none</var>
		</op_sub>
	</create_step_sub>
	
	<create_step_test name="对象检测节点" create_type="1" >
		<op_test name="节点_检测_$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_test(aim,index_name,value)</comment>
			<aim kinds="user_define" user_param="internal_object" delete="no" replace_val="../.name">none</aim>	
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<type name="结果类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="结果" kinds="string" delete="no" restrict="type">none</var>
		</op_test>
	</create_step_test>
	
	<create_step_operate name="对象命令" create_type="1" >
		<op_operate name="节点_$name$操作" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">op_cmd(aim,cmd, index,value)</comment>
			<aim kinds="user_define" user_param="internal_object" replace_val="../.name" delete="no">none</aim>
			<param name="命令" kinds="string" delete="no">none</param>
			<param_collect name="索引" expand="yes" expand_stat="1" delete="no">
				<type name="命令索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<type name="命令值类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="命令值" kinds="string" delete="no" restrict="type">none</var>
		</op_operate>
	</create_step_operate>
	
	
	
	<create_step_getother_object name="获得其他对象" create_type="1" >
		<op_getobj name="节点_获得其他对象:$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">get_other_object(object_name)</comment>
			<param name="对象名" kinds="user_define" user_param="external_objects"  delete="no" replace_val="../.name">none</param>
		</op_getobj>
	</create_step_getother_object>
	
	<create_make_var name="申明变量" create_type="1" auto_index_list="param" auto_index_reference="variant_index">
		<op_make_var name="节点_申明变量$var$" expand="yes" var_name="param">
			<comment name="功能说明" rw_stat="read">create_variant(name,value)</comment>
			<param name="名字" kinds="string" delete="no" auto_index_ref="var_name_index" replace_val="../.name">var</param>
			<type kinds="reference" reference_type="type_data_type" delete="no">18</type>
			<var name="初始值" kinds="string" delete="no" restrict="type">0</var>
		</op_make_var>
	</create_make_var>
	
	<create_global_var name="全局变量" create_type="1" auto_index_list="param" auto_index_reference="variant_index">
		<op_global_var name="节点_全局变量$var$" expand="yes" var_name="param" connect_in_seq="yes">
			<comment name="功能说明" rw_stat="read">global_variant(name,value)</comment>
			<param name="名字" kinds="string" delete="no" auto_index_ref="var_name_index" replace_val="../.name">g_var</param>
			<type kinds="reference" reference_type="type_data_type" delete="no">18</type>
			<var name="初始值" kinds="string" delete="no" restrict="type">0</var>
		</op_global_var>
	</create_global_var>
	
	
	<create_build_json_data name="创建json数据" create_type="1" >
		<op_build_json_data name="节点_创建json数据$name$" create_template="create_user_def_param" var_name="var" auto_index="0" expand_list="comment,msgtype,varInt,varFloat,var,func_name">
			<comment name="功能说明" rw_stat="read">define  name{...}</comment>			
			<var name="变量名" kinds="string" delete="no" replace_val="../.name">none</var>
		</op_build_json_data>
	</create_build_json_data>
	
	<create_build_struct_type name="创建新类型" create_type="1" >
		<op_create_struct_type name="节点_创建新类型$name$" create_template="create_user_def_param" var_name="var" auto_index="0" expand_list="comment,msgtype,varInt,varFloat,var,func_name">
			<comment name="功能说明" rw_stat="read">typedef struct{...}</comment>			
			<var name="类型名" kinds="string" delete="no" replace_val="../.name">none</var>
		</op_create_struct_type>
	</create_build_struct_type>
	
	<create_get_arrsize name="获得数组长度" create_type="1" >
		<op_get_arrsize name="节点_数组$name$长度" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">size()</comment>
			<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="数组名" kinds="string" delete="no" restrict="type" replace_val="../.name">none</var>
		</op_get_arrsize>
	</create_get_arrsize>
		
	<create_assignin name="赋值操作" create_type="1" >
		<op_assignin name="节点_赋值" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">var = value</comment>
			<varname name="变量名" kinds="string" delete="no" restrict="type">none</varname>
			<param_collect name="值" expand="yes" expand_stat="1" delete="no">
				<type name="赋值类型" kinds="reference" reference_type="type_data_type" delete="no">18</type>
				<var name="值" kinds="string" delete="no" restrict="type">0</var>
			</param_collect>
		</op_assignin>
	</create_assignin>
	
	<create_step_type_transfer name="类型转换" create_type="1" >
		<op_type_transfer name="节点_类型转换" expand_list="comment,varname,index" create_label="create_internal_label" >			
			<comment name="功能说明" rw_stat="read">data.convert( type )</comment>			
			<varname name="变量名" kinds="string" delete="no" restrict="type">none</varname>
			<index name="新类型" kinds="reference" reference_type="type_data_type" delete="no">0</index>
		</op_type_transfer>
	</create_step_type_transfer>
	
	
	<create_math_operate name="数学运算符" create_type="1" >
		<op_math_operate name="节点_数学运算" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">+-*/</comment>
			<index kinds="enum" enum_value="+,-,*,/,sqrt,max,min,rand"  delete="no">0</index>	
			<param_collect name="操作数1" expand="yes" expand_stat="1" delete="no">
				<type name="操作数1类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数1值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			<param_collect name="操作数2" expand="yes" expand_stat="1" delete="no">
				<type name="操作数2类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数2值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
		</op_math_operate>
	</create_math_operate>
	
	<create_bit_operate name="位运算符" create_type="1" >
		<op_bit_operate name="节点_位运算" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">and_or_not</comment>
			<index kinds="enum" enum_value="AND,OR,XOR,NOT,LEFT_MOVE,RIGHT_MOVE"  delete="no">0</index>	
			<param_collect name="操作数1" expand="yes" expand_stat="1" delete="no">
				<type name="操作数1类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数1值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			<param_collect name="操作数2" expand="yes" expand_stat="1" delete="no">
				<type name="操作数2类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数2值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
		</op_bit_operate>
	</create_bit_operate>
	
	
	
	
	<create_read_table name="读取excel节点" create_type="1" >
		<op_read_table name="节点_读取表格" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">function:read_excel_table(table,col, index)</comment>
			<tablename name="excel表" kinds="user_define" user_param="dbl_excel" delete="no">none</tablename>
			<param name="列名" kinds="user_define" user_param="dbl_excel_col" delete="no">none</param>						
			<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
		</op_read_table>
	</create_read_table>
	
	<create_step_calc name="公式" create_type="1" >
		<op_calc name="节点_公式" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">run_formulate(formulate_text)</comment>
			<param name="公式内容" kinds="string"  delete="no">0</param>
		</op_calc>
	</create_step_calc>
	
	
	<create_step_chdir name="更改目录" create_type="1" >
		<op_chdir name="节点_更改目录" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">cd(path_name)</comment>
			<param name="目录名" kinds="string"  delete="no">0</param>
		</op_chdir>
	</create_step_chdir>
	
	<create_step_rmfile name="删除文件" create_type="1" >
		<op_rmfile name="节点_删除文件" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">rm(filename)</comment>
			<param name="文件名" kinds="string" delete="no">0</param>
		</op_rmfile>
	</create_step_rmfile>
	
	<create_step_mkdir name="创建目录" create_type="1" >
		<op_mkdir name="节点_公式" expand="yes" create_label="create_internal_label">
			<comment name="创建目录" rw_stat="read">mkdir(newpath)</comment>
			<param name="新目录" kinds="string" delete="no">0</param>
		</op_mkdir>
	</create_step_mkdir>
	
	
	<create_step_waitevent name="等待事件节点" create_type="1" >
		<op_waitevent name="节点_等待事件($id$)" create_template="create_input_param" auto_index="0"  create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">wait_event(event_id)</comment>
			<varInt name="事件名" kinds="user_define" user_param="event_list" delete="no" replace_val="../.name">none</varInt>
		</op_waitevent>
	</create_step_waitevent>
	
	<create_step_send_event name="产生事件节点" create_type="1" >
		<op_sendevent name="节点_产生事件($id$)" create_template="create_input_param" auto_index="0"  create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">generate_event(event_id)</comment>
			<varInt name="事件名" kinds="user_define" user_param="event_list" delete="no" replace_val="../.name">none</varInt>
		</op_sendevent>
	</create_step_send_event>
	
	<create_install_event name="安装本地事件处理器" create_type="1" >
		<op_install_event name="节点_安装本地事件处理器" expand="yes">
			<comment name="功能说明" rw_stat="read">install_local_event_handler(event_id, function)</comment>			
			<index name="事件名" kinds="user_define" user_param="event_list" delete="no">none</index>			
			<param name="函数名" kinds="user_define" user_param="func_list" delete="no">none</param>			
		</op_install_event>
	</create_install_event>
	<create_remove_event name="移除本地事件处理器" create_type="1" >
		<op_remove_event name="节点_移除本地事件处理器" expand="yes">
			<comment name="功能说明" rw_stat="read">install_local_event_handler(event_id, function)</comment>			
			<index name="事件名" kinds="user_define" user_param="event_list" delete="no">none</index>			
			<param name="函数名" kinds="user_define" user_param="func_list" delete="no">none</param>			
		</op_remove_event>
	</create_remove_event>
	
	
	<create_add_timer name="添加定时器" create_type="1" >
		<op_add_timer name="节点_添加定时器$name$" create_template="create_input_param" auto_index="0" expand_list="comment,msgtype,varInt,varFloat,var,func_name" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">add_timer(type, tm_interval,name, function,...)</comment>			
			<msgtype name="类型" kinds="enum" enum_value="局部,全局" delete="no">0</msgtype>					
			<varInt name="类型" kinds="enum" enum_value="循环,一次性" delete="no">0</varInt>			
			<varFloat name="间隔(s)" kinds="numeral" delete="no">1.0</varFloat>
			<param_collect name="定时器名称" delete="no">
				<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">2</type>
				<var name="值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			<func_name name="函数名" kinds="user_define" user_param="func_list" delete="no">none</func_name>
		</op_add_timer>
	</create_add_timer>
	
	<create_del_timer name="删除定时器" create_type="1" >
		<op_del_timer name="节点_删除定时器$name$" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">del_timer(name)</comment>	
			<index name="类型" kinds="enum" enum_value="局部,全局" delete="no">0</index>			
			<param_collect name="定时器名称" delete="no">
				<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">2</type>
				<var name="值" kinds="string" delete="no" restrict="type" replace_val="../../.name">none</var>
			</param_collect>
		</op_del_timer>
	</create_del_timer>
	
	
	<create_step_call_func name="调用函数" create_type="1" >
		<op_call_func name="节点_调用$Function$" create_template="create_input_param"  auto_index="0" expand_list="comment,func_name" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">call_function(name,...)</comment>
			<func_name kinds="user_define" user_param="func_list" delete="no" replace_val="../.name" >none</func_name>
		</op_call_func>
	</create_step_call_func>
	
	<create_step_print name="打印输出" create_type="1" >
		<op_print name="节点_打印" create_template="create_input_param" auto_index="0"  expand_list="comment" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">print(...)</comment>
		</op_print>
	</create_step_print>
	
	<create_step_log name="写日志" create_type="1" >
		<op_log name="节点_日志" create_template="create_input_param" auto_index="0" expand_list="comment"  create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">log(...)</comment>
		</op_log>
	</create_step_log>
		
	<create_step_inistall_msg_handler name="安裝网络消息处理器" create_type="1">
		<op_call_func name="节点_安装消息处理器" expand="yes" expand_list="comment" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">function_install_msg_handler(netModuleName, scriptName,maxId, minId,privilege)</comment>
			<func_name name="函数" kinds="hide" delete="no" >apollo_set_message_handler</func_name>
			<var name="网络接收器" kinds="string" delete="no">listener</var>
			<param_collect name="脚本名" kinds="hide" delete="no">
				<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">14</type>
				<var name="值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>			
			<msgtype name="主消息" kinds="numeral"  delete="no">0</msgtype>	
			<msgtype name="次消息" kinds="numeral"  delete="no">0</msgtype>					
			<varInt name="权限" kinds="enum" enum_value="none,connect,login,ingame,hight" delete="no">2</varInt>
		</op_call_func>
	</create_step_inistall_msg_handler>
	
	<create_step_send_msg name="发送消息" create_type="1">
		<op_call_func name="节点_发送消息" create_template="create_input_param" auto_index="0" expand_list="comment,varInt,msgtype" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">function_send_message(msg_id,...)</comment>
			<func_name name="函数" kinds="hide" delete="no" >apollo_func_send_msg</func_name>
			<varInt name="主消息号" kinds="numeral" delete="no" >0</varInt>
			<msgtype name="次消息号" kinds="numeral" delete="no" >0</msgtype>
		</op_call_func>		
	</create_step_send_msg>
	
	<!-- 通过调用外部函数的方法来读取消息 参数1 -->
	<create_step_read_msg name="读取网络消息" create_type="1">
		<op_call_func name="节点_读取网络数据" expand="yes" expand_list="comment" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">function_read_message(read_type)</comment>
			<func_name name="函数" kinds="hide" delete="no" >apollo_func_read_msg</func_name>
			<param_collect name="参数1_输入消息对象"  kinds="hide" delete="no">
				<type kinds="reference" reference_type="type_data_type" delete="no">10</type>
				<var kinds="string" delete="no" restrict="type">2</var>			
			</param_collect>
			<param_collect name="读取参数" expand="yes" expand_stat="1" delete="no">
				<type kinds="hide" delete="no">0</type>
				<msgtype name="读取数据类型" kinds="reference" reference_type="type_data_type" delete="no" restrict="type">0</msgtype>
			</param_collect>		
		</op_call_func>		
	</create_step_read_msg>
	
	<create_step_read_datatype_msg name="从dataType定义读取消息" create_type="1">
		<op_call_func name="节点_读DataType消息$name$" expand="yes" expand_list="comment,var">
			<comment name="功能说明" rw_stat="read">function_read_fromat_message(data_type_name)</comment>
			<func_name name="函数" kinds="hide" delete="no" >apollo_func_read_userData_from_msg</func_name>
			<param_collect name="参数1_输入消息对象"  kinds="hide" delete="no">
				<type kinds="reference" reference_type="type_data_type" delete="no">10</type>
				<var kinds="string" delete="no" restrict="type">2</var>			
			</param_collect>			
			<var name="类型名字" kinds="string"  delete="no" replace_val="../.name">none</var>			
		</op_call_func>		
	</create_step_read_datatype_msg>
	
	
	<create_step_get_user_data_type name="从dataType获得类型" create_type="1">
		<op_call_func name="节点_获得DataType类型:$name$" expand="yes" expand_list="comment,var">
			<comment name="功能说明" rw_stat="read">function_get_userDataType(data_type_name)</comment>
			<func_name name="函数" kinds="hide" delete="no" >apollo_func_get_userDataType</func_name>					
			<var name="类型名字" kinds="string"  delete="no" replace_val="../.name">none</var>			
		</op_call_func>		
	</create_step_get_user_data_type>
	
	
	<create_step_read_excel_attr name="从excel读取属性节点" create_type="1" >
		<op_call_func name="节点_从excel读取属性" expand="yes" expand_list="comment" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read">function_collect_attrs_from_excel(table,index)</comment>
			<func_name name="函数" kinds="hide" delete="no" >gscript_table_attrs_read</func_name>
			<var name="excel表" kinds="user_define" user_param="dbl_excel" delete="no">none</var>				
			<type name="索引类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
			<var name="索引值" kinds="string" delete="no" restrict="type">none</var>
		</op_call_func>
	</create_step_read_excel_attr>
	
	
	<create_op_comp name="比较运算" create_type="1" >
		<op_comp name="节点_比较运算" expand="yes" create_label="create_internal_label">
			<comment name="功能说明" rw_stat="read"> 大于小于等级 </comment>
			<index  name="逻辑运算" kinds="reference" reference_type="type_compare_sub_entry" delete="no">0</index>	
			<param_collect name="操作数1" expand="yes" expand_stat="1" delete="no">
				<type name="操作数1类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数1值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			<param_collect name="操作数2" expand="yes" expand_stat="1" delete="no">
				<type name="操作数2类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="操作数2值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
		</op_comp>
	</create_op_comp>
	
	
	<create_get_simulate_stat name="检测模拟状态" create_type="1" expand="yes" >
		<op_get_simulate_stat name="节点_检测模拟状态" expand="yes" create_label="create_internal_label">			
			<comment name="功能说明" rw_stat="read">check_simulate_stat()</comment>
		</op_get_simulate_stat>
	</create_get_simulate_stat>
	
	<create_step_check_valid name="测试返回值" create_type="1" >
		<op_check_valide name="节点_测试返回值" expand_list="comment"  create_label="create_internal_label">			
			<comment name="功能说明" rw_stat="read">check_last_return_value_is_valid()</comment>
		</op_check_valide>
	</create_step_check_valid>
		
	<!------------ compount test ------------>	
	
	<create_test_current_value name="检测返回值" create_type="1" >
		<steps_compound_test name="节点_测试上一步结果"  breakPointAnchor="op_check_valide"  blueprintCtrl="CompoundTest"  blueprintTips="if($curValue)">
			<op_check_valide kinds="hide" delete="no" create_label="create_internal_label" />
			<op_bool_entry name="节点_bool上一步结果分支" expand_list="comment">
				<op_sub_comp_entry name="LastValue true" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>
				</op_sub_comp_entry>
				<op_sub_comp_entry name="LastValue false" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide"  delete="no">0</condition>
				</op_sub_comp_entry>
			</op_bool_entry>			
		</steps_compound_test>
	</create_test_current_value>
	
	<create_test_machine_is_simulate name="检测虚拟机状态" create_type="1" >
		<steps_compound_test name="节点_检测虚拟机状态"  breakPointAnchor="op_get_simulate_stat"  blueprintCtrl="CompoundTest"  blueprintTips="if(Simulate==true)">
			<op_get_simulate_stat kinds="hide" delete="no"  create_label="create_internal_label" />
			<op_bool_entry name="节点_bool上一步结果分支" expand_list="comment">
				<op_sub_comp_entry name="VM is Simulate" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>
				</op_sub_comp_entry>
				<op_sub_comp_entry name="VM is Unsimulate" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide"  delete="no">0</condition>
				</op_sub_comp_entry>
			</op_bool_entry>			
		</steps_compound_test>
	</create_test_machine_is_simulate>
	
	<create_test_script_debug name="测试脚本debug" create_type="1" >
		<steps_compound_test name="节点_测试脚本debug"  breakPointAnchor="op_check_debug" blueprintCtrl="CompoundTest" blueprintTips="if(Script-debug)">
			<op_check_debug kinds="hide" delete="no"  create_label="create_internal_label" />
			<op_bool_entry name="节点_bool上一步结果分支" expand_list="comment">
				<op_sub_comp_entry name="This script Debug" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>
				</op_sub_comp_entry>
				<op_sub_comp_entry name="This script Release" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide"  delete="no">0</condition>
				</op_sub_comp_entry>
			</op_bool_entry>			
		</steps_compound_test>
	</create_test_script_debug>
	
	<create_test_host_debug name="测试进程debug" create_type="1" >
		<steps_compound_test name="节点_测试进程debug"  breakPointAnchor="op_check_host_debug"  blueprintCtrl="CompoundTest"  blueprintTips="if(Progress-debug)">
			<op_check_host_debug kinds="hide" delete="no" />
			<op_bool_entry name="节点_bool上一步结果分支" expand_list="comment">
				<op_sub_comp_entry name="Program is Debug" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>
				</op_sub_comp_entry>
				<op_sub_comp_entry name="Program is Release" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide"  delete="no">0</condition>
				</op_sub_comp_entry>
			</op_bool_entry>			
		</steps_compound_test>
	</create_test_host_debug>
	<!------------ login control entry ------------>
	<create_step_bool_entry name="bool分支" create_type="1" >
		<op_bool_entry name="节点_bool上一步结果分支" expand_list="comment" >			
			<comment name="功能说明" rw_stat="read">if{}</comment>
			<op_sub_comp_entry name="上一步true时执行" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
				<condition no_comp="1" kinds="hide" delete="no">1</condition>
			</op_sub_comp_entry>
			<op_sub_comp_entry name="上一步false时执行" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
				<condition no_comp="1" kinds="hide"  delete="no">0</condition>
			</op_sub_comp_entry>
		</op_bool_entry>
	</create_step_bool_entry>
	
	
	<create_comp_steps name="数值比较分支" create_type="1" >
		<steps_block_valcomp name="节点_数值比较分支" auto_index="0" breakPointAnchor="op_comp">	
			<op_comp name="比较运行" expand="yes"  delete="no">
				<comment name="功能说明" rw_stat="read"> 大于小于等级 </comment>
				<index  name="逻辑运算" kinds="reference" reference_type="type_compare_sub_entry" delete="no">0</index>	
				<param_collect name="操作数1" expand="yes" expand_stat="1" delete="no">
					<type name="操作数1类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var name="操作数1值" kinds="string" delete="no" restrict="type">none</var>
				</param_collect>
				<param_collect name="操作数2" expand="yes" expand_stat="1" delete="no">
					<type name="操作数2类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var name="操作数2值" kinds="string" delete="no" restrict="type">none</var>
				</param_collect>
			</op_comp>
			<op_bool_entry name="比较成功就执行"  delete="no" >
				<op_sub_comp_entry name="执行块" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>
				</op_sub_comp_entry>
			</op_bool_entry>
		</steps_block_valcomp>
	</create_comp_steps>
	
	<create_switch_case name="switc-case" create_type="1" >
		<steps_switch_case name="switch-case" auto_index="0"  create_template="create_case_entry"  breakPointAnchor="op_idle">
			<comment name="功能说明" rw_stat="read">switch()-case </comment>
			<op_idle name="idle"  kinds="hide" onlyForDebug="yes"></op_idle>			
			<param_collect name="输入条件" expand="yes" expand_stat="1" delete="no" referenced_only="yes">
				<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var name="数值" kinds="string" delete="no" restrict="type">none</var>
			</param_collect>
			<default_collection name="default" create_template="create_list1" delete="no" auto_index="0">
			</default_collection>
		</steps_switch_case>
	</create_switch_case>
		
	<create_case_entry name="case-entry" create_type="1">
		<steps_collection name="case">
			<op_comp name="case-test" expand="yes" delete="no">			
				<index  name="逻辑运算" kinds="hide" delete="no">1</index>
				<ref_from name="输入条件_引用param_collect" kinds="hide" delete="no" >../../../param_collect</ref_from>
				<param_collect name="匹配条件" expand="yes" expand_stat="1" delete="no">
					<type name="类型" kinds="reference" reference_type="type_data_type" delete="no">0</type>
					<var name="数值" kinds="string" delete="no" restrict="type">none</var>
				</param_collect>
			</op_comp>	
			<op_bool_entry name="case比较成功执行" delete="no" >
				<op_sub_comp_entry name="执行块" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
					<condition no_comp="1" kinds="hide" delete="no">1</condition>					
					<last_run_collection name="break-case-or-loop" kinds="hide" delete="no" >
						<op_short_jump name="break" kinds="hide" >
							<jumpOffset >0</jumpOffset>
						</op_short_jump>
					</last_run_collection>
				</op_sub_comp_entry>
			</op_bool_entry>
		</steps_collection>		
	</create_case_entry>
			
	<create_step_loop_entry name="循环分支" create_type="1" >
		<op_loop_entry name="节点_循环" expand_list="param_collect" auto_index="1" comp_cond="param_collect" expand_stat="0" breakPointAnchor="op_idle">
			<comment name="功能说明" rw_stat="read">loop{}</comment>
			<param_collect name="次数" expand="yes" expand_stat="1" delete="no" referenced_only="yes">
				<type kinds="reference" reference_type="type_data_type" delete="no">0</type>
				<var kinds="string" delete="no" restrict="type">none</var>	
			</param_collect>						
			<op_idle name="idle"  kinds="hide" onlyForDebug="yes"></op_idle>			
			<steps_collection name="loop body" create_template="create_list1" delete="no">
			</steps_collection>
			
		</op_loop_entry>		
	</create_step_loop_entry>
		
	<create_break name="break" create_type="1" >
		<op_short_jump name="节点_break" expand_list="comment"  >
			<comment name="功能说明" rw_stat="read">break</comment>
			<jumpOffset kinds="hide" delete="no" >0</jumpOffset>
		</op_short_jump>
	</create_break>
	
	<op_error_break name="break-on-error" create_type="1" >
		<op_error_short_jump name="节点_break_on_error" expand_list="comment"  >
			<comment name="功能说明" rw_stat="read">break when last false</comment>
			<jumpOffset kinds="hide" delete="no" >0</jumpOffset>
		</op_error_short_jump>
	</op_error_break>
	
	<create_success_break name="break-on-success" create_type="1" >
		<op_success_short_jump name="节点_break_on_success" expand_list="comment"  >
			<comment name="功能说明" rw_stat="read">break when last success</comment>
			<jumpOffset kinds="hide" delete="no" >0</jumpOffset>
		</op_success_short_jump>
	</create_success_break>
	
	
	<create_step_true_exit name="true时返回" create_type="1" >
		<op_bool_entry name="节点_return_on_success" expand_list="comment" >			
			<comment name="功能说明" rw_stat="read">if($curval==true)exit</comment>
			<op_sub_comp_entry name="上一步true时执行" kinds="hide" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
				<condition no_comp="1" kinds="hide" delete="no">1</condition>
				<op_exit name="节点_退出" expand="yes">			
					<comment name="功能说明" rw_stat="read">op_exit(value)</comment>
					<num name="返回结果" kinds="user_define" user_param="error_list" delete="no">0</num>
				</op_exit>
			</op_sub_comp_entry>
		</op_bool_entry>
	</create_step_true_exit>
	
	<create_step_false_eixt name="false时返回" create_type="1" >
		<op_bool_entry name="节点_return_on_error" expand_list="comment" >			
			<comment name="功能说明" rw_stat="read">if($curval==false)exit</comment>
			<op_sub_comp_entry name="上一步false时执行" kinds="hide" create_template="create_list1" delete="no" comp_cond="condition" auto_index="0">
				<condition no_comp="1" kinds="hide"  delete="no">0</condition>				
				<op_exit name="节点_退出" expand="yes">			
					<comment name="功能说明" rw_stat="read">op_exit(value)</comment>
					<num name="返回结果" kinds="user_define" user_param="error_list" delete="no">0</num>
				</op_exit>
			</op_sub_comp_entry>
		</op_bool_entry>
	</create_step_false_eixt>
	
	
	<create_label name="创建标签" create_type="1" >
		<op_set_label name="标签$name$" expand="yes" comp_cond="param">
			<param name="标签名" kinds="string" delete="no" auto_index_ref="auto_index" replace_val="../.name" >Lab</param>	
		</op_set_label>
	</create_label>
	
	<create_goto_label name="goto标签" create_type="1" >
		<op_goto_label name="goto:$name$" expand="yes" comp_cond="param">
			<param name="跳转至" kinds="string" delete="no" auto_index_ref="auto_index" replace_val="../.name">Lab1</param>	
		</op_goto_label>		
	</create_goto_label>
	
	
	<create_internal_label name="在node中创建标签" create_type="1" >
		<internal_label name="标签$name$"  kinds="string" replace_val=".name" auto_index_ref="auto_index" >Lab</internal_label>
	</create_internal_label>
	
	
	<create_internal_goto name="goto标签" create_type="1" >
		<internal_goto name="goto:$name$" kinds="string" replace_val=".name">Lab1</internal_goto>		
	</create_internal_goto>
	
	
	<!--- selector -->		
	<create_bt_selector name="BT-Selector" create_type="1" >
		<steps_bt_selector name="BT-Selector" auto_index="0" breakPointAnchor="op_idle" >
			<comment name="功能说明" rw_stat="read">run until one of children success </comment>
			<op_idle name="idle"  kinds="hide" onlyForDebug="yes"></op_idle>
			
			<op_set_label name="标签$name$" kinds="hide" expand="yes" comp_cond="param"  delete="no">
				<param name="标签名" kinds="string" delete="no" auto_index_ref="auto_index" replace_val="../.name" >_SelectAutoLab</param>	
			</op_set_label>
			<steps_collection name="selector-children"  create_template="create_bt_selector_child" delete="no">
			</steps_collection>
		
			<op_waitevent name="节点_等待0" kinds="hide"  delete="no">
				<varInt name="事件名" >0</varInt>
			</op_waitevent>
			<op_goto_label name="goto:$name$" expand="yes" comp_cond="ref_from" kinds="hide"  delete="no">
				<ref_from name="跳转地址" >../../op_set_label/param</ref_from>
			</op_goto_label>			
		</steps_bt_selector>
	</create_bt_selector>
		
	<create_bt_selector_child name="select-entry" create_type="1">
		<steps_collection name="selector-node">
			<param name="subSelect" kinds="string" delete="no" auto_index_ref="auto_index"  referenced_only="yes" >Node</param>				
			<op_clear_error name="清除错误,非系统错误" ></op_clear_error>
			<steps_collection name="selector-node-entry" create_template="create_list1" delete="no">
			</steps_collection>	
			<op_check_user_error name="检测是否出错"  kinds="hide" delete="no"></op_check_user_error>	
			<op_error_short_jump name="节点_break"  kinds="hide" expand_list="comment"  >
				<comment name="功能说明" rw_stat="read">break when last error</comment>
				<jumpOffset kinds="hide" delete="no" >0</jumpOffset>
			</op_error_short_jump>			
		</steps_collection>
	</create_bt_selector_child>	
	<!--- -->
		
	<create_bt_sequence name="BT-Sequence" create_type="1" >
		<steps_bt_sequence name="BT-Sequence" auto_index="0" breakPointAnchor="op_idle" >
			<comment name="功能说明" rw_stat="read">run until one of children success </comment>
			<op_idle name="idle"  kinds="hide" onlyForDebug="yes"></op_idle>
			
			<op_clear_error name="清除错误,非系统错误" kinds="hide" >
				<internal_label name="标签$name$"  kinds="string" replace_val=".name" auto_index_ref="auto_index" >_sequenceStartLab</internal_label>
			</op_clear_error>
			
			<steps_collection name="selector-children"  create_template="create_bt_sequence_child" delete="no">
			</steps_collection>
			
			<op_short_jump name="break" kinds="hide" expand_list="comment in any-case"  >
				<jumpOffset kinds="hide" delete="no" >0</jumpOffset>
			</op_short_jump>
			
			<op_set_label name="标签$name$" kinds="hide" expand="yes" comp_cond="param"  delete="no">
				<param name="标签名" kinds="string" delete="no" auto_index_ref="auto_index" replace_val="../.name" >_sequenceEndLab</param>	
			</op_set_label>				
			
			<op_waitevent name="节点_等待0" kinds="hide"  delete="no">
				<varInt name="事件名" >0</varInt>
			</op_waitevent>			
			<op_goto_label name="goto:$name$" expand="yes" comp_cond="ref_from" kinds="hide"  delete="no">
				<ref_from name="跳转地址" >../../op_clear_error/internal_label</ref_from>
			</op_goto_label>		
			
		</steps_bt_sequence>
	</create_bt_sequence>
		
	<create_bt_sequence_child name="sequence-entry" create_type="1">
		<steps_collection name="sequence-node">
			<param name="subSequence" kinds="string" delete="no" auto_index_ref="auto_index"  referenced_only="yes" >Node</param>
			<steps_collection name="sequence-node-entry" create_template="create_list1" delete="no">
			</steps_collection>			
			<op_check_user_error name="检测是否出错"  kinds="hide" delete="no"></op_check_user_error>					
			<op_goto_label name="test-success-jump:$name$" expand="yes" comp_cond="ref_from" kinds="hide" value="18" delete="no">
				<ref_from name="跳转地址" >../../../../op_set_label/param</ref_from>
			</op_goto_label>	
		
		</steps_collection>
	</create_bt_sequence_child>	
	<!-- -->
	
	<create_input_param name="创建参数参数" create_type="1" >
		<param_collect name="参数" expand="yes">
			<type kinds="reference" reference_type="type_data_type" delete="no">18</type>
			<var kinds="string" delete="no" restrict="type">0</var>			
		</param_collect>
	</create_input_param>
	
	
	<create_user_def_param name="自定义类型参数" create_type="1" >
		<param_collect name="参数" expand="yes">
			<func_name name="成员名" kinds="string" delete="no" is_param_name="yes">member1</func_name>
			<type kinds="reference" reference_type="type_data_type" delete="no">18</type>
			<var kinds="string" delete="no" restrict="type">0</var>			
		</param_collect>
	</create_user_def_param>
	
	<create_function_init_block name="初始化模块" create_type="1" >
		<function_init_block name="block_加载时运行" create_template="create_list2" expand_list="condition" auto_index="0" >
			<comment name="功能说明" rw_stat="read">脚本被加载到内存是运行这部分代码</comment>
		</function_init_block>
	</create_function_init_block>
	
	<create_list1 name="create_base_function" create_type="2">
		<list>create_function_init_block</list>
		<list>create_step_error_catch_block</list>		
		<list>create_step_print</list>
		<list>create_step_log</list>
		<list>create_step_throw</list>		
		<list>create_step_exit</list>

		<list>create_get_last_error</list>	
		<list>create_step_set_error</list>
		
		<list>create_skip_error</list>
		<!-- list>create_get_simulate_stat</list -->
		<list>create_error_continue</list>
		
		<list>create_step_read</list>
		<list>create_step_write</list>
		<list>create_step_add</list>
		<list>create_step_sub</list>
		<list>create_step_test</list>	
		<list>create_step_operate</list>	
		
		
		<list>create_read_table</list>
		<list>create_step_calc</list>
		<list>create_step_getother_object</list>
		
		
		<list>create_step_waitevent</list>
		<list>create_step_send_event</list>
		<list>create_install_event</list>
		<list>create_remove_event</list>
		<list>create_add_timer</list>
		<list>create_del_timer</list>
		<list>create_step_call_func</list>
		<list>create_op_comp</list>
		<!-- list>create_step_check_valid</list -->
		<list>create_step_bool_entry</list>
		<list>create_step_loop_entry</list>	
		<list>create_comp_steps</list>
		<list>create_switch_case</list>
		
		
		<list>create_break</list>
		<list>create_success_break</list>		
		<list>op_error_break</list>
		<list>create_step_true_exit</list>	
		<list>create_step_false_eixt</list>	
		
		<list>create_test_current_value</list>
		<list>create_test_machine_is_simulate</list>		
		<list>create_test_script_debug</list>	
		<list>create_test_host_debug</list>
		
		<list>create_get_arrsize</list>		
		<list>create_make_var</list>
		<list>create_global_var</list>
		
		<list>create_step_type_transfer</list>
		
		<list>create_build_json_data</list>
		<list>create_build_struct_type</list>
		<list>create_step_get_user_data_type</list>
		
		<list>create_math_operate</list>
		<list>create_bit_operate</list>
		
		<list>create_assignin</list>
		
		<list>create_step_inistall_msg_handler</list>
		<list>create_step_send_msg</list>	
		
		<list>create_step_read_msg</list>	
		<list>create_step_read_datatype_msg</list>
		
		<list>create_step_get_time</list>
		<list>create_step_changed_time</list>
		<list>create_step_time_function</list>
		
		<list>create_step_begin_affair</list>
		<list>create_step_commit_affair</list>
		
		<list>create_step_read_excel_attr</list>
		
		<list>create_step_chdir</list>
		<list>create_step_rmfile</list>	
		<list>create_step_mkdir</list>
		<list>create_step_process_exit</list>	
		<list>create_step_process_abort</list>	
		
		<list>create_bt_selector</list>
		<list>create_bt_sequence</list>
	</create_list1>		
	<create_list2 name="create_message_handler" create_type="2" ref_from="yes">../create_list1</create_list2>
</create_template>