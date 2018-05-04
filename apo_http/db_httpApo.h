/* file db_httpApo.h
 *
 * mysql database interface for aposcript
 *
 * create by duan
 *
 * 2018.4.17
 */

#ifndef _DB_HTTP_APO_H_
#define _DB_HTTP_APO_H_

#include "ndapplib/applib.h"
#include "ndapplib/nd_httpListener.h"
#include "logic_parser/logicEngineRoot.h"

int apoMysqlDBInit();		//must call this function to avoid cpp-link skip this module;
#endif 
