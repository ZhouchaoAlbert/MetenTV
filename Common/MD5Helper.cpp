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
		{//���ļ�ʧ��
			bResult = FALSE;
			break;
		}
		fseek(pFile, 0, SEEK_END);
		MD5MSG md5msg;
		if ((md5msg.len = ftell(pFile)) == -1)//ftell��������long,���Ϊ2GB,��������-1
		{
			fclose(pFile);
			bResult = FALSE;
			break;
		}

		rewind(pFile); //�ļ�ָ�븴λ���ļ�ͷ
		md5msg.A = 0x67452301;
		md5msg.B = 0xefcdab89;
		md5msg.C = 0x98badcfe;
		md5msg.D = 0x10325476; //��ʼ�����ӱ���
		md5msg.flen[1] = md5msg.len / 0x20000000;     //md5msg.flen��λ��bit
		md5msg.flen[0] = (md5msg.len % 0x20000000) * 8;
		memset(md5msg.x, 0, 64);   //��ʼ��md5msg.x����Ϊ0
		fread(&md5msg.x, 4, 16, pFile); //��4�ֽ�Ϊһ��,��ȡ16������
		for (md5msg.i = 0; md5msg.i < md5msg.len / 64; md5msg.i++)	    //ѭ������ֱ���ļ�����
		{
			md5(md5msg);
			memset(md5msg.x, 0, 64);
			fread(&md5msg.x, 4, 16, pFile);
		}
		((char*)md5msg.x)[md5msg.len % 64] = (char)128; //�ļ�������1,��0����,128�����Ƽ�10000000
		if (md5msg.len % 64>55)
		{
			md5(md5msg);
			memset(md5msg.x, 0, 64);
		}
		memcpy(md5msg.x + 14, md5msg.flen, 8);    //�ļ�ĩβ����ԭ�ļ���bit����
		md5(md5msg);
		fclose(pFile);
		strMD5Value.Format(_T("%08x%08x%08x%08x"), PP(md5msg.A), PP(md5msg.B), PP(md5msg.C), PP(md5msg.D));	//�ߵ�λ�淴���
	} while (false);
	return bResult;
}

//������ת32λmd5�ַ���
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