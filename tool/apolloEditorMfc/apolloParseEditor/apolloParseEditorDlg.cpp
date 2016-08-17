
// apolloParseEditorDlg.cpp : 实现文件
//


#include "stdafx.h"
#include "apolloParseEditor.h"
#include "apolloParseEditorDlg.h"
#include "afxdialogex.h"
#include "NewfileDlg.h"

#include "XmlCfg.h"
//#include "ND_File.h"
#include "apolloParseEditor.h"
#include "nd_common/nd_common.h"
#include "nd_net/byte_order.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logicEngineRoot.h"
#include "attribute/roleattr_help.h"
#include "attribute/stat_machine.h"
#include "logic_parser/dbl_mgr.h"
#include "ndapplib/ndsingleton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static ndxml_root m_editor_setting;
static ndxml_root m_io_setting;


void _xml_errorout(const char *errdesc)
{
	CString str1;
	str1.Format("parse xml error %s", errdesc);
	AfxMessageBox((LPCTSTR)str1);
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CapolloParseEditorDlg 对话框



CapolloParseEditorDlg::CapolloParseEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CapolloParseEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ndxml_initroot(&m_editor_setting);
	ndxml_initroot(&m_io_setting);
	m_newLine = true;
}

void CapolloParseEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CapolloParseEditorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BT_FILE, &CapolloParseEditorDlg::OnBnClickedBtFile)
	ON_BN_CLICKED(IDC_BT_EDIT, &CapolloParseEditorDlg::OnBnClickedBtEdit)
	ON_BN_CLICKED(IDC_BT_COMPILE, &CapolloParseEditorDlg::OnBnClickedBtCompile)
	//ON_BN_CLICKED(IDC_BT_OUTT_FILE, &CapolloParseEditorDlg::OnBnClickedBtOuttFile)
	ON_BN_CLICKED(IDC_BT_LOADER_TEST, &CapolloParseEditorDlg::OnBnClickedBtLoaderTest)
	//ON_BN_CLICKED(IDC_BT_CLEAR_LOG, &CapolloParseEditorDlg::OnBnClickedBtClearLog)
	ON_BN_CLICKED(IDC_BT_EXP_DATA, &CapolloParseEditorDlg::OnBnClickedBtExpData)
	ON_BN_CLICKED(IDC_BT_SETTING, &CapolloParseEditorDlg::OnBnClickedBtSetting)
	ON_WM_DESTROY()
//	ON_BN_CLICKED(IDC_BT_PACKAGE_DATA, &CapolloParseEditorDlg::OnBnClickedBtPackageData)
ON_BN_CLICKED(IDC_BT_ALL_EXPORT, &CapolloParseEditorDlg::OnBnClickedBtAllExport)
ON_BN_CLICKED(IDC_BT_RUN_GM, &CapolloParseEditorDlg::OnBnClickedBtRunGm)
END_MESSAGE_MAP()


// CapolloParseEditorDlg 消息处理程序

BOOL CapolloParseEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	if (0 != ndxml_load_ex(CONFIG_FILE_PATH, &m_editor_setting,"gbk")){
		AfxMessageBox("加载配置文件错误");
		return false;
	}
	if (0 != ndxml_load_ex(CONFIG_IO_SETTING, &m_io_setting, "gbk")){
		AfxMessageBox("加载配置文件错误");
		return false;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CapolloParseEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CapolloParseEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CapolloParseEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CapolloParseEditorDlg::LogText(const char* text)
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_OUTPUT);
	if (!pList)	{
		return;
	}
	char buf[4096];
	char *p = (char*)text;
	do	{
		buf[0] = 0;
		p = (char*) ndstr_nstr_end(p, buf, '\n', sizeof(buf));
		if (buf[0]){
			if (m_newLine)	{
				pList->AddString((LPCTSTR)text);
			}
			else {
				int total = pList->GetCount();
				if (total > 0)	{
					CString str1;
					int index = total - 1;
					pList->GetText(index, str1);
					pList->DeleteString(index);
					str1 += (LPCTSTR)buf;
					pList->AddString((LPCTSTR)str1);
				}
			}
		}

		if (p && *p == '\n') {
			++p;
			m_newLine = true;
		}
		else {
			m_newLine = false;
		}

	} while (p && *p);
	
	pList->SetCurSel(pList->GetCount() - 1);
}

void CapolloParseEditorDlg::clearLog()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_OUTPUT);
	if (pList) {
		pList->ResetContent();
		UpdateData(FALSE);
		pList->SetHorizontalExtent(4096);
	}
	m_newLine = true;

}



