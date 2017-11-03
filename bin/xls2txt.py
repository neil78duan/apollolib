# -*- coding: utf-8 -*-

import xlrd
import codecs

import xml.dom.minidom as xmldom
import sys

import os

reload(sys)
#sys.setdefaultencoding('utf8')
sys.setdefaultencoding(sys.argv[3])

def ReadExcel(fileName, outfileName):
	try:
		outfile = open(outfileName,'w') ;
		data = xlrd.open_workbook(fileName)
		table = data.sheet_by_index(0) #通过索引顺序获取
		
		nrows = table.nrows
		ncols = table.ncols
		
		for i in range(0, nrows ):
			if i == 2 or i == 3:
				continue
			tmptxt=""
			for j in range(0,ncols ):
				tmptxt += str(table.cell(i,j).value)+"\t"
			tmptxt += "\n"
			outfile.write(tmptxt)
		
		outfile.close() ;
		return 0
	except Exception,e:
		print str(e)
		return 1

def main(argv):
	try:
		ret = ReadExcel(argv[1],argv[2])
		sys.exit(ret)
	except Exception as e:
		print e
		print "%s convert error" % argv[1]
		sys.exit(1) ;

if __name__ == "__main__":
	main(sys.argv)