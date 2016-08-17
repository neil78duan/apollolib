
// apolloParseEditorDlg.cpp : ʵ���ļ�
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CapolloParseEditorDlg �Ի���



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


// CapolloParseEditorDlg ��Ϣ�������

BOOL CapolloParseEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	if (0 != ndxml_load_ex(CONFIG_FILE_PATH, &m_editor_setting,"gbk")){
		AfxMessageBox("���������ļ�����");
		return false;
	}
	if (0 != ndxml_load_ex(CONFIG_IO_SETTING, &m_io_setting, "gbk")){
		AfxMessageBox("���������ļ�����");
		return false;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CapolloParseEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

//�༭�߼��ű�
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
			AfxMessageBox(_T("��ѡ��Ҫ�༭���ļ�!"));
		}
	}
}
//�����߼��ű�
void CapolloParseEditorDlg::OnBnClickedBtCompile()
{
	//load config file 
	clearLog();

	if (false == compile()){
		AfxMessageBox("����ű�����");
		return;
	}
	LogText("==========����ű��ɹ�===========\n");

}

//�����������ݵ������غ���ع���
void CapolloParseEditorDlg::OnBnClickedBtLoaderTest()
{
	//���ز���������
	clearLog();
	if (false == runTest()){
		AfxMessageBox("����ʧ��!");
	}	
}

//�����߻�����
void CapolloParseEditorDlg::OnBnClickedBtExpData()
{
	clearLog();
	
	if (false == expExcel()) {
		AfxMessageBox("����excel����");
		return;
	}
	LogText("==========EXCEL����ɹ�===========\n");

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
		AfxMessageBox("����ű�����");
		return ;
	}
	if (false == expExcel()) {
		AfxMessageBox("����excel����");
		return ;
	}
	LogText("==========һ������ɹ�===========\n");
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

	LogText("!!!!!!!!!!����ű��ɹ�!!!!!!!!!!!\n���в��Գ���...\n");

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);

	scriptRoot->setPrint(out_print, NULL);
	scriptRoot->getGlobalParser().setSimulate(true);
	if (0 != scriptRoot->LoadScript(outFile.c_str())){
		LogText("���ؽű�����\n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogText("��ʼ���нű�...\n");
	if (0 != scriptRoot->test()){
		LogText("���нű�����\n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogicEngineRoot::destroy_Instant();
	LogText("!!!!!!!!!!!!!!!!!!!�ű��������гɹ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return true;
}

bool CapolloParseEditorDlg::expExcel()
{
	char path[1024];

	LogText("==============��ʼ����excel===============\n");

	const char *exp_cmd = _getFromIocfg("game_data_export_cmd");
	const char *excel_path = _getFromIocfg("excel_data_in_path");
	const char *text_path = _getFromIocfg("text_data_out_path");
	const char *package_file = _getFromIocfg("game_data_package_file");
	const char *excel_list = _getFromIocfg("game_data_listfile");

	if (!exp_cmd || !excel_path || !text_path || !package_file ){
		LogText("����excel����:�����ļ�����\n");
		return false ;
	}
	
	const char *cur_dir = nd_getcwd();
	const char *encodeName = getGameDateEncodeType();

	snprintf(path, sizeof(path), "cmd.exe /c %s/%s %s %s %s %s ", cur_dir, exp_cmd, excel_list, excel_path, text_path,  encodeName);
	int ret = system(path);

	if (0 != ret)	{
		nd_logerror("�����߻�����ʧ�ܣ����ֶ�����\n");
		return false;
	}

	LogText("excel �������!!!!!!!\n==================��ʼ���======================\n");

	//get byte order  
	int orderType = ND_L_ENDIAN;
	const char *orderName = _getFromIocfg("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	DBLDatabase dbtmp;
	if (0 != dbtmp.LoadFromText(text_path, excel_list, encodeName, encodeName)) {
		nd_logerror("������ݴ���:���ܴ�txt�ļ��ж�ȡ����\n");
		return false;
	}
	if (0 == dbtmp.Dump(package_file, "gamedatadb", orderType)) {
		nd_logmsg("package game data SUCCESS!!\n");
		LogText("==========�����߻����ݳɹ�===========\n");
	}
	else{
		nd_logerror("excel�����������ļ�����\n");
	}
	
	//before run test need load dbl 
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		if (0 != pdbl->LoadBinStream(package_file)) {
			nd_logmsg("���ض����Ʋ߻����ݴ���!\n");
			dbtmp.Destroy();
			DBLDatabase::destroy_Instant();
			return false;
		}
	}
	if (!(*pdbl == dbtmp)){
		nd_logmsg("������ݺ�ԭʼ���ݲ����!");
		dbtmp.Destroy();
		DBLDatabase::destroy_Instant();
		return false ;
	}
	
	dbtmp.Destroy();

	std::string strOutTextPath = text_path;
	strOutTextPath  += "/test_outputData";
	nd_mkdir(strOutTextPath.c_str());
	if (0 != pdbl->TestOutput(strOutTextPath.c_str())) {
		nd_logmsg("��data���text�ļ�ʧ��!");
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
	LogText("���ز߻��������\n���ؽű�.....\n");

	if (compile() == false ) {
		return false;
	}

	bool ret = true;

	const char *package_file = _getFromIocfg("game_data_package_file");

	if (0 != DBLDatabase::get_Instant()->LoadBinStream(package_file)) {
		LogText("���ض����Ʋ߻����ݴ���!\n");
		return false;;
	}

	do 	{
		const char*attr_table = _getFromIocfg("role_attr_table");
		NDSingleton<RoleAttrHelper>::Destroy();
		//StatMachine::Destroy();

		if (0 != NDSingleton<RoleAttrHelper>::Get()->Load(attr_table,"role_level_attr.xlsx")) {
			LogText("��ɫ���Լ���ʧ��\n");
			ret = false;
			goto ERROR_EXIT;
		}
// 		if (0 != StatMachine::load(forbid_table, state_table, operate_table)) {
// 			LogText("״̬�����ʧ��\n");
// 			ret = false;
// 			goto ERROR_EXIT;
// 		}
		NDSingleton<RoleAttrHelper>::Destroy();
		//StatMachine::Destroy();

		LogText("״̬���Լ��ز��Գɹ�\n");

	} while (0);
	//test role attribute 

	LogText("�����������������Գɹ���������������������\n");

	
ERROR_EXIT:

	DBLDatabase::destroy_Instant();
	return ret;

}


void CapolloParseEditorDlg::_beginEdit(const char *script_file)
{
	const char *filename;
	clearLog();

	LogText("�༭��.....\n");
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
		LogText("�༭���\n");
	}
	else {
		LogText("û���޸�\n");
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