//
//  crypt_file.h
//  gameHall
//
//  Created by duanxiuyun on 14-11-19.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

// note please include this only you need this functions. It's a better way include it after other header-file.

//#ifndef gameHall_crypt_file_h
//#define gameHall_crypt_file_h


//#endif



#include "nd_crypt/nd_cryptfile.h"
#undef FILE
#define FILE NDC_FILE

#undef fread
#define fread ndc_fread

#undef fwrite
#define fwrite ndc_fwrite

#undef fclose
#define fclose ndc_fclose

#undef fseek
#define fseek ndc_fseek

#undef ftell
#define ftell ndc_ftell

#undef fflush
#define fflush ndc_fflush

//crypt read write file
static const char* get_pwd_text(char *buf, int inlen)
{
#ifdef ND_DEBUG
	char tmp_buf[128];
	int size = snprintf(tmp_buf, sizeof(tmp_buf), "%s%s", __TIME__,__DATE__) ;

	MD5CryptToStr32(tmp_buf, size, buf) ;
#else
	
        MD5CryptToStr32((char*)"test-duan", 9, buf) ;
#endif
	buf[32] = 0 ;
	return buf ;
}


static NDC_FILE *fopen_ex(const char * filename, const char *mode)
{
	char pwd_buf[33] ;
	if (0==strcmp(mode, "r") ||0==strcmp(mode, "rb") || 0==strcmp(mode, "r+b")) {
		return ndc_fopen_r( filename, get_pwd_text(pwd_buf,sizeof(pwd_buf)) ) ;
	}
	else {
		return ndc_fopen_w( filename, get_pwd_text(pwd_buf,sizeof(pwd_buf)) ) ;
	}

}

#undef fopen
#define fopen fopen_ex
