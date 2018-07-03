/* file resumable.js
 *
 * break-point resumable transfer 
 *
 * create by duan 
 * 2014-10-11
 */

var fs = require('fs') ;
var path = require('path');

var sheldonLog = require('./sheldonLog') ;

function getStartPos(Range) {
    var startPos = 0;
    if( typeof Range != 'undefined') {
        var startPosMatch = /^bytes=([0-9]+)-$/.exec(Range);
        startPos = Number(startPosMatch[1]);
    }
    return startPos;
}

function configHeader(Config, resp) {
    var startPos = Config.startPos,
        fileSize = Config.fileSize;

    if (startPos == 0) {
        resp.setHeader('Accept-Range', 'bytes');
    } else {
        resp.setHeader('Content-Range', 'bytes ' + startPos + '-' + (fileSize - 1) + '/' + fileSize);
    }
    resp.writeHead(206, 'Partial Content', {
        'Content-Type': 'application/octet-stream',
    });
}


function beginDownload(filePath, req, resp, down) {

    var config = {};

    fs.stat(filePath, function (error, state) {
        if (error)
            throw error;

        config.fileSize = state.size;


        sheldonLog.debug("download file header.range = ", req.headers.range);

        config.startPos = getStartPos(req.headers.range);
        if (0 == config.startPos) {
            resp.writeHead(200, {
                "Content-Type": getFileType(filePath),
                "Content-Length": config.fileSize,
                "Server": "NodeJs(" + process.version + ")"
            });
        }
        else {
            configHeader(config, resp);
        }

        down(config);
    });
}


function Download(filePath, resp, req) {

    fs.exists(filePath, function (exist) {
        if (exist) {
            beginDownload(filePath, req, resp, function (config) {
                fReadStream = fs.createReadStream(filePath, {
                    encoding: 'binary',
                    bufferSize: 1024 * 128,
                    start: config.startPos,
                    end: config.fileSize
                });
                fReadStream.on('data', function (chunk) {
                    resp.write(chunk, 'binary');
                });
                fReadStream.on('end', function () { resp.end(); });
            });
        }
    });
}

/*
exports.resumableBuffer = function(resp, req,buf)
{
    var config = {};
    
    sheldonLog.debug("buf type = ", typeof buf) ;
    
    config.fileSize = buf.length ; //Buffer.byteLength(buf,'binary');
    config.startPos = getStartPos(req.headers.range);
    
    sheldonLog.debug("resumableBuffer() start pos ", config.startPos, "file size = ", config.fileSize) ;
    
    if(config.startPos <0 || config.startPos>=config.fileSize ) {
        return ;
    }
    configHeader(config, resp);
    
    var tmpbuf = new Buffer(buf,"binary") ;
    
    var chunk = new Buffer(config.fileSize - config.startPos) ;
    tmpbuf.copy(chunk,0,config.startPos, config.fileSize );
    
    resp.write(chunk, 'binary');
    resp.end();
    
}

exports.sendBuffByFragment = function(writer, buf,fragmentSize, end_callback)
{
    
    var step = fragmentSize ;
    var fileSize = buf.length ;
    var startPos = 0 ;
    var read_len = 0 ;
    
    var cachebuf = new Buffer(step) ;
    
    sheldonLog.debug("buf type = ", typeof buf, " length = ", fileSize) ;
    
    write();
    function write() {
        var ok = true;
        do {
            read_len =  step ;
            if(read_len > fileSize-startPos) {
                read_len =  fileSize-startPos ;
            }
            
            buf.copy(cachebuf,0,startPos, startPos + read_len );
            startPos += read_len ;
            
            if (startPos >= fileSize) {
                // last time!
                writer.write(cachebuf, "binary", end_callback);
                sheldonLog.debug(" write data complete startPos= ", startPos, "filesize= ", fileSize ) ;
            } else {
                // see if we should continue, or wait
                // don't pass the callback, because we're not done yet.
                ok = writer.write(cachebuf, "binary");
                //sheldonLog.debug("startPos = " ,startPos, "last write data read_le = ", read_len, "cachebuf = ", cachebuf.length ) ;
            }
        } while (startPos < fileSize && ok);
        
        if (startPos < fileSize) {
            // had to stop early!
            // write some more once it drains
            writer.once('drain', write);
            
            //sheldonLog.debug("startPos = " ,startPos, " write drain " ) ;
        }
    }
    
}
*/

