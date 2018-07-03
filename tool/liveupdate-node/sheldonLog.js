
/*
 * file sheldonLog.js
 * config log 
 * create by duan 
 * 2014-7-30
*/


var isClose = 0 ;
var useLog4 = 1 ; //1 console , 2 log4 ,3 all
var level = 0 ; //debug



//var logger = require('log4node');


var log4js = require('log4js');
var logger = log4js.getLogger();

//logger4.debug("Some debug messages");

/*
 
 logger.trace('Entering cheese testing');
 logger.debug('Got cheese.');
 logger.info('Cheese is Gouda.');
 logger.warn('Cheese is quite smelly.');
 logger.error('Cheese is too ripe!');
 logger.fatal('Cheese was breeding ground for listeria.');
 
 
 */

function loadLoggerInfo()
{
    
    var config_info = require('./index.js').config_info;
    
    var logCfg = config_info.logger ;
    
    if (!logCfg) {
        level = 2 ;
    }
    else if(logCfg.enable.toLowerCase() == "yes") {
        if(logCfg.logtype.toLowerCase() =="console" ) {
            useLog4 = 1 ;
        }
        else if(logCfg.logtype.toLowerCase() =="log4" ) {
            useLog4 = 2 ;
        }
        else if(logCfg.logtype.toLowerCase() =="all" ) {
            useLog4 = 3 ;
        }
        
        //common ,warn, error ,fatal
        if(logCfg.loglevel.toLowerCase() == "debug" ) {
            level = 0 ;
        }
        else if(logCfg.loglevel.toLowerCase() == "common" ) {
            level = 1 ;
        }
        else if(logCfg.loglevel.toLowerCase() == "warn" ) {
            level = 2 ;
        }
        
        else if(logCfg.loglevel.toLowerCase() == "error" ) {
            level = 3 ;
        }
        
        else if(logCfg.loglevel.toLowerCase() == "fatal" ) {
            level = 4 ;
        }

    }
    
    if(useLog4 & 2) {
        log4js.configure(logCfg.log4_config) ;
    }
    
}

loadLoggerInfo() ;


exports.log = function () {
    if (isClose)
        return ;
        
    if(level >1) {
        return ;
    }
        
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.info(params) ;
    }
    if(useLog4 & 1) {
        console.log("LOG " , params) ;
    }

}
exports.trace = function () {
    if (isClose)
        return ;
    
    if(level >1) {
        return ;
    }
    
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.trace(params) ;
    }
    if(useLog4 & 1){
        console.log("TRACE " , params) ;
    }
    
}

exports.debug = function () {
    if (isClose)
        return ;
    if(level > 0){
        return ;
    }
        
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.debug(params) ;
    }
    if(useLog4 & 1){
        console.log("DEBUG ", params) ;
    }

    
}

exports.info = function () {
    if (isClose)
        return ;
    
    if(level >1) {
        return ;
    }
    
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.info(params) ;
    }
    if(useLog4 & 1){
        console.log("INFO " , params) ;
    }

    
}

exports.warn = function () {
    if (isClose)
        return ;
    
    if(level >2) {
        return ;
    }
    
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.warn(params) ;
    }
    if(useLog4 & 1){
        console.warn("WARN ", params) ;
    }

    
}

exports.error =function () {
    if (isClose)
        return ;
    
    if(level >3) {
        return ;
    }
    
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.error(params) ;
    }
    if(useLog4 & 1){
        console.error("ERROR ", params) ;
    }
}

exports.fatal =function () {
    
    //console.log("call fatal") ;
    if (isClose){
        //console.log("closed") ;
        return ;
    }
    var params = "";
    for (var i=0; i<arguments.length; i++) {
        params = params + " " + arguments[i];
    }
    if(useLog4 & 2) {
        logger.fatal(params) ;
    }
    if(useLog4 & 1){
        console.error("FATAL ", params) ;
    }

}




