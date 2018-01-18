// dllmain.h : 模块类的声明。

class CCommonModule : public ATL::CAtlDllModuleT< CCommonModule >
{
public :
	DECLARE_LIBID(LIBID_CommonLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMMON, "{AD7053EE-A6E5-4B9D-A668-C9686D831602}")
};

extern class CCommonModule _AtlModule;
