# -*- coding: utf-8 -*-

import xlrd
import codecs

import xml.dom.minidom as xmldom
import sys,io

import os

#reload(imp)
#sys.setdefaultencoding(sys.argv[3])
#sys.stdout = io.TextIOWrapper(sys.stdout.detach(), encoding="utf-8")
#sys.stderr = io.TextIOWrapper(sys.stderr.detach(), encoding="utf-8")

def getSheetCols(table):
	try:
		ncols = table.ncols
		nRet = 0 
		for j in range(0,ncols ):
			if not table.cell(0,j).value.strip():
				return nRet 
			nRet += 1
		return nRet
	except SystemExit:
		return nRet 
	except:
		return nRet 
	finally:
		return nRet 
		

def ReadExcel(fileName, outfileName):
	try:
		outfile = open(outfileName,'w') ;
		data = xlrd.open_workbook(fileName)
		table = data.sheet_by_index(0) #通过索引顺序获取
		
		nrows = table.nrows
		ncols = getSheetCols(table) 
		
		
		print( "data [ %d, %d]" %(nrows,ncols))
		
		for i in range(0, nrows ):
		
			if i == 2 or i == 3 or i== 4:
				continue
				
			tmptxt=""
			for j in range(0,ncols ):
				tmptxt += str(table.cell(i,j).value)+"\t"
			tmptxt += "\n"
			outfile.write(tmptxt)
		
		outfile.close() ;
		return 0
	except :
		print("parse error [ %d, %d]" %(i,j))
		print("read Unexpected error:", sys.exc_info()[0])
		return 1

def main(argv):
	ret = ReadExcel(argv[1],argv[2])
	sys.exit(ret)	
if __name__ == "__main__":
	main(sys.argv)
