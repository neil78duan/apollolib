using System;
using UnityEngine;

using System.IO;

using System.Collections.Generic;
using System.Runtime.InteropServices;


public class apoFormula {	
#if UNITY_IPHONE || UNITY_XBOX360
	const string APO_DLL_NAME = "__Internal";
#else    
    const string APO_DLL_NAME = "apollo_u3d";
#endif

    [DllImport (APO_DLL_NAME)]
	private static extern bool apoFormulaNameInit(int id, string name, string alias);
	
	[DllImport (APO_DLL_NAME)]
	private static extern void apoFormulaSetEncode(string encodeName);
    
    [DllImport (APO_DLL_NAME)]
	private static extern  bool apoFormulaSetValue(int id, float value);
    
    [DllImport (APO_DLL_NAME)]
	private static extern unsafe bool apoFormulaRun(string formulaText, float *result);
	
	public unsafe bool runTestFormula() 
    {
		apoFormulaSetEncode(@"utf8");
		//init role attribute list 
		apoFormulaNameInit(1, @"attack", @"攻击力");
		apoFormulaNameInit(2, @"def", @"防御力");
		
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

}