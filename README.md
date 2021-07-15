# 1. 安装
**解压到addons文件夹即可,请确保安装了metamod平台**

# 2. 指令
## 2.1 最大支持客户端数(玩家+Bot+特感)(18 ~ 31)
`maxplayers <num>`  
**注意: 大于18可能导致部分地图报错(地图问题)**
## 2.2 最大玩家人数(-1~32)
`sv_maxplayers <num>`
## 2.3 动态移除大厅
`sv_unreserve`  
**注意: 不移除大厅会限制最大玩家数为4人**
## 2.4 允许部分作弊指令
`sv_allow_cheats 1`  
**注意: 无需将sv_cheats的值设为1,因多人原因其他不支持的指令就算解除限制也没有实际效果**  
提示以下信息表示该指令不支持  
`Can't use cheat command sb_takecontrol in multiplayer, unless the server has sv_cheats set to 1.`
