using System;
using UnityEngine;

using System.IO;

using System.Collections.Generic;
using System.Runtime.InteropServices;

//using NetMessage;


public class apoFormula {	
#if (UNITY_IPHONE || UNITY_XBOX360) && !UNITY_EDITOR
	const string APO_DLL_NAME = "__Internal";
#else    
    const string APO_DLL_NAME = "apollo_u3d";
#endif

	[DllImport (APO_DLL_NAME)]
	public static extern void apoFormulaSetEncode(string encodeName); ////全局函数

    [DllImport (APO_DLL_NAME)]
	public static extern bool apoFormulaNameInit(int id, string name, string alias,string formulaText= ""); //全局函数

    [DllImport(APO_DLL_NAME)]
    public static extern void apoFormulaPreParse();     //全局函数

    [DllImport (APO_DLL_NAME)]
	public static extern  bool apoFormulaSetValue(int id, float value);
	
    [DllImport (APO_DLL_NAME)]
	public static extern  bool apoFormulaAddValue(int id, float value);
	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe bool apoFormulaRun(string formulaText, float *result);
	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe void apoFromulaResetValue();
	
	
	// add new function 	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe IntPtr apoCreateAttrForHelper();
	
	[DllImport (APO_DLL_NAME)]
	public static extern unsafe void apoDestroyAttrForHelper(IntPtr calcHelperObj);

	//[DllImport (APO_DLL_NAME)]
	//public static extern  void apoAttrForInit(IntPtr calcHelperObj, int aid, string name, string alias, string formula);

	//[DllImport (APO_DLL_NAME)]
	//public static extern  void apoAttrForPreParse(IntPtr calcHelperObj);

	[DllImport (APO_DLL_NAME)]
	public static extern bool apoAttrForSetValue(IntPtr calcHelperObj,int id, float value);

	[DllImport (APO_DLL_NAME)]
	public static extern bool apoAttrForAddValue(IntPtr calcHelperObj, int id, float value);

	[DllImport (APO_DLL_NAME)]
	public static extern float apoAttrForGetValue(IntPtr calcHelperObj, int id);


    [DllImport(APO_DLL_NAME)]
    public static extern int apoAttrForGetCount(IntPtr calcHelperObj);

    [DllImport(APO_DLL_NAME)]
    public static extern void apoAttrForResetValues(IntPtr calcHelperObj);

    [DllImport (APO_DLL_NAME)]
	public static extern int apoAttrForCalcAll(IntPtr calcHelperObj);
	

    [DllImport (APO_DLL_NAME)]
	public static extern float apoAttrForCalcOne(IntPtr calcHelperObj, int id);

	[DllImport (APO_DLL_NAME)]
	public static extern unsafe  bool apoAttrForRun(IntPtr calcHelperObj, string formulaText, float *result);


    [DllImport(APO_DLL_NAME)]
    public static extern unsafe bool apoAttrForSetValues(IntPtr calcHelperObj, float []value, int count);


