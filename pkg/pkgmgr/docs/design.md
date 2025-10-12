# PkgMgr - M4KK1包管理系统设计文档

## 概述

PkgMgr是M4KK1操作系统的核心包管理系统，负责软件包的安装、卸载、查询、更新、依赖处理与路径控制等。它采用创新的架构设计，提供高效、可靠和安全的软件包管理体验。

## 核心特性

### 1. 高效包管理
- **快速安装**: 优化的安装算法和并行处理
- **智能依赖**: 先进的依赖关系解析和自动解决
- **增量更新**: 最小化更新包大小和时间
- **批量操作**: 支持批量安装和卸载操作

### 2. 安全保障
- **数字签名**: 软件包数字签名验证
- **沙箱安装**: 安全的安装环境隔离
- **回滚支持**: 安装失败时自动回滚
- **权限控制**: 细粒度的权限管理系统

### 3. 灵活配置
- **多仓库支持**: 支持多个软件源
- **优先级管理**: 仓库优先级和包选择策略
- **钩子系统**: 安装前后的自定义脚本
- **插件架构**: 可扩展的插件系统

### 4. 创新功能
- **原子操作**: 所有操作要么完全成功，要么完全回滚
- **状态追踪**: 详细的操作历史和状态追踪
- **冲突检测**: 智能检测和解决包冲突
- **虚拟包**: 支持虚拟包和包组概念

## 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                    PkgMgr 主程序                             │
├─────────────────────────────────────────────────────────────┐
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐  │
│  │   前端界面   │ │   包数据库   │ │   依赖解析  │ │  仓库管理 │  │
│  │   Frontend  │ │   Database  │ │  Resolver  │ │  Repo    │  │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘  │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐  │
│  │  包安装器    │ │  包卸载器    │ │  包查询器    │ │ 包更新器 │  │
│  │  Installer  │ │ Uninstaller │ │  Query    │ │ Updater  │  │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘  │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐  │
│  │  文件提取    │ │  权限设置    │ │  配置处理    │ │ 钩子执行 │  │
│  │ Extractor  │ │ Permission │ │  Config   │ │ Hooks   │  │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 底层工具和库 (Utilities)                     │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐  │
│  │   压缩库     │ │   校验库     │ │   数据库    │ │ 网络库   │  │
│  │ Compression │ │ Checksum  │ │ Database │ │ Network │  │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## 包格式设计

### 包文件结构
```
package.pkg
├── PKGINFO          # 包信息文件
├── PKGDATA          # 包数据（压缩）
├── PKSIGN           # 数字签名
├── PKGSCRIPT        # 安装脚本
└── PKGHOOKS         # 钩子脚本
```

### 包信息格式 (PKGINFO)
```json
{
  "name": "package_name",
  "version": "1.0.0",
  "release": 1,
  "description": "Package description",
  "architecture": "m4kk1",
  "maintainer": "maintainer@example.com",
  "depends": ["dep1", "dep2"],
  "optdepends": ["optdep1:optional feature"],
  "conflicts": ["conflict_pkg"],
  "provides": ["virtual_pkg"],
  "replaces": ["old_pkg"],
  "files": [
    {
      "path": "/usr/bin/binary",
      "mode": "0755",
      "size": 12345,
      "checksum": "sha256:abc123..."
    }
  ],
  "scripts": {
    "pre_install": "pre_install.sh",
    "post_install": "post_install.sh",
    "pre_remove": "pre_remove.sh",
    "post_remove": "post_remove.sh"
  }
}
```

## 数据库设计

### 本地包数据库
```
pkgmgr/
├── local.db         # 本地安装包数据库
├── sync.db          # 同步仓库数据库
├── files.db         # 文件数据库
├── depends.db       # 依赖关系数据库
└── scripts.db       # 脚本数据库
```

### 数据库结构
```sql
-- 包信息表
CREATE TABLE packages (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    version TEXT NOT NULL,
    release INTEGER NOT NULL,
    arch TEXT NOT NULL,
    description TEXT,
    maintainer TEXT,
    install_date INTEGER,
    install_size INTEGER,
    install_reason INTEGER,
    packager TEXT,
    reason INTEGER,
    validation TEXT,
    md5sum TEXT,
    sha256sum TEXT
);

-- 文件表
CREATE TABLE files (
    id INTEGER PRIMARY KEY,
    package_id INTEGER,
    path TEXT NOT NULL,
    mode INTEGER,
    size INTEGER,
    checksum TEXT,
    FOREIGN KEY (package_id) REFERENCES packages(id)
);

-- 依赖表
CREATE TABLE depends (
    id INTEGER PRIMARY KEY,
    package_id INTEGER,
    dep_name TEXT NOT NULL,
    dep_version TEXT,
    dep_type INTEGER,
    FOREIGN KEY (package_id) REFERENCES packages(id)
);
```

## 核心操作

### 1. 包安装流程
```
1. 依赖解析 → 2. 冲突检测 → 3. 下载包文件
4. 验证签名 → 5. 提取文件 → 6. 设置权限
7. 执行钩子 → 8. 更新数据库 → 9. 清理缓存
```

### 2. 包卸载流程
```
1. 检查反依赖 → 2. 执行钩子 → 3. 删除文件
4. 更新数据库 → 5. 清理缓存
```

### 3. 包查询操作
- **按名称查询**: 快速名称匹配
- **按描述查询**: 全文搜索支持
- **按文件查询**: 反向文件查找
- **按依赖查询**: 依赖关系遍历

### 4. 包更新操作
- **增量更新**: 最小化更新大小
- **原子更新**: 确保更新一致性
- **回滚支持**: 更新失败时自动回滚

