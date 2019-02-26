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
		
def ReadArray(sheet_val):
	if(sheet_val.strip() == ""):
		return sheet_val
	sheet_val = "[" + sheet_val + "]"
	sheet_val = sheet_val.replace(';',',')
	sheet_val = sheet_val.replace('-',',')

	if (sheet_val.count("|") > 0):
		sheet_val = "[" + sheet_val + "]"
	sheet_val = sheet_val.replace("|","],[")
	
	return sheet_val
	


def ReadSheet(fileName, outPath, sheet, outEncoding):
	try: 
		outfile = open(outPath + "/" + fileName + "_" + sheet.name + ".txt", "w+", encoding=outEncoding)
		
		nrows = sheet.nrows
		ncols = getSheetCols(sheet) 
		
		print( "data [ %d, %d]" %(nrows,ncols))
		
		type_list = []
		for i in range(0, nrows):
		
			if i == 1:
				continue
			
			tmptxt=""
			for j in range(0,ncols ):
				if(i == 2):
					type_list.append(sheet.cell(i,j).value)
				if(i > 2 and type_list[j] == "array"):
					tmptxt += ReadArray(str(sheet.cell(i,j).value)) + "\t"	
				else:
					tmptxt += str(sheet.cell(i,j).value) + "\t"
					
			tmptxt += "\n"
			outfile.write(tmptxt)
		
		outfile.close()
		return 0
	except:
		print("parse error [ %d, %d]" %(i,j))
		print("read Unexpected error:", sys.exc_info()[0])
		return 1

def WriteFileList(fileName, outPath, outEncoding):
	filelist = open(outPath + "file_list.txt", "a+", encoding=outEncoding)
	filelist.write(fileName + "\n")

def ReadExcel(inPath, fileName, outPath, outEncoding):
	print("read excel", inPath, fileName, outPath, outEncoding)
	data = xlrd.open_workbook(inPath + "/" + fileName)
	for sheet in data.sheets():
		ReadSheet(fileName, outPath, sheet, outEncoding)
		
	
def main(argv):
	ret = ReadExcel(argv[1], argv[2], argv[3], argv[4])
	sys.exit(ret)	
if __name__ == "__main__":
	main(sys.argv)
