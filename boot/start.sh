cd /home/docker-home/nginx/
/usr/local/bin/docker-compose up -d
sleep 3 
cd /home/docker-home/pg13/
/usr/local/bin/docker-compose up -d
sleep 3
cd /home/docker-home/redis/
/usr/local/bin/docker-compose up -d
sleep 3
cd /home/apps/wrcs/
./deploy.sh
sleep 3
cd /home/bind9/
/usr/local/bin/docker-compose up -d
