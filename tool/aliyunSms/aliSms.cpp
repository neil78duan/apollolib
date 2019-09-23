/* file aliSms.cpp
 *
 * create by duan 
 *
 * 2019.9.18
 */
//#include "DBRedis.h"
#include "HttpThread.h"
#include "uws.h"
#include "LiveStruct.h"
#include "Config.h"
#include "Packet.h"
#include "RapidJson/document.h"
#include "RapidJson/error/en.h"
//#include "util.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "net.h"
#include "md5_encrypt.h"
#include "DB/DBHead.h"
#include "redis/DBRedisMsg.h"
#include "json/json.h"
#include "DBStructSa.h"

#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "hmaSha1/hmac.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>

int base64_encode(const char * source, int len, char * destination_string);

static unsigned char bits4ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

static unsigned char bits4ToHexLittle(unsigned char x)
{
	return  x > 9 ? x + 87 : x + 48;
}

static unsigned char FromHexChar(unsigned char x)
{
	unsigned char y = 0;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else {}
	return y;
}


static std::string textToURLcode(const char *text, bool isLittle)
{
	std::string strTemp = "";
	while (*text) {
		unsigned char chr1 = *text++;
		if (isalnum(chr1) || (chr1 == '-') || (chr1 == '_') || (chr1 == '.') || (chr1 == '~')) {
			strTemp += chr1;
		}
		else if (chr1 == ' ') {
			strTemp += "+";
		}
		else {
			strTemp += '%';
			unsigned char t1 = isLittle ? bits4ToHexLittle(chr1 >> 4) : bits4ToHex(chr1 >> 4);
			strTemp += t1;
			unsigned char t2 = isLittle ? bits4ToHexLittle(chr1 % 16) : bits4ToHex(chr1 % 16);
			strTemp += t2;
		}
	}
	return strTemp;
}
static std::string URLcodeTotext(const char *urlCode)
{
	std::string strTemp = "";
	while (*urlCode) {
		unsigned char chr1 = *urlCode++;
		if (chr1 == '%') {
			char hiBits4 = *urlCode++;
			if (!hiBits4) {
				break;
			}
			char loBits4 = *urlCode++;
			if (!loBits4) {
				break;
			}
			unsigned char val = FromHexChar(hiBits4) << 4 | FromHexChar(loBits4);
			strTemp += val;
		}
		else if (chr1 == '+') {
			strTemp += " ";
		}
		else {
			strTemp += chr1;
		}
	}
	return strTemp;
}

static std::string getTimeStamp()
{
	time_t now = time(NULL);
	struct tm  tmnow;
	char buf[128];
	
	gmtime_r(&now, &tmnow);
	strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tmnow);	
	return std::string(buf);
}


 //发送短信请求
void SmsCallback(HttpClient* client, HttpResponse* response)
{
	printf("sms ack : %s \n" ,response->getResponseDataString()) ;
	return;
}

static std::string replaceall(std::string str, string old_value, std::string new_value)
{
	while (true) {
		string::size_type  pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}

	return str;
}

std::string sgin(const char * key, const char * data)
{
	char signText[64];
	char signBase64[128];
	size_t signLen = sizeof(signText);
	hmac_sha1((const uint8_t*)key, strlen(key), (const uint8_t*)data, strlen(data), (uint8_t*)signText, &signLen);

	base64_encode(signText, signLen, signBase64);
	return string(signBase64);
}

bool SendHttpUrl(const char* strUrl)
{
	HttpRequest *request = g_HttpClient.CreateRequest();

	//printf("%s\n", strUrl);

	request->setUrl(strUrl);
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(SmsCallback);

	g_HttpClient.send(request);
	return true;
}

static const char *smsURL = "http://dysmsapi.aliyuncs.com";
static const char *accessKeyId = "LTAI4Fpegw58jBn8UbqxY7WJ";
static const char *accessSecret = "lhfrfeF69X9peBJ0jlRrDfDfmEZdzJ&";
static const char *signName = "滇中第一刀客";				//短信发送者的签名
static const char *templCode = "SMS_174272021";			//模板ID

#define TXT_TO_URL(_a) textToURLcode(_a,false).c_str()

