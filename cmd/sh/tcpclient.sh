exec 9<>/dev/tcp/10.100.70.139/19006    #创建tcp链接

echo -e "stats" >&9     #发送消息

#关闭链接：

exec 9>&-

exec 9<&-
