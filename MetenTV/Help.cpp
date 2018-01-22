#include "stdafx.h"
#include "Help.h"

#include<atlstr.h>
#include "json.h"
#include "json_op.h"
#include "UtilString.h"

static ISafeChannel* s_pISafeChannel = NULL;

ISafeChannel*	GetSafeChannel()
{
	return s_pISafeChannel;
}

void	SetSafeChannel(ISafeChannel* channel)
{
	s_pISafeChannel = channel;
}

//执行JS 把信息投递到渲染进程处理
void ExecJavaScript(CefRefPtr<CefBrowser> browser, LPCTSTR szCmd, LPCSTR szJsonData)
{
	if (!browser.get())
		return;
	CefRefPtr<CefFrame> pFrame = browser->GetMainFrame();
	if (!pFrame.get())
		return;
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ipc_channel_cpp2js");
	if (!msg.get())
		return;
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	if (!args.get())
		return;

	CStringA strCmdA;
	Util::String::W_2_Utf8(szCmd, strCmdA);

	ATL::CStringA strJsCode = "miaCPP2Js(\"";
	strJsCode.Append(strCmdA);
	strJsCode.Append("\",");
	strJsCode.Append(szJsonData);
	strJsCode.Append(")");

	//strJsCode = "miaCPP2Js(\"fileList\", \"{\"filepath\":\"C://Users//18008//Documents//MetenTV//123456789.mp4\", \"id\" : \"123456789\"}\")";
	//strJsCode = "miaCPP2Js(\"fileList\", {})";

	args->SetString(0, CefString(strJsCode));
	CefString strURL = pFrame->GetURL();
	//pFrame->ExecuteJavaScript(CefString(pszBuffer), pFrame->GetURL(), 0);
	browser->SendProcessMessage(PID_RENDERER, msg);
}

CString CombinePath(std::vector<CString>& aFileNames)
{
	int iUpperFolder = 0;
	for (int i = (int)aFileNames.size() - 1; i >= 0; i--)
	{
		if (!aFileNames[i].Compare(_T("..")))
		{
			aFileNames[i] = _T("");
			iUpperFolder++;
		}
		else
		{
			if (iUpperFolder)
			{
				aFileNames[i] = _T("");
				iUpperFolder--;
			}
		}
	}
			
	CString strPath;
	for (int i = 0; i < (int)aFileNames.size(); i++)
	{
		if (aFileNames[i] != _T(""))
		{
			strPath += aFileNames[i];
			if (i + 1 != (int)aFileNames.size())
			{
				strPath += _T("\\");
			}
		}
	}

	return strPath;
}

////////////////////////////////////////////
BOOL Decode_Js2Cpp_SyncList(const CStringA& json_text, map<CString, CString>& list)
{
	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(json_text.GetString(), value))
	{
		return FALSE;
	}

	for (UINT32 uIndex = 0; uIndex < value.size(); uIndex++)
	{
		CString id;
		CString url;

		if (value[uIndex]["id"].isString())
		{
			Util::String::Utf8_2_W(value[uIndex]["id"].asCString(), id);
		}
		if (value[uIndex]["url"].isString())
		{
			Util::String::Utf8_2_W(value[uIndex]["url"].asCString(), url);
		}

		if (!id.IsEmpty() && !url.IsEmpty())
		{
			list[id] = url;
		}
	}

	return TRUE;
}

BOOL Decode_Js2Cpp_Upgrade(const CStringA& strContent, CString& ver, CString& url, CString& md5)
{
	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(strContent.GetString(), value))
	{
		return FALSE;
	}

	if (!value["ver"].isString())
	{
		return FALSE;
	}
	if (!value["url"].isString())
	{
		return FALSE;
	}
	if (!value["md5"].isString())
	{
		return FALSE;
	}

	Util::String::Utf8_2_W(value["ver"].asCString(), ver);
	Util::String::Utf8_2_W(value["url"].asCString(), url);
	Util::String::Utf8_2_W(value["md5"].asCString(), md5);

	return TRUE;
}


BOOL Encode_Cpp2Js_FileList(const map<CString, CString>& list, CStringA& json_text)
{
	Json::FastWriter writer;

	Json::Value arVal;
	for (map<CString, CString>::const_iterator it = list.begin(); it != list.end(); it++)
	{
		Json::Value val;

		CStringA strTemp;
		
		Util::String::W_2_Utf8(it->first, strTemp);
		val["id"] = (LPCSTR)strTemp;
		Util::String::W_2_Utf8(it->second, strTemp);		
		val["filepath"] = (LPCSTR)strTemp;

		arVal.append(val);
	}	
	json_text = writer.write(arVal).c_str();

	return TRUE;
}

BOOL Encode_Cpp2Js_DownComplete(const CString& id, const CString& filepath, CStringA& json_text)
{
	Json::FastWriter writer;

	Json::Value val;
	json_op jop(val);

	CStringA strTemp;
		
	Util::String::W_2_Utf8(id, strTemp);
	jop.add("id", (LPCSTR)strTemp);
	Util::String::W_2_Utf8(filepath, strTemp);		
	jop.add("filepath", (LPCSTR)strTemp);

	json_text = writer.write(val).c_str();

//	json_text.Replace("\n", "");

	return TRUE;
}

BOOL Encode_Cpp2Js_MessageBox(const CString& text, CStringA& json_text)
{
	Json::FastWriter writer;

	Json::Value val;
	json_op jop(val);

	CStringA strTemp;
		
	Util::String::W_2_Utf8(text, strTemp);
	jop.add("text", (LPCSTR)strTemp);

	json_text = writer.write(val).c_str();

	return TRUE;
}

BOOL Encode_Cpp2Js_ReportVersion(const UINT32& local_ver, CStringA& json_text)
{
	Json::FastWriter writer;

	Json::Value val;
	json_op jop(val);

	CString ver_text;
	ver_text.Format(_T("%u"), local_ver);
	
	CStringA strTemp;		
	Util::String::W_2_Utf8(ver_text, strTemp);
	jop.add("ver", (LPCSTR)strTemp);

	json_text = writer.write(val).c_str();

	return TRUE;
}