static std::string buildSignature(const char *reqText)
{
	char urlBuf[4096];

	snprintf(urlBuf, sizeof(urlBuf), "GET&%s&%s", TXT_TO_URL("/"), TXT_TO_URL(reqText));
	std::string strUrl1 = urlBuf;

	std::string strUrl3 = replaceall(replaceall(replaceall(strUrl1, "%7E", "~"), "*", "%2A"), "+", "%20");
	std::string Sgin = sgin(accessSecret, strUrl3.c_str());
	std::string strUrl4 = replaceall(replaceall(replaceall(textToURLcode(Sgin.c_str(),false), "%7E", "~"), "*", "%2A"), "+", "%20");
	printf("signatured:%s\n", strUrl4.c_str());

	return strUrl4;
}

static char* buildSmsRequest(const char *PhoneNumbers, const char *checkcode, const char *param, char *outUrl, size_t size)
{
	time_t now = time(NULL);

	char paramText[128];

	int len = 0;
	char *p = outUrl;

	len = snprintf(p, size, "AccessKeyId=%s&Action=SendSms&Format=JSON", TXT_TO_URL(accessKeyId));
	size -= len; p += len;

	if (param && *param) {
		len = snprintf(p, size, "&OutId=%s", TXT_TO_URL(param));
		size -= len; p += len;
	}

	len = snprintf(p, size, "&PhoneNumbers=%s", TXT_TO_URL(PhoneNumbers));
	size -= len; p += len;

	len = snprintf(p, size, "&RegionId=%s", TXT_TO_URL("cn-hangzhou"));

	len = snprintf(p, size, "&SignName=%s", TXT_TO_URL(signName));
	size -= len; p += len;

	len = snprintf(p, size, "&SignatureMethod=HMAC-SHA1&SignatureNonce=%u-%u&SignatureVersion=1.0", now, rand());
	size -= len; p += len;
	
	len = snprintf(p, size, "&TemplateCode=%s", TXT_TO_URL(templCode));
	size -= len; p += len;

	snprintf(paramText, sizeof(paramText), "{\"code\":\"%s\"}", checkcode);

	len = snprintf(p, size, "&TemplateParam=%s", TXT_TO_URL(paramText));
	size -= len; p += len;


	len = snprintf(p, size, "&Timestamp=%s&Version=2017-05-25", TXT_TO_URL(getTimeStamp().c_str()));
	size -= len; p += len;

	return outUrl;

}

bool SendSms(const char *phoneNumber, const char *checkcode, const char *param)
{
	char reqForm[4096];

	buildSmsRequest(phoneNumber, checkcode, param, reqForm, sizeof(reqForm));
	std::string strSign = buildSignature(reqForm);

	printf("tobe SHA1 text: %s\n\n", strSign.c_str());

	char URL[4096];
	snprintf(URL, sizeof(URL), "%s/?Signature=%s&%s", smsURL, strSign.c_str(), reqForm);

	//send 	
	printf("url:%s\n", URL);
	
	if (SendHttpUrl(URL))
		printf("send Sms success!\n");
	else
		printf("send Sms failed!\n");
	
	return true;
		
}


//base 64

const char _nd_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define END_OF_BASE64_ENCODED_DATA           ('=')
#define BASE64_END_OF_BUFFER                 (0xFD)
#define BASE64_IGNORABLE_CHARACTER           (0xFE)
#define BASE64_UNKNOWN_VALUE                 (0xFF)
#define BASE64_NUMBER_OF_CHARACTERS_PER_LINE (72)

