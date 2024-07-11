./dufs upload  --allow-upload --allow-search  -a alantop:passwd@/:rw -a guest:passwd@/ -p 9720
当前目录建立 upload


./dufs upload  --allow-upload --allow-search  -a alantop:passwd@/:rw -p 9720


curl http://hcl.qv-china.com:9720/frp_0.58.1_linux_amd64.tar.gz --user alantop:1


curl http://127.0.0.1:5000/file --user user:pass 

curl http://127.0.0.1:80/1.txt -I --user admin:admin


serve-path: '.'
bind: 0.0.0.0
port: 5000
path-prefix: /dufs
hidden:
  - tmp
  - '*.log'
  - '*.lock'
auth:
  - admin:admin@/:rw
  - user:pass@/src:rw,/share
  - '@/'  # According to the YAML spec, quoting is required.
allow-all: false
allow-upload: true
allow-delete: true
allow-search: true
allow-symlink: true
allow-archive: true
enable-cors: true
render-index: true
render-try-index: true
render-spa: true
assets: ./assets/
log-format: '$remote_addr "$request" $status $http_user_agent'
log-file: ./dufs.log
compress: low
tls-cert: tests/data/cert.pem
tls-key: tests/data/key_pkcs1.pem