#define _LOAD_XML(_xml_name, _filename,_encode,_isexit) \
	ndxml_root _xml_name;				\
	ndxml_initroot(&_xml_name);			\
	if (ndxml_load_ex((char*)_filename, &_xml_name,_encode)) {	\
		CString tipText  ;		\
		tipText.Format("open file %s error", _filename);	\
		AfxMessageBox((LPCTSTR)tipText);	\
		if(_isexit) return;							\
	}

//编辑逻辑脚本
void CapolloParseEditorDlg::OnBnClickedBtEdit()
{
	CNewfileDlg dlg;
	const char *scriptRoot = _getFromIocfg("script_root");
	const char *newTmplFile = _getFromIocfg("new_template");

	dlg.InitFileRoot(scriptRoot, newTmplFile);
	if (IDOK==dlg.DoModal() ){
		const char *file = dlg.getSelectFile();
		if (file && *file)	{
			_beginEdit(file);
		}
		else {
			AfxMessageBox(_T("请选择要编辑的文件!"));
		}
	}
}
//编译逻辑脚本
void CapolloParseEditorDlg::OnBnClickedBtCompile()
{
	//load config file 
	clearLog();

	if (false == compile()){
		AfxMessageBox("编译脚本错误");
		return;
	}
	LogText("==========编译脚本成功===========\n");

}

//测试所有数据导出加载和相关功能
void CapolloParseEditorDlg::OnBnClickedBtLoaderTest()
{
	//加载并测试数据
	clearLog();
	if (false == runTest()){
		AfxMessageBox("测试失败!");
	}	
}

//导出策划数据
void CapolloParseEditorDlg::OnBnClickedBtExpData()
{
	clearLog();
	
	if (false == expExcel()) {
		AfxMessageBox("导出excel错误");
		return;
	}
	LogText("==========EXCEL打包成功===========\n");

}

void CapolloParseEditorDlg::OnBnClickedBtSetting()
{
	CXmlCfg xmlDlg;
	xmlDlg.SetXML(&m_editor_setting, &m_io_setting);

	if (IDOK == xmlDlg.DoModal()) {
		nd_chdir(nd_getcwd());
		ndxml_save_encode(&m_io_setting, CONFIG_IO_SETTING, E_SRC_CODE_GBK, E_SRC_CODE_UTF_8);
	}
}


void CapolloParseEditorDlg::OnBnClickedBtAllExport()
{
	clearLog();
	if (false == compile()){
		AfxMessageBox("编译脚本错误");
		return ;
	}
	if (false == expExcel()) {
		AfxMessageBox("导出excel错误");
		return ;
	}
	LogText("==========一键打包成功===========\n");
}

void CapolloParseEditorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	ndxml_destroy(&m_editor_setting);
	ndxml_destroy(&m_io_setting);
}
#include "gmToolDlg.h"

void CapolloParseEditorDlg::OnBnClickedBtRunGm()
{

	const char*filename = _getFromIocfg("gm_send_msg");
	const char *dataTypeFile = _getFromIocfg("net_data_def");
	const char *client_script = _getFromIocfg("connect_script");
	const char *message_file = _getFromIocfg("net_protocol"); 

	_LOAD_XML(xmlSend, filename, "gbk", 0);

	gmToolDlg dlg;

	dlg.m_editor_setting = &m_editor_setting;
	dlg.m_gmCfg = &xmlSend;
	if (!dlg.LoadDataDef(dataTypeFile, client_script, message_file))	{
		AfxMessageBox("load message data type error");
	}
	dlg.DoModal();

	ndxml_save_encode(&xmlSend, filename, E_SRC_CODE_GBK, E_SRC_CODE_UTF_8);

	ndxml_destroy(&xmlSend);
}

bool CapolloParseEditorDlg::compile()
{
	const char *script_root = _getFromIocfg("script_root");

	ndxml_root xmlEntry;
	ndxml_initroot(&xmlEntry);
	if (-1==ndxml_load_ex(script_root, &xmlEntry,"gbk")) {
		return false;
	}

	ndxml_root *xml = ndxml_getnode(&xmlEntry, "script_file_manager");
	if (!xml){
		return false;
	}
	bool ret = true;
	int num = ndxml_num(xml);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node)
			continue;
		if (!compileScript(ndxml_getval(node))) {
			ret = false;
			break;
		}
	}

	ndxml_destroy(&xmlEntry);
	return ret;
}