int base64_encode(const char * source, int len, char * destination_string)
{

	int loop_index = 0;
	int number_of_bytes_to_encode = len;

	unsigned char byte_to_add = 0;
	unsigned char byte_1 = 0;
	unsigned char byte_2 = 0;
	unsigned char byte_3 = 0;

	unsigned int number_of_bytes_encoded = (unsigned int)((double)number_of_bytes_to_encode / (double) 0.75) + 1;
	char * destination;

	number_of_bytes_encoded += (unsigned int)(((number_of_bytes_encoded / BASE64_NUMBER_OF_CHARACTERS_PER_LINE) + 1) * 2);

	destination = destination_string;

	number_of_bytes_encoded = 0;

	while (loop_index < number_of_bytes_to_encode) {
		// Output the first byte
		byte_1 = source[loop_index];
		byte_to_add = _nd_alphabet[(byte_1 >> 2)];

		destination[number_of_bytes_encoded] = byte_to_add;
		number_of_bytes_encoded++;

		loop_index++;

		if (loop_index >= number_of_bytes_to_encode) {
			// We're at the end of the data to encode
			byte_2 = 0;
			byte_to_add = _nd_alphabet[(((byte_1 & 0x03) << 4) | ((byte_2 & 0xF0) >> 4))];

			destination[number_of_bytes_encoded] = byte_to_add;
			number_of_bytes_encoded++;

			destination[number_of_bytes_encoded] = END_OF_BASE64_ENCODED_DATA;
			number_of_bytes_encoded++;

			destination[number_of_bytes_encoded] = END_OF_BASE64_ENCODED_DATA;

			destination[number_of_bytes_encoded + 1] = 0;

			return 0;
		}
		else {
			byte_2 = source[loop_index];
		}

		byte_to_add = _nd_alphabet[(((byte_1 & 0x03) << 4) | ((byte_2 & 0xF0) >> 4))];

		destination[number_of_bytes_encoded] = byte_to_add;
		number_of_bytes_encoded++;

		loop_index++;

		if (loop_index >= number_of_bytes_to_encode)
		{
			// We ran out of bytes, we need to add the last half of byte_2 and pad
			byte_3 = 0;

			byte_to_add = _nd_alphabet[(((byte_2 & 0x0F) << 2) | ((byte_3 & 0xC0) >> 6))];

			destination[number_of_bytes_encoded] = byte_to_add;
			number_of_bytes_encoded++;

			destination[number_of_bytes_encoded] = END_OF_BASE64_ENCODED_DATA;

			destination[number_of_bytes_encoded + 1] = 0;

			return 0;
		}
		else
		{
			byte_3 = source[loop_index];
		}

		loop_index++;

		byte_to_add = _nd_alphabet[(((byte_2 & 0x0F) << 2) | ((byte_3 & 0xC0) >> 6))];

		destination[number_of_bytes_encoded] = byte_to_add;
		number_of_bytes_encoded++;

		byte_to_add = _nd_alphabet[(byte_3 & 0x3F)];

		destination[number_of_bytes_encoded] = byte_to_add;
		number_of_bytes_encoded++;

		if ((number_of_bytes_encoded % BASE64_NUMBER_OF_CHARACTERS_PER_LINE) == 0)
		{
			destination[number_of_bytes_encoded] = 13;		//return
			number_of_bytes_encoded++;

			destination[number_of_bytes_encoded] = 10;		//new line
			number_of_bytes_encoded++;
		}
	}

	destination[number_of_bytes_encoded] = END_OF_BASE64_ENCODED_DATA;

	destination[number_of_bytes_encoded + 1] = 0;

	return 0;
}


int base64_decode(unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len)
{
	static char inalphabet[256], decoder[256];
	int i, bits, c = 0, char_count, errors = 0;
	unsigned int input_idx = 0;
	unsigned int output_idx = 0;

	for (i = (sizeof _nd_alphabet) - 1; i >= 0; i--) {
		inalphabet[(int)_nd_alphabet[i]] = 1;
		decoder[(int)_nd_alphabet[i]] = i;
	}

	char_count = 0;
	bits = 0;
	for (input_idx = 0; input_idx < input_len; input_idx++) {
		c = input[input_idx];
		if (c == '=')
			break;
		if (c > 255 || !inalphabet[c])
			continue;
		bits += decoder[c];
		char_count++;
		if (char_count == 4) {
			output[output_idx++] = (bits >> 16);
			output[output_idx++] = ((bits >> 8) & 0xff);
			output[output_idx++] = (bits & 0xff);
			bits = 0;
			char_count = 0;
		}
		else {
			bits <<= 6;
		}
	}

	if (c == '=') {
		switch (char_count) {
		case 1:
			errors++;
			break;
		case 2:
			output[output_idx++] = (bits >> 10);
			break;
		case 3:
			output[output_idx++] = (bits >> 16);
			output[output_idx++] = ((bits >> 8) & 0xff);
			break;
		}
	}
	else if (input_idx < input_len) {
		if (char_count) {
			errors++;
		}
	}

	if (output_len) {
		*output_len = output_idx;
	}

	return errors ? -1 : 0;
}