## 命令行接口

### 基本命令
```bash
# 安装包
pkgmgr -S package_name
pkgmgr -S package1 package2 package3

# 卸载包
pkgmgr -R package_name
pkgmgr -Rns package_name  # 同时删除依赖和配置文件

# 查询包
pkgmgr -Q package_name
pkgmgr -Qi package_name   # 详细信息
pkgmgr -Ql package_name   # 文件列表
pkgmgr -Qs keyword        # 搜索包

# 更新包
pkgmgr -Syu              # 更新所有包
pkgmgr -Syy              # 更新数据库
```

### 高级命令
```bash
# 依赖操作
pkgmgr -T package_name    # 显示依赖树
pkgmgr -Qdt              # 列出孤立包
pkgmgr -Qo /path/file     # 查询文件所属包

# 仓库操作
pkgmgr -Syy              # 刷新仓库数据库
pkgmgr --flect           # 生成镜像列表

# 系统操作
pkgmgr -Rns $(pkgmgr -Qdtq)  # 清理孤立包
pkgmgr -Qii package_name     # 完整包信息
```

## 钩子系统

### 钩子类型
- **pre_install**: 安装前钩子
- **post_install**: 安装后钩子
- **pre_upgrade**: 升级前钩子
- **post_upgrade**: 升级后钩子
- **pre_remove**: 卸载前钩子
- **post_remove**: 卸载后钩子

### 钩子脚本示例
```bash
#!/bin/m4sh
# post_install 钩子示例

case $1 in
    post_install)
        # 重新加载系统服务
        systemctl reload service_name
        ;;
    post_remove)
        # 清理残留文件
        rm -f /var/cache/app/cache.db
        ;;
esac
```

## 仓库管理

### 仓库配置
```bash
# /etc/pkgmgr.conf
[options]
HoldPkg     = pacman glibc
Architecture = auto
Color
CheckSpace
VerbosePkgLists

[core]
Server = https://repo.m4kk1.org/core/$arch

[extra]
Server = https://repo.m4kk1.org/extra/$arch

[community]
Server = https://repo.m4kk1.org/community/$arch
```

### 仓库优先级
1. **本地包**: 最高优先级
2. **显式安装**: 高优先级
3. **依赖包**: 标准优先级
4. **仓库顺序**: 按配置顺序

## 安全特性

### 数字签名
- **GPG签名**: 软件包GPG签名验证
- **证书链**: 证书链验证
- **离线验证**: 支持离线签名验证

### 沙箱机制
- **安装沙箱**: 隔离的安装环境
- **权限限制**: 最小权限原则
- **资源限制**: CPU和内存限制

### 审计日志
- **操作审计**: 所有操作详细记录
- **变更追踪**: 文件变更追踪
- **合规报告**: 安全合规报告生成

## 性能优化

### 1. 数据库优化
- **索引优化**: 高效的数据库索引
- **查询缓存**: 常用查询结果缓存
- **批量操作**: 批量数据库操作

### 2. 网络优化
- **并行下载**: 多线程并行下载
- **断点续传**: 下载中断恢复
- **镜像选择**: 智能镜像选择算法

### 3. 磁盘I/O优化
- **异步I/O**: 非阻塞磁盘操作
- **预读优化**: 智能预读策略
- **压缩存储**: 包数据压缩存储

## 与现有包管理器的对比

| 特性 | PkgMgr | Pacman | APT | Yum |
|------|--------|--------|-----|-----|
| 原子操作 | ✅ | ❌ | ✅ | ❌ |
| 依赖解析 | ✅ | ✅ | ✅ | ✅ |
| 回滚支持 | ✅ | ❌ | ✅ | ❌ |
| 钩子系统 | ✅ | ✅ | ✅ | ❌ |
| 沙箱安装 | ✅ | ❌ | ❌ | ❌ |
| 数字签名 | ✅ | ✅ | ✅ | ✅ |
| 并行下载 | ✅ | ✅ | ❌ | ❌ |
| 增量更新 | ✅ | ❌ | ❌ | ❌ |
| 虚拟包 | ✅ | ✅ | ✅ | ✅ |

## 开发计划

### 第一阶段：基础框架
- [ ] 核心数据结构和算法
- [ ] 基本的包操作（安装、卸载、查询）
- [ ] 简单的依赖处理
- [ ] 基础数据库设计

### 第二阶段：核心功能
- [ ] 高级依赖解析
- [ ] 仓库管理系统
- [ ] 钩子系统实现
- [ ] 安全特性集成

### 第三阶段：高级特性
- [ ] 原子操作支持
- [ ] 回滚机制
- [ ] 沙箱机制
- [ ] 性能优化

### 第四阶段：完善和测试
- [ ] 完整测试套件
- [ ] 性能基准测试
- [ ] 与现有系统的互操作性
- [ ] 文档完善

## 使用场景

### 1. 系统管理
- 软件包安装和维护
- 系统更新管理
- 依赖关系处理
- 安全补丁管理

### 2. 开发环境
- 开发依赖管理
- 构建环境配置
- 测试环境部署
- 持续集成支持

### 3. 生产环境
- 大规模部署
- 配置管理
- 合规审计
- 故障恢复

## 结论

PkgMgr包管理系统通过创新的架构设计和丰富的功能特性，为M4KK1操作系统提供了企业级的软件包管理解决方案。其独特的原子操作、沙箱机制和钩子系统等特性将重新定义包管理系统的可能性边界，为各种部署场景提供可靠、高效和安全的软件包管理体验。