历史记录在 /home/alantop/.history目录
按照用户名存放

修改配置文件 /etc/profile

#Record history operation
USER_IP=`who -u am i 2>/dev/null |awk '{print $NF}' |sed -e 's/[()]//g'`
LOGNAME=`who -u am i |awk '{print $1}'`
HISTDIR=/home/alantop/.history
if [ -z $USER_IP ]
then
    USER_IP=`hostname`
fi

if [ ! -d $HISTDIRi ]
then
    mkdir -p $HISTDIR
    chmod 777 $HISTDIR
fi

if [ ! -d $HISTDIR/${LOGNAME} ]
then
    mkdir -p $HISTDIR/${LOGNAME}
    chmod 300 $HISTDIR/${LOGNAME}
fi

export HISTSIZE=100000

DT=`date +"%Y%m%d_%H%M%S"`
export HISTFILE="$HISTDIR/${LOGNAME}/${USER_IP}.history.$DT"
export HISTTIMEFORMAT="[%Y.%m.%d %H:%M:%S]"
chmod 600 $HISTDIR/${LOGNAME}/*.history* 2>/dev/null

#alantop 2024/06/17