bool CapolloParseEditorDlg::compileScript(const char *scriptFile)
{
	ndxml_root xmlScript;
	ndxml_initroot(&xmlScript);
	if (-1 == ndxml_load_ex(scriptFile, &xmlScript, "gbk")) {
		return false;
	}
	const char*inFile = scriptFile;

	std::string outFile = getScriptSetting(&xmlScript, "out_file");
	int outEncode = getScriptExpEncodeType(&xmlScript);
	bool withDebug = getScriptExpDebugInfo(&xmlScript);
	ndxml_destroy(&xmlScript);


	int orderType = ND_L_ENDIAN;
	const char *orderName = _getFromIocfg("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	LogicCompiler lgcompile;

	if (!lgcompile.setConfigFile(CONFIG_FILE_PATH)) {
		return false;
	}
	if (!lgcompile.compileXml(inFile, outFile.c_str(), outEncode, withDebug, orderType)) {

		const char *pFunc = lgcompile.m_cur_function.c_str();
		const char *pStep = lgcompile.m_cur_step.c_str();
		char func_name[256];
		char step_name[256];
		if (outEncode == E_SRC_CODE_UTF_8) {
			pFunc = nd_utf8_to_gbk(pFunc, func_name, sizeof(func_name));
			pStep = nd_utf8_to_gbk(pStep, step_name, sizeof(step_name));
		}
		nd_logerror("compile error file %s, function %s, step %s , stepindex %d\n",
			lgcompile.m_cur_file.c_str(), pFunc, pStep, lgcompile.m_cur_node_index);

		return false ;
	}

	LogText("!!!!!!!!!!编译脚本成功!!!!!!!!!!!\n运行测试程序...\n");

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);

	scriptRoot->setPrint(out_print, NULL);
	scriptRoot->getGlobalParser().setSimulate(true);
	if (0 != scriptRoot->LoadScript(outFile.c_str())){
		LogText("加载脚本出错！\n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogText("开始运行脚本...\n");
	if (0 != scriptRoot->test()){
		LogText("运行脚本出错！\n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogicEngineRoot::destroy_Instant();
	LogText("!!!!!!!!!!!!!!!!!!!脚本编译运行成功!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return true;
}

bool CapolloParseEditorDlg::expExcel()
{
	char path[1024];

	LogText("==============开始导出excel===============\n");

	const char *exp_cmd = _getFromIocfg("game_data_export_cmd");
	const char *excel_path = _getFromIocfg("excel_data_in_path");
	const char *text_path = _getFromIocfg("text_data_out_path");
	const char *package_file = _getFromIocfg("game_data_package_file");
	const char *excel_list = _getFromIocfg("game_data_listfile");

	if (!exp_cmd || !excel_path || !text_path || !package_file ){
		LogText("导出excel错误:配置文件错误\n");
		return false ;
	}
	
	const char *cur_dir = nd_getcwd();
	const char *encodeName = getGameDateEncodeType();

	snprintf(path, sizeof(path), "cmd.exe /c %s/%s %s %s %s %s ", cur_dir, exp_cmd, excel_list, excel_path, text_path,  encodeName);
	int ret = system(path);

	if (0 != ret)	{
		nd_logerror("导出策划数据失败，请手动导出\n");
		return false;
	}

	LogText("excel 导出完成!!!!!!!\n==================开始打包======================\n");

	//get byte order  
	int orderType = ND_L_ENDIAN;
	const char *orderName = _getFromIocfg("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	DBLDatabase dbtmp;
	if (0 != dbtmp.LoadFromText(text_path, excel_list, encodeName, encodeName)) {
		nd_logerror("打包数据错误:不能从txt文件中读取数据\n");
		return false;
	}
	if (0 == dbtmp.Dump(package_file, "gamedatadb", orderType)) {
		nd_logmsg("package game data SUCCESS!!\n");
		LogText("==========导出策划数据成功===========\n");
	}
	else{
		nd_logerror("excel导出二进制文件错误\n");
	}
	
	//before run test need load dbl 
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		if (0 != pdbl->LoadBinStream(package_file)) {
			nd_logmsg("加载二进制策划数据错误!\n");
			dbtmp.Destroy();
			DBLDatabase::destroy_Instant();
			return false;
		}
	}
	if (!(*pdbl == dbtmp)){
		nd_logmsg("打包数据和原始数据不相等!");
		dbtmp.Destroy();
		DBLDatabase::destroy_Instant();
		return false ;
	}
	
	dbtmp.Destroy();

	std::string strOutTextPath = text_path;
	strOutTextPath  += "/test_outputData";
	nd_mkdir(strOutTextPath.c_str());
	if (0 != pdbl->TestOutput(strOutTextPath.c_str())) {
		nd_logmsg("从data输出text文件失败!");
		DBLDatabase::destroy_Instant();
		return false; 

	}

	DBLDatabase::destroy_Instant();
	return true;
}

bool CapolloParseEditorDlg::runTest()
{
	if (expExcel()==false ){
		return false;
	}
	LogText("加载策划数据完成\n加载脚本.....\n");

	if (compile() == false ) {
		return false;
	}

	bool ret = true;

	const char *package_file = _getFromIocfg("game_data_package_file");

	if (0 != DBLDatabase::get_Instant()->LoadBinStream(package_file)) {
		LogText("加载二进制策划数据错误!\n");
		return false;;
	}

	do 	{
		const char*attr_table = _getFromIocfg("role_attr_table");
		NDSingleton<RoleAttrHelper>::Destroy();
		//StatMachine::Destroy();

		if (0 != NDSingleton<RoleAttrHelper>::Get()->Load(attr_table,"role_level_attr.xlsx")) {
			LogText("角色属性加载失败\n");
			ret = false;
			goto ERROR_EXIT;
		}
// 		if (0 != StatMachine::load(forbid_table, state_table, operate_table)) {
// 			LogText("状态表加载失败\n");
// 			ret = false;
// 			goto ERROR_EXIT;
// 		}
		NDSingleton<RoleAttrHelper>::Destroy();
		//StatMachine::Destroy();

		LogText("状态属性加载测试成功\n");

	} while (0);
	//test role attribute 

	LogText("！！！！！！！测试成功！！！！！！！！！！\n");

	
ERROR_EXIT:

	DBLDatabase::destroy_Instant();
	return ret;

}


void CapolloParseEditorDlg::_beginEdit(const char *script_file)
{
	const char *filename;
	clearLog();

	LogText("编辑中.....\n");
	//filename = getGameScript();

	filename = script_file;
	_LOAD_XML(xml_script, filename, "gbk", 1);

	filename = getScriptSetting(&xml_script, "c_func_api");
	_LOAD_XML(xml_cpp_func, filename, "gbk", 0);

	filename = getScriptSetting(&xml_script, "events_id");
	_LOAD_XML(xml_events_id, filename, "gbk", 0);


	filename = getNetProtocol();
	_LOAD_XML(xml_net_protocol, filename, "gbk", 0);

	CXmlCfg xmlDlg;
	xmlDlg.SetXML(&m_editor_setting, &xml_script);

	//load function list 
	ndxml *funcroot = ndxml_getnode(&xml_net_protocol, "MessageDefine");
	if (funcroot) {
		char buf[256];
		text_list_t messageList;
		for (int i = 0; i < ndxml_num(funcroot); i++){
			ndxml *fnode = ndxml_getnodei(funcroot, i);
			const char *pDispname = ndxml_getattr_val(fnode, "comment");
			const char *pRealVal = ndxml_getattr_val(fnode, "id");
			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
			messageList.push_back(CString(p));
		}

		xmlDlg.addDisplayNameList("msg_list", messageList);
	}
	xmlDlg.loadUserdefDisplayList(xml_cpp_func, LOGIC_FUNCTION_LIST_NAME);
	xmlDlg.loadUserdefDisplayList(xml_events_id, LOGIC_EVENT_LIST_NAME);


	const char *package_file = _getFromIocfg("game_data_package_file");
	DBLDatabase::get_Instant()->LoadBinStream(package_file);

	if (IDOK == xmlDlg.DoModal()) {
		nd_chdir(nd_getcwd());

		ndxml_save_encode(&xml_script, script_file, E_SRC_CODE_GBK, E_SRC_CODE_UTF_8);
		LogText("编辑完成\n");
	}
	else {
		LogText("没有修改\n");
	}
	ndxml_destroy(&xml_script);
	ndxml_destroy(&xml_cpp_func);
	ndxml_destroy(&xml_net_protocol);
	ndxml_destroy(&xml_events_id);
	DBLDatabase::destroy_Instant();
}


const char *CapolloParseEditorDlg::getScriptSetting(ndxml *scriptXml, const char *settingName)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, settingName);
		if (node){
			return ndxml_getval(node);
		}
	}
	return NULL;
}

int CapolloParseEditorDlg::getScriptExpEncodeType(ndxml *scriptXml)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, "script_out_encode");
		if (node){
			return ndxml_getval_int(node);
		}
	}
	return 0;
}

bool CapolloParseEditorDlg::getScriptExpDebugInfo(ndxml *scriptXml)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, "script_with_debug");
		if (node){
			return ndxml_getval_int(node) ? true : false;
		}
	}
	return false;
}

const char *CapolloParseEditorDlg::getNetProtocol()
{
	return _getFromIocfg("net_protocol");
}

const char *CapolloParseEditorDlg::getGameDateEncodeType()
{
	int codeType = atoi(_getFromIocfg("game_data_out_encode"));
	return nd_get_encode_name(codeType);
}

const char *CapolloParseEditorDlg::_getFromIocfg(const char *cfgName)
{
	ndxml *node = ndxml_getnode(&m_io_setting, "setting_config");
	if (node){
		ndxml *cfgxml = ndxml_getnode(node, cfgName);
		if (cfgxml)	{
			return ndxml_getval(cfgxml);
		}
	}
	return NULL;
}