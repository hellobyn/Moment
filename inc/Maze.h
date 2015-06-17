
/********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**-------------------------------------------------------------------------------------------------------
** 	文件名：		Maze.h
** 	创建时间：		2014-4-2 20：15
** 	创建人员： 		赵秉贤
** 	文件描述:  		迷宫算法头文件
** 
**-------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-4-2 20：15 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		迷宫搜索测试
**
*********************************************************************************************************/

#include "stm32f10x.h"

#ifndef __MAZE_H
#define __MAZE_H

/*********************************************************************************************************
**	迷宫坐标结构体定义
*********************************************************************************************************/
struct mazecoor
{
    u8 X;
    u8 Y;
};
typedef struct mazecoor MAZECOOR;

/*********************************************************************************************************
**	电脑鼠属性结构体定义
*********************************************************************************************************/
struct micromouse
{
    u8 state;													/*	电脑鼠运行状态						*/
	u8 dir;														/*	电脑鼠运行绝对方向					*/
	u8 coorSwt;													/*	电脑鼠坐标更新标志					*/
	float angel;												/*	电脑鼠当前朝向绝对角度（暂时无用）	*/
	float vertD;												/*	电脑鼠在当前坐标格的位置			*/
};
typedef struct micromouse MICROMOUSE;
																/*	coorSwt 各赋值功能功能解释			**
																**	置0：两面有墙->任意一面无墙			**
																**	置0：左右转弯后						**
																**	置1：CoorUpdate之后					**
																**	置3：检测到路口但仍然直行			**
																**	0：左右有路时 更新坐标 跳出search	** 
																**	3：防止执行路口坐标二次更新			*/

/*********************************************************************************************************
**	电脑鼠运行状态定义
*********************************************************************************************************/
#define	WAIT	 			0									/*	等待运行开始命令					*/
#define	START		 		1									/*	开始运行							*/
#define	MAZESEARCH			2									/*	迷宫搜索							*/
#define	SPURT		 		3									/*	冲刺								*/

/*********************************************************************************************************
**	相对方向 -> 绝对方向上的迷宫挡板信息记录位
*********************************************************************************************************/
#define MOUSEWAY_F          (1 <<   mouse.dir)
#define MOUSEWAY_R          (1 << ((mouse.dir + 1) % 4))
#define MOUSEWAY_B          (1 << ((mouse.dir + 2) % 4))
#define MOUSEWAY_L          (1 << ((mouse.dir + 3) % 4))

/*********************************************************************************************************
**	电脑鼠变量宏定义
*********************************************************************************************************/
extern MICROMOUSE 	mouse;
extern u8 turnFlag;												/*	标志电脑鼠进入转弯状态				*/
extern u8 turnBackFlag;											/*	标志电脑鼠进入转弯状态				*/

/*********************************************************************************************************
**	电脑鼠运行--绝对方向定义
**	规定电脑鼠的初始运行方向--Y轴方向，为0度方向
*********************************************************************************************************/
#define	MAZEANGLE0		 	0									/*	迷宫0度方向							*/
#define	MAZEANGLE90		 	1									/*	迷宫90度方向						*/
#define	MAZEANGLE180	 	2									/*	迷宫180度方向						*/
#define	MAZEANGLE270	 	3									/*	迷宫270度方向						*/

/*********************************************************************************************************
**	电脑鼠运行--相对方向定义
**	规定电脑鼠的前方 为0度方向
*********************************************************************************************************/
#define	MOUSELEFT		 	0									/*	电脑鼠左方							*/
#define	MOUSEFRONT			1									/*	电脑鼠前方							*/
#define	MOUSERIGHT		 	2									/*	电脑鼠右方							*/

/*********************************************************************************************************
**	迷宫算法函数声明
*********************************************************************************************************/
void SearchGoAhead(void);
void CoorUpdate(void);
void WallCheck(void);
u8 CrossWayCheck(u8 mazeX, u8 mazeY);
void CrossWayChoice(void);
u8 MapBlockDataGet(u8 dirTemp);
void SpurtGoTo(u8 mazeX,u8 mazeY);
void DisCorrection(void);
void MapStepEdit(u8 mazeX,u8 mazeY);
void MouseGoStr(u8 strBlock);

#endif
