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

	USES_CONVERSION;
	CStringA strCmdA = T2A(szCmd);

	ATL::CStringA strJsCode = "miaCPP2Js(\"";
	strJsCode.Append(strCmdA);
	strJsCode.Append("\",");
	strJsCode.Append(szJsonData);
	strJsCode.Append(")");

	//strJsCode = "miaCPP2Js(\"fileList\", \"{\"filepath\":\"C://Users//18008//Documents//MelianTV//123456789.mp4\", \"id\" : \"123456789\"}\")";
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

		USES_CONVERSION;
		val["id"] = T2A(it->first);
		val["filepath"] = T2A(it->second);

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

	USES_CONVERSION;
	jop.add("id", T2A(id));
	jop.add("filepath", T2A(filepath));

	json_text = writer.write(val).c_str();

//	json_text.Replace("\n", "");

	return TRUE;
}

BOOL Encode_Cpp2Js_MessageBox(const CString& text, CStringA& json_text)
{
	Json::FastWriter writer;

	Json::Value val;
	json_op jop(val);

	USES_CONVERSION;
	jop.add("text", T2A(text));

	json_text = writer.write(val).c_str();

	return TRUE;
}