<?php
$username = $_GET['username'];
$passwd   = $_GET['passwd'];
$sex      = $_GET['sex'];
$age      = $_GET['age'];


$db_host='192.168.7.160';
$db_database='zyl_server';
$db_username='arpg';
$db_password='E8M4LQD5R7JIkR2xIa6wgjblTJy0meOU';

function isCorrectParp()
{
	return true;
}
if (count(_GET) != 4 || isCorrectParp())
{
	echo "用户名或密码不合法！";
}
else{

$connection=mysql_connect($db_host,$db_username,$db_password);//连接到数据库
mysql_query("set names 'utf8'");//编码转化
if(!$connection){
die("could not connect to the database:</br>".mysql_error());//诊断连接错误

}
$db_selecct=mysql_select_db($db_database);//选择数据库
if(!$db_selecct)
{
die("could not to the database</br>".mysql_error());	
}
$query="insert into t_account(username,passwd,sex,age) values('".$username."','".$passwd."','".$sex."','".$age."')";//构建查询语句
$result=mysql_query($query);//执行查询
//	echo ($result==False);
if(mysql_num_rows($result)<1)
{
	echo "注册失败!";
}
else
{
	echo "注册成功!"
}
}
?>