    struct transferFloatArray
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public float[] dst_addr;
    }

    [DllImport(APO_DLL_NAME)]
    private static extern unsafe int apoAttrForGetValues(IntPtr calcHelperObj, ref transferFloatArray outArray, int count);

    struct transferIntArray
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public int[] dst_addr;
    }

    [DllImport(APO_DLL_NAME)]
    private static extern unsafe int apoFromulaGetCalcSort(ref transferIntArray outArray, int bufsize);

    [DllImport(APO_DLL_NAME)]
    private static extern unsafe int apoAttrForGetInfections(IntPtr calcHelperObj, int attrid, ref transferIntArray outArray, int bufsize);


    public static int getAllValues(IntPtr calcHelperObj, ref float[] attrVals)
    {
        transferFloatArray tdata;
        tdata.dst_addr = new float[256];

        int ret = apoAttrForGetValues( calcHelperObj,ref tdata, 256);
        for (int i = 0; i < ret; ++i)
        {
            attrVals[i] = tdata.dst_addr[i];
        }
        return ret;
    }

    public static int getInfections(IntPtr calcHelperObj, int attrid, ref int[] attrIds)
    {
        transferIntArray tdata;
        tdata.dst_addr = new int[256];

        int ret = apoAttrForGetInfections(calcHelperObj, attrid, ref tdata, 256);
        for (int i = 0; i < ret; ++i)
        {
            attrIds[i] = tdata.dst_addr[i];
        }
        return ret;
    }

    public struct attrCfgInfo
    {
        public int aId;
        public string name;
        public string aliasname;
        public string formulaText;
    }

    public static void globalInitConfig(ref attrCfgInfo[] attrCfgs)
    {
        for (int i = 0; i < attrCfgs.Length; ++i)
        {
            apoFormulaNameInit( attrCfgs[i].aId, attrCfgs[i].name, attrCfgs[i].aliasname, attrCfgs[i].formulaText);
        }
        apoFormulaPreParse();        
    }

    //创建一个属性计算器
    public static unsafe IntPtr creatAttrHelper()
    {
        return apoCreateAttrForHelper();
    }

    //通过属性计算器调用表格中所有公式计算，并输出结果到attrVals
    public static int calcAllAttr(IntPtr pHelper, ref float[] attrVals)
    {
        int ret = 0;
        for (int i = 0; i < attrVals.Length; ++i)
        {
            if (attrVals[i] != 0)
            {
                apoAttrForSetValue(pHelper, i, attrVals[i]);
            }
        }
        ret = apoAttrForCalcAll(pHelper);

        for (int i = 0; i < attrVals.Length; ++i)
        {
            attrVals[i] = apoAttrForGetValue(pHelper, i);
        }
        return ret;
    }


    public unsafe bool testOfNeWObject()
    {
        apoFormulaSetEncode(@"utf8");
        //global init role attribute list 
        apoFormulaNameInit(1, @"attack", @"攻击力", @"(力量+基础加成) * (1+力量百分比)");

        apoFormulaNameInit(2, @"power", @"力量", @"基础力量+力量加成");
        apoFormulaNameInit(3, @"powerBase", @"基础力量");
        apoFormulaNameInit(4, @"powerAddon", @"力量加成");

        apoFormulaNameInit(5, @"powerRate", @"力量百分比", @"力量百分百基础+力量百分百加成");
        apoFormulaNameInit(6, @"powerRateBase", @"力量百分百基础");
        apoFormulaNameInit(7, @"powerRateAddon", @"力量百分百加成");
        apoFormulaNameInit(8, @"powerBaseAddon", @"基础加成");
        apoFormulaNameInit(108, @"@powerBaseAddon", @"@基础加成");

        apoFormulaPreParse();

        // end global init 

        IntPtr pInst = apoCreateAttrForHelper();
        
        //set new value
        apoAttrForSetValue(pInst, 3, 80f);
        apoAttrForSetValue(pInst, 4, 20f);

        apoAttrForSetValue(pInst, 6, 0.10f);
        apoAttrForSetValue(pInst, 7, 0.10f);

        int changedNum = apoAttrForCalcAll(pInst);

        float[] results = new float[200];

        int allNum = getAllValues(pInst, ref results);

        int[] attrIds = new int[200];
        int nInrefecNum = getInfections(pInst, 7, ref attrIds);


        apoAttrForSetValue(pInst, 108, 10);
        apoAttrForSetValue(pInst, 8, 20);

        float res = 0;
        apoAttrForRun(pInst, @"@基础加成-基础加成", &res);

        return true;
    }

    /*
    //示例，如何计算明星属性
    public bool CalcStarAttrs(IntPtr pHelper,ref NetMessage.StarInfo star, ref NetMessage.EquipInfo []equips, ref NetMessage.AssistantInfo []assis, ref float[] outAttrVals)
    {
        apoAttrForResetValues(pHelper);

        //add start attribute 
        for (int i=0; i< star.attrsCount; i++)
        {
            apoAttrForAddValue(pHelper, star.attrs[i].id, star.attrs[i].val);
        }

        //add equips 
        for (int x = 0; x < equips.Length; ++x)
        {
            for (int i = 0; i < equips[x].attrCount; i++)
            {
                apoAttrForAddValue(pHelper,  equips[x].attr[i].id , equips[x].attr[i].val);
            }

            for (int i = 0; i < equips[x].attrRandCount; i++)
            {
                apoAttrForAddValue(pHelper, equips[x].attrRand[i].id, equips[x].attrRand[i].val);
            }
            
        }

        for (int x = 0; x < assis.Length; ++x)
        {
            for (int i = 0; i < assis[x].attrCount; i++)
            {
                apoAttrForAddValue(pHelper, assis[x].attr[i].id, assis[x].attr[i].val);
            }

            for (int i = 0; i < assis[x].attrRandCount; i++)
            {
                apoAttrForAddValue(pHelper,assis[x].attrRand[i].id, assis[x].attrRand[i].val);
            }
        }

        if(0==apoAttrForCalcAll(pHelper))
        {
            return false;
        }

        for ( int i=0;i< apoAttrForGetCount(pHelper); i++)
        {
            outAttrVals[i] = apoAttrForGetValue( pHelper, i);
        }
        return true;
    }
    */
}

//sample 
public class AttrCalculator
{
    protected IntPtr m_calcObj = apoFormula.creatAttrHelper();

    public void SetValue(int id, float val)
    {
        apoFormula.apoAttrForSetValue(m_calcObj, id, val);
    }

    public float GetValue(int id)
    {
        return apoFormula.apoAttrForGetValue(m_calcObj, id);
    }
    public int CalcAll()
    {
        return apoFormula.apoAttrForCalcAll(m_calcObj);
    }    
}