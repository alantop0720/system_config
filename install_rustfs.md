# RustFS 安装指南（Docker 方式）

## 环境信息

- 操作系统：Ubuntu 24.04.4 LTS
- Docker 版本：29.6.1
- RustFS 版本：latest
- 安装目录：/home/user/rustfs
- 数据目录：/home/user/rustfs/data

## RustFS 简介

RustFS 是一款高性能、100% 兼容 S3 的开源分布式对象存储系统。支持单节点和多节点部署，适用于本地测试、小型场景和生产环境。

## 安装步骤

### 1. 创建目录结构

```bash
mkdir -p /home/user/rustfs/data
```

### 2. 设置数据目录权限

RustFS 容器以非 root 用户 `rustfs` (id 为 `10001`) 运行，需要设置正确的权限：

```bash
sudo chown -R 10001:10001 /home/user/rustfs/data
```

### 3. 拉取 RustFS 官方镜像

```bash
docker pull rustfs/rustfs
```

### 4. 运行 RustFS 容器

```bash
docker run -d \
  --name rustfs \
  -p 9000:9000 \
  -p 9001:9001 \
  -v /home/user/rustfs/data:/data \
  -e RUSTFS_ACCESS_KEY=alantop \
  -e RUSTFS_SECRET_KEY=13601994674 \
  -e RUSTFS_CONSOLE_ENABLE=true \
  rustfs/rustfs:latest \
  --address :9000 \
  --console-enable \
  --access-key alantop \
  --secret-key 13601994674 \
  /data
```

### 参数说明

| 参数 | 说明 |
|------|------|
| `-d` | 后台运行容器 |
| `--name rustfs` | 容器名称 |
| `-p 9000:9000` | 映射 S3 API 端口 |
| `-p 9001:9001` | 映射 Web 控制台端口 |
| `-v /home/user/rustfs/data:/data` | 挂载数据卷 |
| `-e RUSTFS_ACCESS_KEY` | 设置访问密钥 |
| `-e RUSTFS_SECRET_KEY` | 设置密钥 |
| `-e RUSTFS_CONSOLE_ENABLE=true` | 启用 Web 控制台 |

## 遇到的问题及解决方案

### 问题 1：Docker 权限不足

**现象：**
```
permission denied while trying to connect to the docker API at unix:///var/run/docker.sock
```

**原因：** 当前用户不在 `docker` 组中，无权访问 Docker 守护进程。

**解决方案：** 使用 `sudo` 执行 Docker 命令：
```bash
sudo docker pull rustfs/rustfs
sudo docker run -d ...
```

或者将用户加入 docker 组（需要重新登录）：
```bash
sudo usermod -aG docker $USER
```

---

### 问题 2：数据目录权限拒绝

**现象：**
```
permission denied while trying to create data directory
```

**原因：** 容器以用户 ID 10001 运行，主机目录权限不匹配。

**解决方案：**
```bash
sudo chown -R 10001:10001 /home/user/rustfs/data
```

---

### 问题 3：端口被占用

**现象：**
```
Bind for 0.0.0.0:9000 failed: port is already allocated
```

**原因：** 主机 9000 或 9001 端口已被其他服务占用。

**解决方案：**
```bash
# 检查端口占用
ss -tuln | grep -E "9000|9001"

# 停止占用端口的服务，或修改映射端口
docker run -d -p 9002:9000 -p 9003:9001 ...
```

---

### 问题 4：健康检查返回 AccessDenied

**现象：**
```bash
curl http://localhost:9000/minio/health/live
# 返回：Access Denied
```

**原因：** 健康检查端点需要认证，这是正常行为。

**解决方案：** 使用正确的 S3 签名或通过 Web 控制台验证：
```bash
# 通过容器内部测试
docker exec rustfs curl -s http://localhost:9000/minio/health/live
```

---

### 问题 5：mc 命令不是 MinIO 客户端

**现象：**
```
mc 命令打开的是文件管理器而非 MinIO 客户端
```

**原因：** 系统中的 `mc` 是 Midnight Commander 文件管理器。

**解决方案：** 安装真正的 MinIO 客户端：
```bash
wget https://dl.min.io/client/mc/release/linux-amd64/mc
chmod +x mc
sudo mv mc /usr/local/bin/mc-client
```

## 验证安装

### 方法 1：检查容器状态

```bash
# 查看运行中的容器
docker ps | grep rustfs

# 预期输出：
# CONTAINER ID   IMAGE                  COMMAND                  STATUS          PORTS                                                             NAMES
# 36b413dafc68   rustfs/rustfs:latest   "/entrypoint.sh ..."     Up 10 seconds   0.0.0.0:9000-9001->9000-9001/tcp, [::]:9000-9001->9000-9001/tcp   rustfs
```

### 方法 2：查看容器日志

```bash
docker logs rustfs

# 预期输出：
# Initializing data directories: /data
# Initializing log directory: /logs
# Starting: /usr/bin/rustfs --address :9000 --console-enable --access-key alantop --secret-key 13601994674 /data
```

### 方法 3：检查端口监听

```bash
ss -tuln | grep -E "9000|9001"

# 预期输出：
# tcp   LISTEN   0   4096   0.0.0.0:9001   0.0.0.0:*
# tcp   LISTEN   0   4096   0.0.0.0:9000   0.0.0.0:*
```

### 方法 4：访问 Web 控制台

1. 打开浏览器访问：http://localhost:9001
2. 输入用户名：`alantop`
3. 输入密码：`13601994674`
4. 登录成功即表示安装完成

### 方法 5：使用 curl 测试 API

```bash
# 测试服务是否响应（返回 AccessDenied 表示服务正常）
curl -s http://localhost:9000/

# 预期输出：
# <?xml version="1.0" encoding="UTF-8"?><Error><Code>AccessDenied</Code><Message>Access Denied</Message></Error>
```

### 方法 6：安装 MinIO 客户端测试

```bash
# 下载并安装 mc
wget https://dl.min.io/client/mc/release/linux-amd64/mc
chmod +x mc
sudo mv mc /usr/local/bin/

# 配置别名
mc alias set rustfs http://localhost:9000 alantop 13601994674

# 测试连接
mc ls rustfs
```

## 常用命令

```bash
# 查看容器状态
docker ps

# 查看日志
docker logs rustfs

# 停止容器
docker stop rustfs

# 启动容器
docker start rustfs

# 重启容器
docker restart rustfs

# 删除容器
docker rm -f rustfs

# 进入容器
docker exec -it rustfs /bin/bash
```

## 访问信息

- **Web 控制台**: http://localhost:9001
- **S3 API**: http://localhost:9000
- **用户名**: alantop
- **密码**: 13601994674

## 生产环境建议

1. **修改默认密码**：使用强密码替换默认的 alantop/13601994674
2. **启用 TLS**：配置 HTTPS 加密通信
3. **多节点部署**：采用分布式架构提高可用性
4. **定期备份**：设置数据备份策略
5. **监控告警**：部署 Prometheus + Grafana 监控

## 参考链接

- [RustFS 官方文档](https://docs.rustfs.cn/)
- [RustFS Docker 安装指南](https://docs.rustfs.cn/installation/docker/index.html)
- [RustFS GitHub](https://github.com/rustfs/rustfs)