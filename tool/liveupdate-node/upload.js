/* file upload.js
 * upload file to server
 *
 * create by duan 
 *
 * 2015-3-16
 * duan
 */


var http = require("http");
var path=require("path");
var fs=require("fs");
var crypto = require('crypto') ;
var sheldon = require("./index");
var config_info = sheldon.config_info;
var sheldonLog = require('./sheldonLog') ;

var resumable = require('./resumable') ;

var verFileGlobal = "/version.txt" ;
//var dataFileGlobal = "/data.zip" ;
var __uploadLog = "/upload.log";
var __dataList = "/datalist.txt";


function getMd5FromFile(filePath)
{
	var ret = '' ;
	try {
		ret = crypto.createHash('md5').update(fs.readFileSync(filePath, "binary")).digest('hex');
	}
	catch (e) {
		sheldonLog.error("getMd5FromFile() catch execption : ", e) ;
		return '' ;
	}
	return ret ;
}

function checkUploadMd5(md5Info, datafile)
{
	var md5Res = getMd5FromFile(datafile.path) ;
	if(md5Info.DataMd5 != md5Res) {
		sheldonLog.error("Upload " , datafile.name, " error md5 not match file-md5=", md5Res) ;
		return false ;
	}
	
	return true ;
}

function trim_verID(strInput){ //过滤字符中的全部空格
	
	var result = "";
 
	for (var i=0; i < strInput.length; i++) {
		var test = strInput[i];
 		if(test > ' '){
			result += test ;
		}
	};
	return result;
}


function saveFiles(response,FileVersion, FileData,FileMd5, desc, md5Json) {
	
	sheldonLog.log("enter saveFile() ") ;
	try {
		
		
		if(md5Json && !checkUploadMd5(md5Json,FileData) ) {
			sheldon.ResponeError(response,200, '<a href="/">Home</a><br /><br />Upload file ERROR MD5 check error ' ) ;
			return true ;
		}
		
		var verFile = FileVersion.path ;
		var dataFile = FileData.path ;
		
		//sheldonLog.log("get upload file ", verFile, dataFile);
		
		var verID = fs.readFileSync(verFile, "utf8");
		verID = trim_verID(verID) ;
		
		var aimDir = config_info.liveUpdateDir + '/' + verID;
		
		if(fs.existsSync(aimDir)) {
			sheldon.ResponeError(response,200, '<a href="/">Home</a><br /><br />Upload version already exist ' + verID) ;
			return true ;
		}
		
		fs.mkdirSync(aimDir);
		sheldonLog.debug("upload file to ", aimDir);
		
		var dataPath = aimDir + "/" + FileData.name;
		
		fs.renameSync(verFile, aimDir+verFileGlobal ) ;
		fs.renameSync(dataFile, dataPath ) ;
		
		if (FileMd5 && FileMd5.size > 0 ) {
			fs.renameSync(FileMd5.path, aimDir + "/md5file.txt" ) ;
			//console.log(" md5 file :" ,FileMd5) ;
		}
		
		if(fs.existsSync(aimDir+verFileGlobal) && fs.existsSync(dataPath) ) {
			
			sheldonLog.info("upload version file success version-id = ", verID);
			
			var downUrl =config_info.my_domain_name +':' +
				config_info.port.toString() + '/loaddata?data=' + verID +'&name=' + FileData.name;
			
			var backTip = '<a href="/">Home</a><br /><br />Upload version success ! ' +
				'<br />  url is : ' + downUrl ;
			
			sheldon.ResponeError(response,200,  backTip) ;
			
			//write log
			fs.appendFile(config_info.liveUpdateDir  + __uploadLog,
				'\n<br />' + verID + ' : ' + downUrl) ;
			
			// write file list
			fs.appendFile(config_info.liveUpdateDir +  __dataList,
				verID + ' : ' + FileData.name + '\n') ;
			
			
			return true ;
		}
		else {
			sheldon.ResponeError(response,500, '<a href="/">Home</a><br /><br />Upload version error  ') ;
			return true ;
		}
		
	}
	catch (e) {
		
		sheldonLog.log("liveUpdate::saveFiles() catch exception ", e) ;
	}
	return false ;
}

