# -*- coding: utf-8 -*-

import xlrd
import codecs

import xml.dom.minidom as xmldom
import sys

import os

reload(sys)
if len(sys.argv) >= 2 :
    sys.setdefaultencoding(sys.argv[2])
else :
    sys.setdefaultencoding('utf8')

	
def getSheetCols(table):
	try:
		ncols = table.ncols
		nRet = 0 
		for j in range(0,ncols ):
			if not table.cell(0,j).value.strip():
				return nRet 
			nRet += 1
		return nRet
	except Exception,e:
		return nRet
		
		
def ReadExcel(fileName):
	try:
		data = xlrd.open_workbook(fileName)
		table = data.sheet_by_index(0) #通过索引顺序获取
		
		nrows = table.nrows
		ncols = getSheetCols(table) 		
		# print "data [ %d, %d]" %(nrows,ncols)
		
		for i in range(0, nrows ):		
			
			if i == 2 or i == 3 or i== 4:
				continue
				
			tmptxt=""
			for j in range(0,ncols ):
				tmptxt += str(table.cell(i,j).value)+"\t"
			print(tmptxt)
		return 0
	except Exception,e:	
		print "parse error [ %d, %d]" %(i,j)
		print str(e)
		return 1

def main(argv):
	try:
		ret = ReadExcel(argv[1])
		sys.exit(ret)
	except Exception as e:
		print e
		print "%s convert error" % argv[1]
		sys.exit(1) ;

if __name__ == "__main__":
	main(sys.argv)