exports.resumableDownload = Download ;

var fileTypeList = { ".3gp"   : "video/3gpp"
          , ".a"     : "application/octet-stream"
          , ".ai"    : "application/postscript"
          , ".aif"   : "audio/x-aiff"
          , ".aiff"  : "audio/x-aiff"
          , ".asc"   : "application/pgp-signature"
          , ".asf"   : "video/x-ms-asf"
          , ".asm"   : "text/x-asm"
          , ".asx"   : "video/x-ms-asf"
          , ".atom"  : "application/atom+xml"
          , ".au"    : "audio/basic"
          , ".avi"   : "video/x-msvideo"
          , ".bat"   : "application/x-msdownload"
          , ".bin"   : "application/octet-stream"
          , ".bmp"   : "image/bmp"
          , ".bz2"   : "application/x-bzip2"
          , ".c"     : "text/x-c"
          , ".cab"   : "application/vnd.ms-cab-compressed"
          , ".cc"    : "text/x-c"
          , ".chm"   : "application/vnd.ms-htmlhelp"
          , ".class"   : "application/octet-stream"
          , ".com"   : "application/x-msdownload"
          , ".conf"  : "text/plain"
          , ".cpp"   : "text/x-c"
          , ".crt"   : "application/x-x509-ca-cert"
          , ".css"   : "text/css"
          , ".csv"   : "text/csv"
          , ".cxx"   : "text/x-c"
          , ".deb"   : "application/x-debian-package"
          , ".der"   : "application/x-x509-ca-cert"
          , ".diff"  : "text/x-diff"
          , ".djv"   : "image/vnd.djvu"
          , ".djvu"  : "image/vnd.djvu"
          , ".dll"   : "application/x-msdownload"
          , ".dmg"   : "application/octet-stream"
          , ".doc"   : "application/msword"
          , ".dot"   : "application/msword"
          , ".dtd"   : "application/xml-dtd"
          , ".dvi"   : "application/x-dvi"
          , ".ear"   : "application/java-archive"
          , ".eml"   : "message/rfc822"
          , ".eps"   : "application/postscript"
          , ".exe"   : "application/x-msdownload"
          , ".f"     : "text/x-fortran"
          , ".f77"   : "text/x-fortran"
          , ".f90"   : "text/x-fortran"
          , ".flv"   : "video/x-flv"
          , ".for"   : "text/x-fortran"
          , ".gem"   : "application/octet-stream"
          , ".gemspec" : "text/x-script.ruby"
          , ".gif"   : "image/gif"
          , ".gz"    : "application/x-gzip"
          , ".h"     : "text/x-c"
          , ".hh"    : "text/x-c"
          , ".htm"   : "text/html"
          , ".html"  : "text/html"
          , ".ico"   : "image/vnd.microsoft.icon"
          , ".ics"   : "text/calendar"
          , ".ifb"   : "text/calendar"
          , ".iso"   : "application/octet-stream"
          , ".jar"   : "application/java-archive"
          , ".java"  : "text/x-java-source"
          , ".jnlp"  : "application/x-java-jnlp-file"
          , ".jpeg"  : "image/jpeg"
          , ".jpg"   : "image/jpeg"
          , ".js"    : "application/javascript;charset=utf-8"
          , ".json"  : "application/json"
          , ".log"   : "text/plain;charset=utf-8"
          , ".m3u"   : "audio/x-mpegurl"
          , ".m4v"   : "video/mp4"
          , ".man"   : "text/troff"
          , ".mathml"  : "application/mathml+xml"
          , ".mbox"  : "application/mbox"
          , ".mdoc"  : "text/troff"
          , ".me"    : "text/troff"
          , ".mid"   : "audio/midi"
          , ".midi"  : "audio/midi"
          , ".mime"  : "message/rfc822"
          , ".mml"   : "application/mathml+xml"
          , ".mng"   : "video/x-mng"
          , ".mov"   : "video/quicktime"
          , ".mp3"   : "audio/mpeg"
          , ".mp4"   : "video/mp4"
          , ".mp4v"  : "video/mp4"
          , ".mpeg"  : "video/mpeg"
          , ".mpg"   : "video/mpeg"
          , ".ms"    : "text/troff"
          , ".msi"   : "application/x-msdownload"
          , ".odp"   : "application/vnd.oasis.opendocument.presentation"
          , ".ods"   : "application/vnd.oasis.opendocument.spreadsheet"
          , ".odt"   : "application/vnd.oasis.opendocument.text"
          , ".ogg"   : "application/ogg"
          , ".p"     : "text/x-pascal"
          , ".pas"   : "text/x-pascal"
          , ".pbm"   : "image/x-portable-bitmap"
          , ".pdf"   : "application/pdf"
          , ".pem"   : "application/x-x509-ca-cert"
          , ".pgm"   : "image/x-portable-graymap"
          , ".pgp"   : "application/pgp-encrypted"
          , ".pkg"   : "application/octet-stream"
          , ".pl"    : "text/x-script.perl"
          , ".pm"    : "text/x-script.perl-module"
          , ".png"   : "image/png"
          , ".pnm"   : "image/x-portable-anymap"
          , ".ppm"   : "image/x-portable-pixmap"
          , ".pps"   : "application/vnd.ms-powerpoint"
          , ".ppt"   : "application/vnd.ms-powerpoint"
          , ".ps"    : "application/postscript"
          , ".psd"   : "image/vnd.adobe.photoshop"
          , ".py"    : "text/x-script.python"
          , ".qt"    : "video/quicktime"
          , ".ra"    : "audio/x-pn-realaudio"
          , ".rake"  : "text/x-script.ruby"
          , ".ram"   : "audio/x-pn-realaudio"
          , ".rar"   : "application/x-rar-compressed"
          , ".rb"    : "text/x-script.ruby"
          , ".rdf"   : "application/rdf+xml"
          , ".roff"  : "text/troff"
          , ".rpm"   : "application/x-redhat-package-manager"
          , ".rss"   : "application/rss+xml"
          , ".rtf"   : "application/rtf"
          , ".ru"    : "text/x-script.ruby"
          , ".s"     : "text/x-asm"
          , ".sgm"   : "text/sgml"
          , ".sgml"  : "text/sgml"
          , ".sh"    : "application/x-sh"
          , ".sig"   : "application/pgp-signature"
          , ".snd"   : "audio/basic"
          , ".so"    : "application/octet-stream"
          , ".svg"   : "image/svg+xml"
          , ".svgz"  : "image/svg+xml"
          , ".swf"   : "application/x-shockwave-flash"
          , ".t"     : "text/troff"
          , ".tar"   : "application/x-tar"
          , ".tbz"   : "application/x-bzip-compressed-tar"
          , ".tcl"   : "application/x-tcl"
          , ".tex"   : "application/x-tex"
          , ".texi"  : "application/x-texinfo"
          , ".texinfo" : "application/x-texinfo"
          , ".text"  : "text/plain"
          , ".tif"   : "image/tiff"
          , ".tiff"  : "image/tiff"
          , ".torrent" : "application/x-bittorrent"
          , ".tr"    : "text/troff"
          , ".txt"   : "text/plain"
          , ".vcf"   : "text/x-vcard"
          , ".vcs"   : "text/x-vcalendar"
          , ".vrml"  : "model/vrml"
          , ".war"   : "application/java-archive"
          , ".wav"   : "audio/x-wav"
          , ".wma"   : "audio/x-ms-wma"
          , ".wmv"   : "video/x-ms-wmv"
          , ".wmx"   : "video/x-ms-wmx"
          , ".wrl"   : "model/vrml"
          , ".wsdl"  : "application/wsdl+xml"
          , ".xbm"   : "image/x-xbitmap"
          , ".xhtml"   : "application/xhtml+xml"
          , ".xls"   : "application/vnd.ms-excel"
          , ".xml"   : "application/xml"
          , ".xpm"   : "image/x-xpixmap"
          , ".xsl"   : "application/xml"
          , ".xslt"  : "application/xslt+xml"
          , ".yaml"  : "text/yaml"
          , ".yml"   : "text/yaml"
          , ".zip"   : "application/zip"
          } ;


var path=require("path");
function getFileType( fileName )
{
	var extName = path.extname(fileName).toLowerCase();
    return fileTypeList.extName || "application/octet-stream";
}

exports.getFileType = getFileType;