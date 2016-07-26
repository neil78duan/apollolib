/* file route.js
 *
 * route of sheldon server
 *
 * create by duan 
 *
 * 2014-8-13
 */


var http = require("http");
var url = require("url");
var qs = require("querystring");
var sheldonLog = require('./sheldonLog') ;
var https = require('https') ;
var fs = require('fs');


//request route
function request_route(post_handle,get_handle, inPath, response, request,bIsJasonData) {
	
	try {
		var pathname = inPath.toLowerCase() ;
		
		if(request.method == 'POST') {
			post_entry(post_handle, pathname,response, request,bIsJasonData);
		}
		else if (request.method == 'OPTIONS') {
			var headers = {};
			headers["Access-Control-Allow-Origin"] = "*";
			headers["Access-Control-Allow-Methods"] = "POST, GET, OPTIONS";
			headers["Access-Control-Allow-Headers"] = "X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept";
			response.writeHead(200, headers);
			response.end();
		}
		else {
			get_entry(get_handle, pathname,response, request);
		}
	}
	catch (e) {
		sheldonLog.error("request_route() on catch exception ", e) ;
		
	}
	
	
}

function get_entry(handle, pathname,response, request)
{
//	var ret = 200 ;
//	try
	{
		if (typeof handle[pathname] === 'function') {
			ret = handle[pathname](response, request ) ;
		}
		else {
			sheldonLog.debug("GET No request handler found for " + pathname);
			response.writeHead(404, {"Content-Type": "text/html"});
			response.write("Request Error : [" + pathname + "] not found , Please check your request path.");
			response.end();
		}
	}
//	catch (e) {
//        sheldonLog.error("parseRoutePost() on " , pathname, "catch exception ", e) ;
//		sheldonLog.error("parseRoutePost() request host= ", request.headers.host, "method=", request.method) ;
//		ret = 500 ;
//    }
//
//    if (ret != 200) {
//        sheldonLog.error("Get access error when " + pathname);
//        response.writeHead(404, {"Content-Type": "text/html"});
//        response.write("Request Error : [" + pathname + "] not found , Please check your request path. err =", ret);
//        response.end();
//    }
}

function post_entry(handle, pathname,response, request,bIsJasonData)
{
    
    var queryData = "";
    request.on('data', function(data) {
               queryData += data;
               });
    
    request.on('end',function() {
               parseRoutePost(queryData, handle, pathname,response, request,bIsJasonData) ;
               });
    
}
function parseRoutePost(queryData, handle, pathname,response, request, bIsJasonData)
{
    var ret = 404 ;
    //console.log("queryData type =" , typeof queryData ) ;
    try
	{
        var data1={} ;
        if(queryData.length > 0) {

            if(bIsJasonData) {
                data1 = JSON.parse(queryData) ; //parse text from json text
            }
            else {
                data1 = qs.parse(queryData) ;  //parse text from form.
            }

        }
		
		if (typeof handle[pathname] === 'function' ) {
			ret = handle[pathname](response, request, data1);
		}
		else {
			sheldonLog.debug("function [",pathname,"] not found data=", queryData, "type=[",handle[pathname], "]");
		}
        
    }
    catch (e) {
        sheldonLog.error("parseRoutePost() on " , pathname, "catch exception ", e) ;
        sheldonLog.error("parseRoutePost() need json =", bIsJasonData, " data= ", queryData) ;
		sheldonLog.error("parseRoutePost() request host= ", request.headers.host, "method=", request.method) ;

    }

    if (ret != 200) {
        sheldonLog.error("POST access error when " + pathname);
        response.writeHead(ret, {"Content-Type": "text/html"});
        response.write("Request Error : [" + pathname + "] not found , Please check your request path.");
        response.end();
    }

}

//server entry
exports.routeStart = function (port,post_handle, get_handle,bIsJasonData, bHttps) {
    
    function onRequest(request, response) {
        var pathname = url.parse(request.url).pathname;
        request_route(post_handle,get_handle, pathname, response, request,bIsJasonData);
    }
	
	if(bHttps && bHttps==true) {
        var options = {
            key :  fs.readFileSync('./keydir/sheldon_privatekey.pem'),  //带路径的文件名，注意两个文件不要写反了
            cert : fs.readFileSync('./keydir/sheldon_certificate.pem')
        };
        
        https.createServer(options, onRequest).listen(port);
        sheldonLog.log("SSL HTTPS start success " , port, new Date() );
    }
	 else{
		 http.createServer(onRequest).listen(port);
		 sheldonLog.log("Server has started " , port, new Date() );
    }
}

exports.request_route = request_route ;
