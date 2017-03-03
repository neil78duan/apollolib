/* 
 * file gmTool.js
 *
 * simple tools for gm
 *
 * create by duan 
 *
 * 2014-7-31
 */

var sheldon =require('./index.js') ;
var config_info = sheldon.config_info;
var serverStartTime = require('./index.js').serverStartTime;

var sheldonLog = require('./sheldonLog') ;

var liveupdate = require('./upload.js');


var RETURNSUCCESS = 200 ;

function checkIP( request, pathName)
{
    if(!config_info.gm_reliable_ip) {
        return RETURNSUCCESS;
    }
    
    var ips = config_info.gm_reliable_ip ;
    //console.log("ips =", ips) ;
    
    for(var i=0; i<ips.length; i++) {
        var ip = ips[i] ;
        if(ip== "0.0.0.0" || ip == request.connection.remoteAddress) {
            
            sheldonLog.log(request.connection.remoteAddress, " access  ", pathName ) ;
            return RETURNSUCCESS ;
        }
    }
    sheldonLog.log("Try to access manager server come from ", request.connection.remoteAddress ) ;
    return 401;

}

function startPage(response)
{
    var body = '<html>'+
    '<head>'+
    '<meta http-equiv="Content-Type" '+
    'content="text/html; charset=UTF-8" />'+
    '</head>'+
    '<body>'+
	
    '<a href="/liveupdate">Upload Version Data</a>' +
    '<br />' +

    
    '<a href="/uploadlog">View Upload logs</a>' +
    '<br />' +
	
	//'<a href="/listuserid">Show UserId list</a>' +
	//'<br />' +
	
	
    '<a href="/versioninfo">Server Version Info</a>' +
    '<br />' +

    '</body>'+
    '</html>';
    
    response.writeHead(200, {"Content-Type": "text/html"});
    response.write(body);
    response.end();
}

function showVersion(res)
{
	sheldonLog.debug("call gmTool::showVersion() ") ;
    try {
        var os = require('os') ;
        
        var util = require('util');
		var versionExt ='<br /> node.js Start Time : ' + serverStartTime +'<br /> Your IP : '+ res.connection.remoteAddress +
        '<br /> Host name : ' + os.hostname() +
        '<br /> cpu endianness : ' +os.endianness() +
        '<br /> os-type : ' + os.type() +
        '<br /> Platform : ' + os.platform() +
        '<br /> System running time(s): ' + os.uptime() +
        '<br /> Total memory : '+ os.totalmem() /(1024 *1024) + 'M ' +
        '<br /> Free memory : ' + os.freemem() /(1024 *1024) + 'M ' +
        '<br /> Process memory usage : ' + util.inspect(process.memoryUsage()) ;
		
		sheldon.ResponeError(res,200, '<a href="/">Home</a><br /><br />' + 'unknow version' +  versionExt);
		/*
        var fs = require("fs");
		fs.readFile("./VERSION.txt",'utf8',function(err,file){
                    if(err) {
					
                    }
                    else {
                    sheldon.ResponeError(res,200, '<a href="/">Home</a><br /><br />'+ file + versionExt );
					
                    }
                    });
		*/
    }
    catch (e) {
        sheldon.ResponeError(res,200, 'unknow version' + versionExt);
        sheldonLog.error("showVersion() catch excption : " ,e) ;
    }
    
}


function requestShowUploadLogs(response)
{
    liveupdate.getUploadLog( function (logText) {
                            
                            var body = '<html>'+
                            '<head>'+
                            '<meta http-equiv="Content-Type" '+
                            'content="text/html; charset=UTF-8" />'+
                            '</head>'+
                            '<body>'+
                            '<a href="/">Home</a><br />' +
                            logText +
                            '</body>'+
                            '</html>';
                            
                            response.writeHead(200, {"Content-Type": "text/html"});
                            response.write(body);
                            response.end();

                            
                            }) ;
    }


//reset user data



var get_handle = {}
get_handle["/"] = startPage;

get_handle["/liveupdate"] = liveupdate.requestUploadVersion;
get_handle["/versioninfo"] = showVersion;
get_handle["/uploadlog"] = requestShowUploadLogs;



var post_handle = {}

exports.showVersion = showVersion ;


//server entry
exports.start = function () {
    
    var http = require("http");
    var url = require("url");
    var qs = require("querystring");
    var route = require("./route") ;
    
    function onRequest(request, response) {
        
        var pathname = (url.parse(request.url).pathname).toLowerCase() ;
        //console.log ("pathname =", pathname) ;
        if(RETURNSUCCESS!=checkIP( request,pathname) ) {
            return ;
        }
		//过滤post
		if(pathname == "/uploadversion") {
            liveupdate.UploadVersionHandle(response, request);
        
        }
        else {
            route.request_route(post_handle,get_handle, pathname, response, request,false);
        }
    }
    
    http.createServer(onRequest).listen(config_info.gmtoolport);
    sheldonLog.log("GM TOOL Server has started " , config_info.gmtoolport, new Date() );
	
}



