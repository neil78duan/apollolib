readme.txt

用nodejs 实现的一个程序数据更新服务器

自动部署 ./publish

访问后台管理
http://hostname:8000/
后台功能：
Upload Version Data		-> 上传数据
View Upload logs		-> 查看上传日志
Server Version Info		-> 查看服务器信息

版本服务器功能


http://hostname/loaddata -> 下载版本数据
用法：
http://localhost/loaddata?data=1.1.1&name=data.zip
版本1.1.1 上传的文件名是data.zip

获取每个版本对应的文件名 （下载txt文件）
http://hostname/getlist

所有地址使用get 方法

版本数据的生产：
	使用恰当的客户端生产一个zip压缩包，然后放到指定版本的目录。比如 目录 1.1.2
之后使用 create_md5.sh工具生成版本的md5数据（把目录1.1.2和 create_md5.sh 放在同一个目录中）
然后登陆
http://localhost:8000/liveupdate
上传即可


在远程机器上部署：
	1 把liveupdate（redhat版本）拷贝到 /etc/init.d/ 目录下
	2 在home目录下建liveupdate 目录，设置相应的权限
	3 把run_insrv.sh拷贝到远程机器的liveupdate目录
	4 修改 publish.sh 倒数第二行 	update_hostFile $AWS_APO_HOST "-i $AWS_APO_PEM" 运行即可

duan

good luck
2015