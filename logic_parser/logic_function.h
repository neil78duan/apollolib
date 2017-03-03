/* file logicengine_api.h
 *
 * interface of game logic engine 
 *
 * create by duan 
 *
 * 2015-5-20
 */

#ifndef _LOGIC_ENGINE_API_H_
#define _LOGIC_ENGINE_API_H_

#include "nd_common/nd_common.h"

class LogicEngineRoot;
class LogicParserEngine;
//install c-api
int init_sys_functions(LogicEngineRoot *root);

int common_export_c_api_descript(const char *outfile);

int common_export_event_id_descript(const char *outfile);

int common_export_error_list(const char *outfile);//export error to xml file 

bool apollo_printf(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
bool apollo_log(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
bool apollo_time_func(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
int apollo_logic_out_put(const char *text);
bool apollo_load_file_data(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
bool apollo_write_file(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);

#ifdef ND_OUT_LOG_2CTRL

int _apollo_script_printf(const char *stm, ...);
#define apollo_script_printf _apollo_script_printf

#else 
#define apollo_script_printf(stm,...)
#endif


#endif
