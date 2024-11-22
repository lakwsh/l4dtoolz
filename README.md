# L4DToolZ
### [English Version](https://github.com/lakwsh/l4dtoolz/blob/main/README_EN.md)
- 安装方法: [下载](https://github.com/lakwsh/l4dtoolz/actions/workflows/main.yml)并解压到addons文件夹
- **如使用tickrate解锁功能,请删除tickrate_enabler**
- 遇到任何问题请先在**服务器控制台**输入`plugin_print`确认扩展已经正确加载

## 1. 人数解锁
### 1.1 最大客户端数(即MaxClients)(18 ~ 32)
#### `sv_setmax <num>`
- 建议不要将最大值设置超过31,`The Last Stand`更新后会有崩溃问题
- **引擎默认值18,如需默认31客户端请在启动项加入`+sv_setmax 31`**
### 1.2 最大玩家数(-1~31)
#### `sv_maxplayers <num>`
- 服务器最多能进多少个玩家(设置为-1则不做修改)
### 1.3 禁止大厅匹配
#### `sv_force_unreserved <0/1>`(置1为禁止)
- 开启功能会将`sv_allow_lobby_connect_only`的值置0
- 开启本功能后服务器**不会处理**大厅匹配请求(也不会有大厅cookie)
### 1.4 获取/设置大厅cookie
#### `sv_cookie <cookie>`
- 通常不需要手动使用本指令,建议使用[动态大厅插件](https://github.com/lakwsh/l4d2_rmc)自动管理
- `cookie`为0即移除大厅,`sv_allow_lobby_connect_only`值自动置0
- `cookie`不为0时`sv_allow_lobby_connect_only`值自动置1
- **注意: 不移除大厅会限制最大玩家数为战役4人/对抗8人**

## 2. tickrate解锁
- 在启动项中添加`-tickrate <tick>`,不设置则不做修改
- **注意: 如通过plugin_load指令手动加载本扩展,可能出现tickrate异常问题**
### 2.1 相关CVar
- 需要修改(写到server.cfg,部分cvar需要sm_cvar前缀):
- `sv_minupdaterate`,`sv_maxupdaterate`,`sv_mincmdrate`,`sv_maxcmdrate`,
- `nb_update_frequency`,`sv_client_min_interp_ratio`,`fps_max`,
- `sv_client_max_interp_ratio`,`net_splitrate`,`net_splitpacket_maxrate`

## 3. 绕过SteamID验证
#### `sv_steam_bypass <0/1>`(置1为不验证SteamID)
- 本功能可以缓解`No Steam logon`(code 6)问题(仅限开启状态下进入的玩家)
- 开启本功能**会削弱服务器安全性**,且禁止家庭共享功能将失效
- **注意: 开启此功能会导致A2S_INFO结果异常,可以通过[插件](https://github.com/lakwsh/l4d2_vomit_fix)修复**

## 4. 禁止家庭共享(无依赖)
#### `sv_anti_sharing`(置1为开启功能)
- 开启本功能可以完全禁止家庭共享帐号(小号)进入服务器

## 5. 主要特色
### 5.1 更可靠
- 相比于原版l4dtoolz和tickrate_enabler完全不依赖签名
- 采用偏移方式寻址,游戏更新此扩展失效的几率低
- 重写大部分功能实现方式,大幅提高可靠性
### 5.2 MaxClients可动态修改
- 原版最大客户端数为固定值32(建议在服务器闲置状态下修改,否则可能崩溃)

## 6. 推荐插件
#### [配套纯净多人&动态大厅插件(可选)](https://github.com/lakwsh/l4d2_rmc)
- 功能: 自动移除大厅、允许投票设置最大玩家数
#### [Boomer喷吐距离修复插件](https://github.com/lakwsh/l4d2_vomit_fix)
- 功能: 修复高于30tick情况下对抗模式出现Boomer喷吐距离变短问题
- 该功能自2.2.4版本开始拆分为插件方式实现
