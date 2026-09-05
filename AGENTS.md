# AGENTS.md

## 仓库概述

系统配置集合仓库，包含 Linux 服务器管理脚本、Docker 部署配置、开发环境设置和自定义命令行工具。非软件工程项目。

## 目录结构

- `cmd/` — 自定义命令行工具集合
  - `cmd/src/` — 源代码（C、C++、Python）
  - `cmd/sh/` — Shell 脚本工具
  - `cmd/py/` — Python 脚本工具
- `boot/` — 服务器启动/停止脚本（Docker 容器编排）
- `config/` — 系统配置文件（bashrc、systemd service 等）
- `docker/` — Docker daemon 配置
- `docker-home/` — Docker Compose 部署目录（nginx 等）
- `vim配置/` — Vim 配置和插件
- `zsh/` — Zsh 和 oh-my-zsh 配置
- `win_cmd/` — Windows 命令行工具（Visual Studio C++ 项目）
- `win_set/` — Windows 系统设置脚本
- `显示信息的脚本/` — 系统信息显示脚本
- `x86/` — x86 架构相关配置

## 构建命令

### C/C++ 工具（cmd/src/c 和 cmd/src/cpp）

```bash
# Linux 下编译
cd cmd/src/cpp && make        # C++ 版本
cd cmd/src/c && make          # C 版本

# 安装到 /usr/local/bin
cd cmd/src/cpp && make install
cd cmd/src/c && make install
```

### Windows 工具（win_cmd）

使用 Visual Studio 打开 `cmdline.sln` 编译。

### Python 工具（cmd/src/uv_cmd）

使用 uv 管理依赖：
```bash
cd cmd/src/uv_cmd
uv sync
uv run python cmd.py
```

## 服务器部署

### 启动所有服务

```bash
bash boot/start.sh
```

按顺序启动：nginx → pg13 → redis → wrcs → bind9

### 停止所有服务

```bash
bash boot/stop.sh
```

## 关键约定

- 仓库以中文注释和文档为主
- 脚本中的硬编码路径（如 `/home/cmd/`）是部署目标路径，不要修改
- Docker Compose 文件使用镜像 ID 而非标签，修改时注意版本兼容
- 安装脚本会直接 `cp` 到 `/usr/local/bin/`，确保编译通过后再执行 install
