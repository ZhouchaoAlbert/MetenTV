#include "StdAfx.h"
#include "MD5Helper.h"
#include "md5.h"
#include "UtilPath.h"
#include "string.h"
#include <share.h>
#include "hash_md5.h"

BOOL Util::MD5::GetFileMD5Value(const CString &strFile, CString &strMD5Value)
{
	BOOL bResult = TRUE;
	do 
	{
		if (FALSE == Util::Path::IsFileExist(strFile))
		{
			bResult = FALSE;
			break;
		}

		FILE *pFile = _tfsopen(strFile, L"rb", SH_DENYNO);
		if (NULL == pFile)
		{//打开文件失败
			bResult = FALSE;
			break;
		}
		fseek(pFile, 0, SEEK_END);
		MD5MSG md5msg;
		if ((md5msg.len = ftell(pFile)) == -1)//ftell函数返回long,最大为2GB,超出返回-1
		{
			fclose(pFile);
			bResult = FALSE;
			break;
		}

		rewind(pFile); //文件指针复位到文件头
		md5msg.A = 0x67452301;
		md5msg.B = 0xefcdab89;
		md5msg.C = 0x98badcfe;
		md5msg.D = 0x10325476; //初始化链接变量
		md5msg.flen[1] = md5msg.len / 0x20000000;     //md5msg.flen单位是bit
		md5msg.flen[0] = (md5msg.len % 0x20000000) * 8;
		memset(md5msg.x, 0, 64);   //初始化md5msg.x数组为0
		fread(&md5msg.x, 4, 16, pFile); //以4字节为一组,读取16组数据
		for (md5msg.i = 0; md5msg.i < md5msg.len / 64; md5msg.i++)	    //循环运算直至文件结束
		{
			md5(md5msg);
			memset(md5msg.x, 0, 64);
			fread(&md5msg.x, 4, 16, pFile);
		}
		((char*)md5msg.x)[md5msg.len % 64] = (char)128; //文件结束补1,补0操作,128二进制即10000000
		if (md5msg.len % 64>55)
		{
			md5(md5msg);
			memset(md5msg.x, 0, 64);
		}
		memcpy(md5msg.x + 14, md5msg.flen, 8);    //文件末尾加入原文件的bit长度
		md5(md5msg);
		fclose(pFile);
		strMD5Value.Format(_T("%08x%08x%08x%08x"), PP(md5msg.A), PP(md5msg.B), PP(md5msg.C), PP(md5msg.D));	//高低位逆反输出
	} while (false);
	return bResult;
}

//缓存区转32位md5字符串
CString Util::MD5::BufferToMD5(const BYTE * buffer, UINT nLen)
{
	BYTE szMD5[32] = { 0 };
	Md5HashBuffer(szMD5, (BYTE*)buffer, nLen);
	CString strMD5;
	for (int i = 0; i < 16; i++)
	{
		strMD5.AppendFormat(L"%02X", *(szMD5 + i));
	}
	return strMD5;
}

BOOL Util::MD5::BufferToMD5(const BYTE * buffer, UINT nLen, UINT8 *buf, UINT16 buflen)
{
	CString strMd5 = BufferToMD5(buffer, nLen);
	memcpy_s(buf, buflen, strMd5.GetString(), strMd5.GetLength() * sizeof(TCHAR));
	return TRUE;
}

CString Util::Storage::CreateDBPassWord(LPCTSTR pNormalStr, LPCTSTR pAttchStr /* = ATTACHPASSWORDSTRING */)
{
	CString strPassWord;
	strPassWord.Format(_T("%s%s"), pNormalStr, pAttchStr);
	CW2A pszut8(strPassWord, CP_UTF8);
	std::string buffer = pszut8;
	return Util::MD5::BufferToMD5((BYTE *)buffer.c_str(), buffer.length());
}