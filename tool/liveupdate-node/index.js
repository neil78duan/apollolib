#!/usr/local/bin/node

/*
 * file index.js
 * server program entry
 * create by duan 
 * 2014-7-21
 * all right reserved by Citrusjoy
 */

var configFileName = '' ;
var config_info ={} ;
var myProcessID ;

var fs=require("fs");

var serverStartTime = new Date() ;

function parseArgs()
{
    //console.log("begin parse args ",process.execArgv) ;
    var workingDir = '' ;
    myProcessID = process.pid ;
    
    try {
        for(var i=0 ; i<process.argv.length; i++) {
            if(process.argv[i].toLowerCase() =='--path') {
                if(i <process.argv.length -1) {
                    workingDir = process.argv[i+1] ;
                    i++ ;
                }
            }
            
            else if(process.argv[i].toLowerCase() =='--config') {
                if(i <process.argv.length -1) {
                    configFileName = process.argv[i+1] ;
                    i++ ;
                }
            }
            else if(process.argv[i].toLowerCase() =='--help') {
                console.log("USAGE: node index.js [--path working_path] [--config configFileName ] [--help]  ") ;
                process.exit(1) ;
            }
            
            else if(process.argv[i].toLowerCase() =='--pidfile') {
                if(i <process.argv.length -1) {
                    var pidf = process.argv[i+1] ;
                    fs.writeFileSync(pidf, myProcessID);
                }
            }
            
            
        }
        if(workingDir && workingDir.length > 0) {
            process.chdir(workingDir) ;
            //console.log("change to ", workingDir) ;
        }
        
    }
    catch (e) {
        console.log("parse error : exception ", e) ;
        process.exit(1) ;
    }
    
    if(configFileName && configFileName.length > 0) {
        config_info = require(configFileName);
        //console.log("load file ", configFileName) ;
    }
    else {
        config_info = require('./configuration.json');
    }
    
    //console.log("working dir = ", workingDir, "config file =",configFileName) ;
}
parseArgs() ;

exports.configFileName = configFileName ;
exports.config_info = config_info ;
exports.serverStartTime = serverStartTime ;

var sheldonLog = require('./sheldonLog') ;
var route = require('./route') ;

function checkHandle(response, request)
{
    response.writeHead(200, {"Content-Type": "text/html"});
    response.write("I am OK.<br/>" + new Date() + '<br/> Server Start Time : ' + serverStartTime);
    response.end();
}

function errorHandle(response, request)
{
    //console.log("request \n", request) ;
    response.writeHead(404, {"Content-Type": "text/html"});
    response.write("ERROR not found 404");
    response.end();
}


exports.ResponeClient =function (response, body)
{
	var headers = {};
	headers["Access-Control-Allow-Origin"] = "*";
	headers["Access-Control-Allow-Methods"] = "POST, GET, OPTIONS";
	headers["Access-Control-Allow-Headers"] = "X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept";
	headers["Content-Type"] = "application/json; charset=utf-8";
	response.writeHead(200, headers);
	response.write(JSON.stringify(body));
	response.end();
	
}
exports.ResponeError=function (response, errorCode, errorText)
{
	
	response.writeHead(errorCode, {"Content-Type": "text/html"});
	response.write(errorText);
	response.end();
}

// for browser
var sheldonGet_handle = {} ;
sheldonGet_handle["/check"] = checkHandle ;
sheldonGet_handle["/"] = errorHandle ;


//start live update
var downFile = require("./downloadFile.js");

sheldonGet_handle["/loaddata"] = downFile.DownLoadData ;
//sheldonGet_handle["/loadversion"] = downFile.DownLoadVersion ;
sheldonGet_handle["/getlist"] = downFile.DownLoadList


//start game server
route.routeStart(config_info.port, null, sheldonGet_handle,false) ;

//start
//if (config_info.https_port != null ) {
//    route.routeStart(config_info.https_port, sheldon.routeTable, sheldonGet_handle,true,true) ;
//}


var gmtool = require('./gmTool');
//start GM tool
if (config_info.openGMTool==null ||  config_info.openGMTool.toLowerCase() != 'no' ) {
    gmtool.start() ;
}

//sheldonGet_handle['/showversion'] = gmtool.showVersion;

/*
 所有 HTTP 状态代码及其定义。
 　代码  指示
 2xx  成功
 200  正常；请求已完成。
 201  正常；紧接 POST 命令。
 202  正常；已接受用于处理，但处理尚未完成。
 203  正常；部分信息 — 返回的信息只是一部分。
 204  正常；无响应 — 已接收请求，但不存在要回送的信息。
 3xx  重定向
 301  已移动 — 请求的数据具有新的位置且更改是永久的。
 302  已找到 — 请求的数据临时具有不同 URI。
 303  请参阅其它 — 可在另一 URI 下找到对请求的响应，且应使用 GET 方法检索此响应。
 304  未修改 — 未按预期修改文档。
 305  使用代理 — 必须通过位置字段中提供的代理来访问请求的资源。
 306  未使用 — 不再使用；保留此代码以便将来使用。
 4xx  客户机中出现的错误
 400  错误请求 — 请求中有语法问题，或不能满足请求。
 401  未授权 — 未授权客户机访问数据。
 402  需要付款 — 表示计费系统已有效。
 403  禁止 — 即使有授权也不需要访问。
 404  找不到 — 服务器找不到给定的资源；文档不存在。
 407  代理认证请求 — 客户机首先必须使用代理认证自身。
 415  介质类型不受支持 — 服务器拒绝服务请求，因为不支持请求实体的格式。
 5xx  服务器中出现的错误
 500  内部错误 — 因为意外情况，服务器不能完成请求。
 501  未执行 — 服务器不支持请求的工具。
 502  错误网关 — 服务器接收到来自上游服务器的无效响应。
 503  无法获得服务 — 由于临时过载或维护，服务器无法处理请求。
 */
