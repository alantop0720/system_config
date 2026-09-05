current_datetime=$(date +%Y%m%d_%H%M%S)
filename="file_${current_datetime}.pcap"
echo "生成的文件名：$filename"
tcpdump -i ens33 host 210.22.133.114 -w ${filename}