function getMd5Info(inputMd5InfoFile, inputDataFile)
{
	var ret = {} ;
	try {
		if(inputMd5InfoFile.size==0) {
			return NULL ;
		}
		
		var Md5Text = fs.readFileSync(inputMd5InfoFile.path, "utf8");
		var inputArrayTxt = Md5Text.split(',',10);
		
		
		for(var i=0; i<inputArrayTxt.length; i++){
			var FieldTxt = inputArrayTxt[i].split('=',2) ;
			
			if(FieldTxt.length == 2  ) {
				
				if(inputDataFile.size >0 && FieldTxt[0].indexOf(inputDataFile.name)!=-1 ) {
					ret.DataMd5 = FieldTxt[1] ;
				}
			}
			
			
		}
		
	}
	catch (e) {
		
		sheldonLog.log("liveUpdate::getMd5Info() catch exception ", e) ;
	}
	
	//console.log("file md5 text= ", ret) ;
	return ret ;
}

exports.UploadVersionHandle = function (response, request) {
	
	sheldonLog.debug("Request handler 'UploadVersionHandle' was called ");
	
	try
	{
		var bsuccess = false ;
		var querystring = require("querystring"),
		fs = require("fs"),
		formidable = require("formidable");
		
		
		var form = new formidable.IncomingForm();
		var uploadPathTmp = config_info.liveUpdateDir + '/.tmp' ;
		
		if(!fs.existsSync(uploadPathTmp)) {
			fs.mkdirSync(uploadPathTmp);
		}
		form.uploadDir= uploadPathTmp ;
		
		//console.log("about to parse");
		form.parse(request, function(error, formData, files) {
				   //console.log("parse notice data done", files);
		   if(error ) {
				sheldonLog.error("add UploadVersionHandle error", error) ;
				
		   }

		   else if(files && files.VersionFile && files.VersionFile.size > 0 && files.DataFile && files.DataFile.size > 0) {
		   
		   		var md5Json = {} ;
		   		if(files.Md5Info.size > 0 ){
		   			md5Json = getMd5Info(files.Md5Info, files.DataFile);
		   		}
		   		bsuccess = saveFiles(response,files.VersionFile ,files.DataFile,files.Md5Info, formData.VerDesc, md5Json) ;
		   }
		   if( bsuccess == false) {
		   		sheldon.ResponeError(response,400, "parse data error");
		   }

		});
	}
	catch (e) {
		
		sheldonLog.log("UploadVersionHandle() catch exception ", e) ;
		sheldon.ResponeError(response,500, "Internal error");
	}
	return 200 ;
}


exports.getUploadLog =function( callback )
{
	sheldonLog.debug("Request getUploadLog() ");
	
	//try {
		var logfile = config_info.liveUpdateDir + __uploadLog ;
		if(fs.existsSync(logfile)) {
			fs.readFile(logfile, "utf8", function(err, file) {
				if(!err && file) {
					callback(file.toString()) ;
				}
			}) ;
		}
		else {
			callback("log file not found") ;
			
			sheldonLog.debug(logfile, "file not found ");
		}

}

exports.requestUploadVersion = function (response)
{
	
	//console.log("Request handler 'start' was called.");
	
	var body = '<html>'+
	'<head>'+
	'<meta http-equiv="Content-Type" '+
	'content="text/html; charset=UTF-8" />'+
	'</head>'+
	'<body>'+
	
	'<a href="/">Home</a><br />' +
	
	'<br />' +
	'<br />' +
	
	'<form action="/uploadversion" method="post" enctype="multipart/form-data"> '+
	
	'Version File :' +
	'<input type="file" name="VersionFile" multiple="multiple">'+
	
	'Data File :' +
	'<input type="file" name="DataFile" multiple="multiple">'+
	'<br />' +
	
	
	'Md5-Info File :' +
	'<input type="file" name="Md5Info" multiple="multiple">'+
	'<br />' +
	
	'Description: ' +
	'<input type="text" name="VerDesc" />' +
	'<br />' +
	
	
	'<input type="submit" value="Submit" />' +
	'</form>'+
	'</body>'+
	'</html>';
	
	response.writeHead(200, {"Content-Type": "text/html"});
	response.write(body);
	response.end();
}
