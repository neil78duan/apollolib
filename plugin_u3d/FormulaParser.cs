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
	public static extern bool apoFormulaNameInit(int id, string name, string alias);
	
	[DllImport (APO_DLL_NAME)]
	public static extern void apoFormulaSetEncode(string encodeName);
    
    [DllImport (APO_DLL_NAME)]
	public static extern  bool apoFormulaSetValue(int id, float value);
	
    [DllImport (APO_DLL_NAME)]
	public static extern  bool apoFormulaAddValue(int id, float value);
	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe bool apoFormulaRun(string formulaText, float *result);
	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe void apoFromulaResetValue();
	
	public unsafe bool runTestFormula() 
    {
		apoFormulaSetEncode(@"utf8");
		//init role attribute list 
		apoFormulaNameInit(1, @"attack", @"攻击力");
		apoFormulaNameInit(2, @"def", @"防御力");

		apoFromulaResetValue();

		//set new value
		apoFormulaSetValue(1, 100);
		apoFormulaSetValue(2, 50);
		
		float result =0;
		
		bool ret = apoFormulaRun(@"攻击力 * 1.5 - 防御力 * 2", &result);
		if(!ret) {
			Debug.LogError("read message error \n");
		}
		return ret;
    }
	
	// add new function 
	
    [DllImport (APO_DLL_NAME)]
	public static extern unsafe IntPtr apoCreateAttrForHelper();
	
	[DllImport (APO_DLL_NAME)]
	public static extern unsafe void apoDestroyAttrForHelper(IntPtr calcHelperObj);

	[DllImport (APO_DLL_NAME)]
	public static extern  void apoAttrForInit(IntPtr calcHelperObj, int aid, string name, string alias, string formula);

	[DllImport (APO_DLL_NAME)]
	public static extern  void apoAttrForPreParse(IntPtr calcHelperObj);

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
	public static extern unsafe  bool apoAttrForRun(IntPtr calcHelperObj, string formulaText, float *result);

    public struct attrCfgInfo
    {
        public int aId;
        public string name;
        public string aliasname;
        public string formulaText;
    }

    //创建一个属性计算器
    public unsafe IntPtr creatAttrHelper(ref attrCfgInfo []attrCfgs)
    {
        IntPtr pHelper = apoCreateAttrForHelper();
        if(pHelper==null)
        {
            return pHelper;
        }
        for (int i = 0; i < attrCfgs.Length; ++i)
        {
            apoAttrForInit(pHelper, attrCfgs[i].aId, attrCfgs[i].name, attrCfgs[i].aliasname, attrCfgs[i].formulaText);
        }
        apoAttrForPreParse(pHelper);
        return pHelper;
    }

    //
    public  int creatAttrHelper(IntPtr pHelper, ref float[] attrVals)
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
