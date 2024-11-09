cd /home/docker-home/pg13/
/usr/local/bin/docker-compose down
sleep 3
cd /home/docker-home/redis/
/usr/local/bin/docker-compose down
sleep 3
cd /home/docker-home/nginx/
/usr/local/bin/docker-compose down
sleep 3
cd /home/bind9/
/usr/local/bin/docker-compose down

