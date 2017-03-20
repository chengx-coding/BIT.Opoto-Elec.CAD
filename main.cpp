/**************************************************************************************************/
/*  Copyright (C) 2014-2016                                                                       */
/*                                                                                                */
/*  FILE NAME             :  main.c                                                               */
/*  PRINCIPAL AUTHOR      :  ChengXin                                                             */
/*  SUBSYSTEM NAME        :  CourseDesignCAD                                                      */
/*  MODULE NAME           :  CourseDesignCAD                                                      */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY                                                                  */
/*  DATE OF FIRST RELEASE :  2014/09/28                                                           */
/*  DESCRIPTION           :  This is the course design CAD in Opoto-electrical School of BIT      */
/**************************************************************************************************/

/*
 * 本程序初始版本完成于2014年5月
 *
 * 2016/10/29 更新：
 * 本程序所采用的graphics.h文件来自http://www.easyx.cn/downloads/View.aspx?id=4，为EasyX库(附带graphics.h) 测试版 (2015-10-15 更新)
 * 本程序构建代码和编译所用的IDE为Visual Studio 2015
 * 本程序中应用了大量的全局变量，这是一种很不好的编程习惯，当时初学无知。要修正的话基本上就要重构了。
 * 在画等位线的时候，个别情况下会有一两条线有部分错误，原因未知
 *
 * 2016/10/30 更新：
 * 优化了绘图界面
 *
 * 2016/10/31 更新：
 * 部分简单过程模块化
 *
 * 2016/11/01 更新：
 * 修改了求w_lmda的公式，在开方之内加入求绝对值，以应对特殊情况；
 * 修改了变量名以提升代码可读性：i-->a, j-->i, k-->j, row-->nRows, col-->nCols. 可能有部分注释未改
 *
 * 2016/11/05 更新：
 * 重写完成画等位线模块，解决不少BUG；
 * 模块化部分代码，制作了接口
 *
 * 2016/11/06 更新：
 * 模块化部分代码，制作了接口；
 * 增加了叠加法求电位空间的过程，并比较了绝对误差；
 * 优化了部分变量的数据结构
 *
 *
 *
 */

 /*
  * 注意
  * 输入数据和输出数据文件路径直接写入代码中，请将宏定义：FILEPATH、FILEIN、FILEOUT 设置为本地输入和输出文件路径
  * 程序编译运行即可显示等位线，回车继续可显示轴上电位分布，无标度。
  */

#pragma warning (disable:4996) //主要针对VS的warning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphics.h>
#include <conio.h>

#define FILEPATH "C:\\BIT\\CAD\\" //【可修改】定义输入数据和输出数据的文件路径
#define FILEIN "in.dat" //【可修改】定义输入数据文件名
#define FILEOUT "out.dat" //【可修改】定义输出数据文件名
#define DISPLAYLIMIT 600 //【可修改】定义绘图显示区域大小的限制
#define EDGEWIDTH 10 //【可修改】绘图区电位空间边界宽度
#define EXTRAREGION 160 //【可修改】绘图区额外区域宽度，可用来绘制图例
#define AXESGRID_X_NUM 15 //【可修改】绘制轴上点位曲线时坐标网格中x方向划分的刻度数量
#define AXESGRID_Y_NUM 10 //【可修改】绘制轴上点位曲线时坐标网格中y方向划分的刻度数量

#define FAILURE -1 //出错或失败的返回值
#define ACCURACY 1E-6 //设置精确度大小，用来比较两个double型数据是否相等。因为浮点型数据由于二进制存储方式问题，无法用==判断是否相等，只能认为二者小于某个精度即认为相等

  /* 网格坐标内某个点所在位置的状态标志 */
#define is_P 0 //在网格点上
#define is_V 1 //在垂直竖边上
#define is_H 2 //在水平横边上
#define is_H_T 3 //在上边界
#define is_H_B 4 //在下边界（电极也算边界）
#define is_V_L 5 //在左边界（电极也算边界）
#define is_V_R 6 //在右边界（电极也算边界）
#define is_P_T 7 //在上边界中的网格点上
#define is_H_E 8 //在电极顶边


/*
 *
 * c1, c2, c3, c4分别是SOR法中的系数(coefficient)，c0为4个系数之和
 * 电位空间中每个网格点都对应4个不同的系数，不同的网格点对应的系数不同
 * 每次计算一个网格点上的电位值，都需要用到这个点对应的四个不同系数
 * 关于这4个系数，有三种处理方法：
 * 第一种是每到一个网格点，都计算一次这个网格点对应的四个系数
 *     好处是只有4个double型变量，不需要额外的存储空间，不产生空间复杂度；
 *     坏处是迭代过程中每个网格点都需要计算多次，而每次系数相同，就会产生大量重复计算，增加时间复杂度
 * 第二种是预先计算好每个网格点对应的4个系数值，以及4个系数之和，并存入二维数组
 *     好处是避免了重复计算，减少了时间复杂度，而且代码便于理解；
 *     坏处是存储的系数中有大量重复，每个系数的二维数组都跟电位空间网格点的二维数组一样大，即占用的存储空间增加了5倍
 *         相对于网格点的数目，本程序中其他变量所占据的存储空间可忽略，故而相当于程序的空间复杂度增加了5倍
 * 第三种根据电极之间的间距和上下部分区域的长度的划分不同区域，每个区域内系数值相同
 *     好处是这样做，系数的总数量只跟电极数目有关，避免了存储空间的浪费，同时也节省了计算量，算是综合了前两种方法的优点；
 *     坏处是迭代计算时还要判断当前网格点位于哪个区域内，增加了一步理解难度
 * 本程序中采用的方法是第二种，即以空间换时间的方法
 * 之所以不用第三种，是因为c的取值情况也比较多，第二种更便于理解更不容易出错，就姑且浪费一些存储空间
 * 应该还有其他更优化的求系数方法，本程序采用的方法简单粗暴不值得提倡，只因时间仓促未再做优化
 *
 */

typedef struct Coefficient {
    double c1;
    double c2;
    double c3;
    double c4;
    double c0;
}tCoefficient;

tCoefficient **C;


/*
 * 结构类型point
 * ip和jp是电位空间内某个点(z,r)所在网格的左上角点的坐标，
 * status是这个点及其所在网格的状态，有7种：
 * 网格点上 is_P=0，竖边上 is_V=1，横边上 is_H=2，上边界 is_H_T=3，下边 is_H_B=4，左边界 is_V_L=5，右边界 is_V_R=6
 */
typedef struct PointType {
    int status;
    int ip;
    int jp;
    double z;
    double r;
}tPoint;

/* 由于输出文件要被许多函数访问并写入内容，因此声明为全局变量 */
FILE *data;
FILE *out;


/* 输入文件FILEIN中的数据 */
int n;//电极总数
double d;//电极厚度
double *z;//相邻电极之间的距离
int *step;//相邻电极之间要划分的步长数
double *v;//电极电位
double r1;//电极内孔径半径
int m1;//r1范围内等步长划分的网格数
double r2;//从电极内孔边到封闭边界处的径向距离
int m2;//r2范围内等步长划分的网格数
double e;//迭代控制精度
int nst;//输出打印空间电位时的网格点间格数
int ins;//轴上电位作等距插值时的步长数
double dv;//要求扫描搜索等位线的电位间隔值
int m;//要求扫描搜索等位线的电位值的个数
double *ev;//要求扫描搜索等位线的电位值

int nRows = 0;//电位空间电位二维数组的行数
int nCols = 0;//电位空间电位二维数组的列数
int *j_v;//电极靠左一边的j坐标值
double *z_v;//电极靠左一边的z坐标值


/******************************************** 声明函数 ***********************************************/
/* 从数据输入文件中初始化所有输入数据变量 */
double InitData();
/* 根据边界条件初始化电位空间，分配电位空间大小，以及为整个电位空间赋初值 */
int InitField(double **Field);
/* 只有单个电极存在时初始化电位空间，a表示第a个电极存在 */
int InitFieldSingleV(double **FieldSingleV, int a);
/* 向输出文件打印“输入数据”*/
int PrintData();
/* 向输出文件打印网格点坐标 */
int PrintGridCoordinates(double **Field);
/* 向输出文件打印网格上电位 */
int PrintGridValues(double **Field);
/* 向输出文件打印网格上电位绝对误差 */
int PrintGridValuesE(double **Field);

/* 计算除了电极位置网格点对应的系数C，EdgeClosure表示是否封闭上边界 */
int ComputeCwithoutV(int EdgeClosure);
/* 清零第a个电极位置网格点对应的系数C */
int CleanCofV(int a);

/* 计算电位空间中的电位值并向输出文件打印相关信息 */
int ComputeFieldValues(double **Field, int outputFlag);
/* 利用w参数对整个电位空间进行一次迭代计算 */
int iteration(double w, double **Field);

/* 将j坐标转换为z坐标 */
double jtoz(int j);
/* 将i坐标转换为r坐标 */
double itor(int i);
/* 返回电位值为EV的点的z坐标值 */
double evZ(double EV, double **Field, int i, int j);
/* 返回电位值为EV的点的r坐标值 */
double evR(double EV, double **Field, int i, int j);

/* 绘制电位空间 */
int DrawField(double height, double width, double asp, double vmax, double **Field, TCHAR *DrawFieldName);
/* 伪彩色函数 */
int PseudoColor(BYTE value, double x, double y, double HoriL, double VertiL);

/* 绘制等位线 */
int DrawEquLines(double height, double width, double asp, double **Field);
/* 点内容赋值，将p2[i2]的内容赋给p1[i1] */
int PointsEqu(tPoint *p1, int i1, tPoint *p2, int i2);
/* 在逐点连线绘制等位线的过程中对等位点的处理过程 */
int PointsProcessing(int nPoints, int status, tPoint *point, tPoint *ptemp, double EV, double **Field, int i, int j);
/* 判断输入参数(i, j)所代表的点是否在边界上 */
int isPointEdge(int i, int j);

/* 初始化拉格朗日插值法求轴上点位的绘制区域 */
int InitDrawLagrange(double height, double width, double asp, double vmax, double **Field);
/* 拉格朗日插值法计算轴上点位并绘制曲线 */
double *DrawLagrange(double *axis, double height, double width, double asp, double vmax, double **Field);
/* 判断某点在轴上的位置 */
int OnGrid_j(double x);


int main()
{
    int a;//一般用于循环时电极数计数，个别情况下也用于其它计数
    int i;//表示电位空间中的第i(start from 0)行
    int j;//表示电位空间中的第j(start from 0)列

    double vmax = 0;//整个电位空间电位的最大值
    vmax = InitData();//从数据输入文件中初始化所有输入数据变量，返回最大电位值

    /********************************* 计算一些关于电位空间的必要数据 *****************************/
    nRows = m1 + m2 + 1;//求总行数
    for (a = 0; a < n; a++)
        nCols = nCols + step[a];
    nCols = nCols + n;//求总列数

    /* 建立电位空间二维数组，并初始化电位为0 */
    double **Field;//电位空间电位（SOR迭代求得）
    Field = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        Field[i] = (double *)malloc(sizeof(double)*nCols);
        memset(Field[i], 0, sizeof(double)*nCols);
    }

    InitField(Field);//初始化电位空间

    /* 求各电极靠左一边的j坐标 */
    j_v = (int *)malloc(n * sizeof(int));
    for (a = 0, j_v[0] = 0; a < n; a++)
    {
        if (a == 0)
            j_v[a] = j_v[a] + step[a];
        else
            j_v[a] = j_v[a - 1] + step[a] + 1;
    }
    /* 求各电极靠左一边的z坐标 */
    z_v = (double *)malloc(n * sizeof(double));
    for (a = 0, z_v[0] = 0; a < n; a++)
    {
        if (a == 0)
            z_v[a] = z_v[a] + z[a];
        else
            z_v[a] = z_v[a - 1] + z[a] + d;
    }

    /* 二维系数结构体C，表示每个网格点上所对应的系数 */
    C = (tCoefficient **)malloc(sizeof(tCoefficient *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        C[i] = (tCoefficient *)malloc(sizeof(tCoefficient)*nCols);
        memset(C[i], 0, sizeof(tCoefficient)*nCols);
    }

    /* 计算C的值 */
    ComputeCwithoutV(1);//1为封闭上边界，0为开放上边界

    /**************************************** 初始化输出文件指针 **********************************/
    char outname[128] = FILEPATH;
    strcat(outname, FILEOUT);
    errno_t err2;
    err2 = fopen_s(&out, outname, "w");
    if (out == NULL || err2)
    {
        printf("无法写入文件：%s，请确认路径和文件名配置正确，或者该文件是否正在被使用！\n", outname);
        system("pause");
        exit(1);
    }
    else
    {
        printf("成功打开文件：%s\n", outname);
    }

    PrintData();//输出“输入数据”
    PrintGridCoordinates(Field);//输出网格点坐标

/**************************************** 计算电位空间中的电位值并向输出文件打印相关信息 **********************************/
    ComputeFieldValues(Field, 1);//计算电位空间中的电位值并向输出文件打印相关信息

    PrintGridValues(Field);//输出网格上电位

    printf("\n请按【任意键】继续，将显示【电位空间及等位线】绘制；\n再按【任意键】继续，将显示【轴上电位值曲线】绘制。\n\n");
    system("pause");

    /********************************** 开始画图 **********************************************/
    double width;//绘制电位空间宽度
    double height;//绘制电位空间高度

    double asp;//缩放因子。由于只需要放大电位空间而不需要放大等位线，因此不适用缩放函数，需手动调整缩放比例。
    for (a = 0, width = 0; a < n; a++)
    {
        width = width + z[a];
    }
    asp = DISPLAYLIMIT / (r1 + r2);//根据DISPLAYLIMIT对显示区域大小的限制决定缩放因子大小 
    width = (width + d*(n - 1))*asp;
    height = (r1 + r2)*asp;


    TCHAR DrawFieldName[50];//绘图名称
    _stprintf(DrawFieldName, _T("电位空间电位值伪彩色以及等位线绘制"));
    initgraph(width + 3 * EDGEWIDTH + EXTRAREGION, height + 6 * EDGEWIDTH, SHOWCONSOLE);//初始化绘图环境
    setorigin(2 * EDGEWIDTH, 4 * EDGEWIDTH);//原点设置为(10,10)，给左边和上边留出画边界的空间
    DrawField(height, width, asp, vmax, Field, DrawFieldName);//绘制电位空间

    system("pause");


    /************************************************ 画等位线 ***************************************************/
    fprintf(out, "\n\n\n打印等位线的电位值和各点坐标(z,r)\n");
    DrawEquLines(height, width, asp, Field);
    system("pause");

    closegraph();//关闭图形环境

    /************************************* 拉格朗日插值法算轴上电位 ********************************************/
    double *axis;//轴上电位（拉格朗日差值法求得）

    height = 0.8*height;
    initgraph((int)width + 10 * EDGEWIDTH, (int)height + 8 * EDGEWIDTH, SHOWCONSOLE);//初始化绘图环境
    setorigin(6 * EDGEWIDTH, 4 * EDGEWIDTH);

    InitDrawLagrange(height, width, asp, vmax, Field);//初始化绘制曲线坐标区域

    /* 计算并绘制轴上电位曲线 */
    axis = (double *)malloc((ins + 1) * sizeof(double));
    axis[0] = 0;
    axis[ins] = v[n - 1];
    axis = DrawLagrange(axis, height, width, asp, vmax, Field);//绘制曲线，返回轴上电位

    /* 向输出文件打印轴上点位值 */
    fprintf(out, "\n输出轴上电位值及其位置：\n\n");
    for (a = 0; a <= ins; a++)
        fprintf(out, "轴上位置: %lf \t电位值为: %lf\n", a*(width / asp) / ins, axis[a]);
    system("pause");
    closegraph();//关闭图形环境

    /***************************************** 直接法和叠加法的比较 ******************************************/

    fprintf(out, "\n\n********** 直接法与叠加法比较 **********\n");

    double **FieldSingleV;//只有一个电极存在时的电位空间分布
    FieldSingleV = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        FieldSingleV[i] = (double *)malloc(sizeof(double)*nCols);
        memset(FieldSingleV[i], 0, sizeof(double)*nCols);
    }

    double AE;//单个电位的绝对误差
    double MaxAE = 0;//最大绝对误差
    double MAE = 0;//平均误差
    double RMSE = 0;//均方根误差

    for (a = 0, width = 0; a < n; a++)
    {
        width = width + z[a];
    }
    width = (width + d*(n - 1))*asp;
    height = (r1 + r2)*asp;
    initgraph(width + 3 * EDGEWIDTH + EXTRAREGION, height + 6 * EDGEWIDTH, SHOWCONSOLE);//初始化绘图环境
    setorigin(2 * EDGEWIDTH, 4 * EDGEWIDTH);//原点设置为(10,10)，给左边和上边留出画边界的空间

    ComputeCwithoutV(0);//开放上边界

    for (a = 0; a < n; a++)
    {
        InitFieldSingleV(FieldSingleV, a);
        ComputeFieldValues(FieldSingleV, 0);
        //_stprintf(DrawFieldName, _T("第%d个电极V%d单独存在时的电位分布"), a, a + 1);
        //DrawField(height, width, asp, vmax, FieldSingleV, DrawFieldName);//绘制电位空间
        //system("pause");
        for (i = 0; i <= nRows - 1; i++)
        {
            for (j = 0; j <= nCols - 1; j++)
            {
                Field[i][j] = Field[i][j] - FieldSingleV[i][j];
            }
        }
    }
    for (i = 0; i <= nRows - 1; i++)
    {
        for (j = 0; j <= nCols - 1; j++)
        {
            Field[i][j] = fabs(Field[i][j]);
            AE = fabs(Field[i][j]);
            MaxAE = MaxAE > AE ? MaxAE : AE;
            MAE = MAE + AE;
            RMSE = RMSE + AE*AE;
        }
    }
    vmax = MaxAE;
    MAE = MAE / (nRows*nCols);
    RMSE = sqrt(RMSE / (nRows*nCols));
    PrintGridValuesE(Field);//向输出文件打印绝对误差
    fprintf(out, "\n\n统计结果：\n");
    fprintf(out, "【最大绝对误差】\t%lf\n", MaxAE);
    fprintf(out, "【平均绝对误差】\t%lf\n", MAE);
    fprintf(out, "【均方根误差】\t\t%lf\n", RMSE);

    clearcliprgn();
    _stprintf(DrawFieldName, _T("直接法与叠加法电位空间电位值（最大绝对误差为%lf V，如图例所示）"), MaxAE);
    DrawField(height, width, asp, vmax, Field, DrawFieldName);//绘制电位空间
    system("pause");
    closegraph();//关闭图形环境

    free(FieldSingleV);

    /***************************************** 数据写入完成,收尾 ***********************************/
    printf("对文件：%s 数据写入完成\n", outname);
    fclose(out);
    free(Field);//严格来说应该每一个malloc都对应一个free，防止内存泄漏
    free(C);
    *C = NULL;
    system("pause");

    return 0;
}



/********************************************** 定义函数 *********************************************************/
/* 从数据输入文件中初始化所有输入数据变量 */
double InitData()
{
    int a;
    char fname[128] = FILEPATH;
    strcat(fname, FILEIN);
    errno_t err1;
    err1 = fopen_s(&data, fname, "r");

    if (data == NULL || err1)
    {
        printf("无法打开文件：%s，请确认路径和文件名配置正确，或者该文件是否正在被使用！\n", fname);
        system("pause");
        exit(1);
    }
    else
    {
        printf("成功打开文件:%s\n", fname);
    }
    int datanum[15] = { 0 };//读入数据个数，若为0则说明未读入数据
    char *datastr[15] = {
        "n\t电极总数\n",
        "\nd\t电极厚度\n",
        "\nz\t相邻电极之间的距离\n",
        "\n\nstep\t相邻电极之间要划分的步长数\n",
        "\n\nv\t电极电位\n",
        "\n\nr1\t电极内孔径半径\n",
        "\nm1\tr1范围内等步长划分的网格数\n",
        "\nr2\t从电极内孔边到封闭边界处的径向距离\n",
        "\nm2\tr2范围内等步长划分的网格数\n",
        "\ne\t迭代控制精度\n",
        "\nnst\t输出打印空间电位时的网格点间格数\n",
        "\nins\t轴上电位作等距插值时的步长数\n",
        "\ndv\t要求扫描搜索等位线的电位间隔值\n",
        "\nm\t要求扫描搜索等位线的电位值的个数\n",
        "\nev\t要求扫描搜索等位线的电位值\n"
    };
    /* 为n赋值 */
    fscanf(data, datastr[0]);
    datanum[0] = fscanf(data, "%d\n", &n);
    /* 为d赋值 */
    fscanf(data, datastr[1]);
    datanum[1] = fscanf(data, "%lf\n", &d);
    /* 为z赋值 */
    z = (double *)malloc(n * sizeof(double));
    fscanf(data, datastr[2]);
    for (a = 0; a < n; a++)
        datanum[2] = fscanf(data, "%lf ", &z[a]) + datanum[2];
    if (datanum[2] != n)
        datanum[2] = 0;
    /* 为step赋值 */
    step = (int *)malloc(n * sizeof(int));
    fscanf(data, datastr[3]);
    for (a = 0; a < n; a++)
        datanum[3] = fscanf(data, "%d ", &step[a]) + datanum[3];
    if (datanum[3] != n)
        datanum[3] = 0;
    /* 为v赋值 */
    v = (double *)malloc(n * sizeof(double));
    fscanf(data, datastr[4]);
    for (a = 0; a < n; a++)
        datanum[4] = fscanf(data, "%lf ", &v[a]) + datanum[4];
    if (datanum[4] != n)
        datanum[4] = 0;
    /* 为r1赋值 */
    fscanf(data, datastr[5]);
    datanum[5] = fscanf(data, "%lf\n", &r1);
    /* 为m1赋值 */
    fscanf(data, datastr[6]);
    datanum[6] = fscanf(data, "%d\n", &m1);
    /* 为r2赋值 */
    fscanf(data, datastr[7]);
    datanum[7] = fscanf(data, "%lf\n", &r2);
    /* 为m2赋值 */
    fscanf(data, datastr[8]);
    datanum[8] = fscanf(data, "%d\n", &m2);
    /* 为e赋值 */
    fscanf(data, datastr[9]);
    datanum[9] = fscanf(data, "%lf\n", &e);
    /* 为nst赋值 */
    fscanf(data, datastr[10]);
    datanum[10] = fscanf(data, "%d\n", &nst);
    /* 为ins赋值 */
    fscanf(data, datastr[11]);
    datanum[11] = fscanf(data, "%d\n", &ins);
    /* 为dv赋值 */
    fscanf(data, datastr[12]);
    datanum[12] = fscanf(data, "%lf\n", &dv);
    /* 为m赋值 */
    fscanf(data, datastr[13]);
    datanum[13] = fscanf(data, "%d\n", &m);
    /* 为ev赋值 */
    if (m > 0) //m不为0时则ev有数据，直接输入ev的值
    {
        ev = (double *)malloc(m * sizeof(double));
        fscanf(data, datastr[14]);
        for (a = 0; a < m; a++)
            datanum[14] = fscanf(data, "%lf ", &ev[a]) + datanum[14];
        if (datanum[14] != m)
            datanum[14] = 0;
        printf("m大于零，按照给定的等位线的电位值进行扫描搜索\n");
    }

    /* 求整个电位空间电位的最大值，并作为返回值 */
    double vmax = 0;
    for (a = 0; a < n; a++)
    {
        if (v[a] > vmax)
            vmax = v[a];
    }

    if (m == 0)//m为0时则ev无数据，按电位间隔值dv计算ev的值
    {
        m = vmax / dv;
        datanum[14] = m;
        ev = (double *)malloc(m * sizeof(double));
        for (a = 0; a < m; a++)
            ev[a] = (a + 1)*dv;
        printf("m等于零，按照给定的等位线电位间隔值dv进行扫描搜索\n");
    }
    if (datanum[14] == 0)
        printf("Error: m值错误！\n");

    fclose(data);

    //考虑NULL的情况，即数据未正常读取
    if (&n == NULL || &d == NULL || z == NULL || step == NULL || v == NULL || &r1 == NULL || &m1 == NULL || &r2 == NULL || &m2 == NULL || &e == NULL || &nst == NULL || &ins == NULL)
    {
        printf("无法正常读取数据\n");
        system("pause");//暂停，在控制台上显示以上文字
        exit(1);
    }
    else
    {
        for (a = 0; a < 15; a++)
        {
            if (datanum[a] == 0)
            {
                printf("下列数据未能读取成功:\n");
                printf(datastr[14]);
                printf("\n请检查输入文件中该项数据以及其前一项数据的格式是否正确！\n");
                system("pause");
                exit(1);
            }
        }
        printf("成功读取文件：%s 中的数据\n", fname);
    }


    return vmax;
}

/* 根据边界条件初始化电位空间，分配电位空间大小，以及为整个电位空间赋初值 */
int InitField(double **Field)
{
    int a, i, j;
    /* 给电极赋值，以及用插值法给每个点赋初值 */
    int l;//用于两电极之间列数的计数
    double dif;//两个点之间的插值

    for (a = 0, j = 0; a < n; a++)
    {
        j = j + step[a] + 1;//第a+1个电极的靠右一列(除最后一个外电极用两列表示)
        for (i = 0; i < m2 + 1; i++)
        {
            if (j < nCols)
            {
                Field[i][j - 1] = v[a];
                Field[i][j] = Field[i][j - 1];//给除最后一个电极外每个电极赋值(最后一个电极用一列表示)
            }
            if (a == 0)
                dif = v[a] / step[a];
            else
                dif = (v[a] - v[a - 1]) / step[a];
            for (l = j - step[a]; l < j - 1; l++)//两电极间的区域(不包括电极)
            {
                Field[i][l] = Field[i][l - 1] + dif;
            }
        }
    }
    for (i = 0; i < nRows; i++)
        Field[i][nCols - 1] = v[n - 1];//给最后一个电极赋值

    return 0;
}

/* 只有单个电极存在时初始化电位空间，a表示第a个电极存在 */
int InitFieldSingleV(double **FieldSingleV, int a)
{
    int i, j;
    double dif;
    for (i = 0; i <= nRows - 1; i++)
    {
        for (j = 0; j <= nCols - 1; j++)
        {
            FieldSingleV[i][j] = 0;
        }
    }
    for (i = 0; i <= m2; i++)
    {
        FieldSingleV[i][j_v[a]] = v[a];
        dif = v[a] / z[a];
        for (j = (a == 0 ? 1 : (j_v[a - 1] + 2)); j < j_v[a]; j++)
        {
            FieldSingleV[i][j] = FieldSingleV[i][j - 1] + dif;
        }
        if (a != n - 1)
        {
            FieldSingleV[i][j_v[a] + 1] = v[a];
            dif = v[a] / z[a + 1];
            for (j = j_v[a] + 2; j < j_v[a + 1]; j++)
            {
                FieldSingleV[i][j] = FieldSingleV[i][j - 1] - dif;
            }
        }
    }
    if (a == n - 1)
    {
        dif = v[a] / (nCols - 1);
        for (i = m2 + 1; i <= nRows - 1; i++)
        {
            for (j = 1; j <= nCols - 2; j++)
            {
                FieldSingleV[i][j] = FieldSingleV[i][j - 1] + dif;
            }
        }
        j = nCols - 1;
        for (i = 0; i <= nRows - 1; i++)
        {
            FieldSingleV[i][j] = v[a];
        }
    }

    return 0;
}

/* 向输出文件打印“输入数据”*/
int PrintData()
{
    int a;
    fprintf(out, "电极总数n:");
    fprintf(out, "%d\n", n);
    fprintf(out, "电极厚度d:");
    fprintf(out, "%lf\n", d);
    fprintf(out, "相邻电极之间的距离z:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%lf ", z[a]);
    fprintf(out, "\n相邻电极之间要划分的步长数step:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%d ", step[a]);
    fprintf(out, "\n电极电位v:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%lf ", v[a]);
    fprintf(out, "\n电极内孔径半径r1:");
    fprintf(out, "%lf\n", r1);
    fprintf(out, "r1范围内等步长划分的网格数m1:");
    fprintf(out, "%d\n", m1);
    fprintf(out, "从电极内孔边到封闭边界处的径向距离r2:");
    fprintf(out, "%lf\n", r2);
    fprintf(out, "r2范围内等步长划分的网格数m2:");
    fprintf(out, "%d\n", m2);
    fprintf(out, "迭代控制精度e:");
    fprintf(out, "%lf\n", e);
    fprintf(out, "输出打印空间电位时的网格点间格数nst:");
    fprintf(out, "%d\n", nst);
    fprintf(out, "轴上电位作等距插值时的步长数ins:");
    fprintf(out, "%d\n", ins);
    if (m == 0)
    {
        fprintf(out, "要求扫描搜索等位线的电位间隔值dv:");
        fprintf(out, "%lf\n", dv);
    }
    else
    {
        fprintf(out, "要求扫描搜索等位线的电位值的个数m:");
        fprintf(out, "%d\n", m);
        fprintf(out, "要求扫描搜索等位线的电位值ev:\n");
        for (a = 0; a < m; a++)
            fprintf(out, "%lf ", ev[a]);
    }

    return 0;
}

/* 向输出文件打印网格点坐标 */
int PrintGridCoordinates(double **Field)
{
    int i, j;
    fprintf(out, "\n输出r方向的网格点坐标如下:\n");
    for (i = 0; i < nRows; i++)
    {
        fprintf(out, "第%d个点:\t", i);
        fprintf(out, "%lf\n", itor(i));
    }
    fprintf(out, "\n输出z方向的网格点坐标如下:\n");
    for (j = 0; j < nCols; j++)
    {
        fprintf(out, "第%d个点:\t", j);
        fprintf(out, "%lf\n", jtoz(j));
    }

    return 0;
}

/* 向输出文件打印网格上电位 */
int PrintGridValues(double **Field)
{
    int i, j;
    fprintf(out, "\n打印网格上的电位，间隔数NST为%d:\n", nst);
    for (i = 0; i < nRows; i = i + nst)
    {
        fprintf(out, "\n第【%d】行电位：", i);
        fprintf(out, "\n");
        for (j = 0; j < nCols; j = j + nst)
        {
            fprintf(out, "%lf \t", Field[i][j]);
        }
    }

    return 0;
}

/* 向输出文件打印网格上电位绝对误差 */
int PrintGridValuesE(double **Field)
{
    int i, j;
    fprintf(out, "\n打印网格上的直接法和叠加法的电位绝对误差，间隔数NST为%d:\n", nst);
    for (i = 0; i < nRows; i = i + nst)
    {
        fprintf(out, "\n第【%d】行绝对误差：", i);
        fprintf(out, "\n");
        for (j = 0; j < nCols; j = j + nst)
        {
            fprintf(out, "%lf \t", Field[i][j]);
        }
    }

    return 0;
}


/* 计算除了电极位置网格点对应的系数C，EdgeClosure表示是否封闭上边界 */
int ComputeCwithoutV(int EdgeClosure)
{
    int a, i, j;
    double h1, h2, h3, h4;
    for (i = 0; i <= nRows - 1; i++)
    {
        for (j = 0; j <= nCols - 1; j++)
        {
            C[i][j].c1 = 0;
            C[i][j].c2 = 0;
            C[i][j].c3 = 0;
            C[i][j].c4 = 0;
            C[i][j].c0 = 0;
        }
    }
    for (i = EdgeClosure; i <= nRows - 1; i++)
    {
        for (j = 1; j <= nCols - 1; j++)
        {
            h1 = jtoz(j) - jtoz(j - 1);
            h2 = jtoz(j + 1) - jtoz(j);
            h3 = i == (nRows - 1) ? 0 : itor(i) - itor(i + 1);
            h4 = i == 0 ? 0 : (itor(i - 1) - itor(i));
            C[i][j].c1 = 2 / (h1*(h1 + h2));
            C[i][j].c2 = 2 / (h2*(h1 + h2));
            C[i][j].c3 = i == (nRows - 1) ? 0 : (2 * itor(i) - h4) / (itor(i)*h3*(h3 + h4));
            if (i == (nRows - 1))
            {
                C[i][j].c4 = 4 / (h4*h4);
            }
            else
            {
                C[i][j].c4 = i == 0 ? 0 : ((2 * itor(i) + h3) / (itor(i)*h4*(h3 + h4)));
            }
            C[i][j].c0 = C[i][j].c1 + C[i][j].c2 + C[i][j].c3 + C[i][j].c4;
        }
    }

    for (a = 0; a < n - 1; a++)
    {
        CleanCofV(a);
    }

    return 0;
}

/* 清零第a个电极位置网格点对应的系数C */
int CleanCofV(int a)
{
    int i, j;
    double h1, h2, h3, h4;
    if (a == n - 1)
    {
        j = nCols - 1;
        for (i = 0; i <= nRows - 1; i++)
        {
            C[i][j].c1 = 0;
            C[i][j].c2 = 0;
            C[i][j].c3 = 0;
            C[i][j].c4 = 0;
            C[i][j].c0 = 0;
        }
    }
    else
    {
        for (i = 0; i <= m2; i++)
        {
            for (j = j_v[a]; j <= j_v[a] + 1; j++)
            {
                C[i][j].c1 = 0;
                C[i][j].c2 = 0;
                C[i][j].c3 = 0;
                C[i][j].c4 = 0;
                C[i][j].c0 = 0;
            }
        }
    }

    return 0;
}

/* 计算电位空间中的电位值并向输出文件打印相关信息 */
int ComputeFieldValues(double **Field, int outputFlag)
{
    int a, i, j;
    int num_w, num_f;//用于显示求w和求电位时迭代轮次
    num_w = 0;
    num_f = 0;

    double w_0, w_1, w_lmda, w_m, w_m_prior;//超张弛迭代因子
    double lmda;//拉姆达
    double u_lmda;
    double E_0, E_1;
    double Emax;//每轮最大残差

    double **Field_Prior;//前一次运算得到的电位空间电位

    Field_Prior = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
        Field_Prior[i] = (double *)malloc(sizeof(double)*nCols);
    /* 计算超张弛迭代因子w最佳值的估计值 */
    w_m_prior = 0;
    w_0 = 1;
    iteration(w_0, Field);//迭代
    w_1 = 1.375;//为w_1赋初值
    int count;//迭代计次
    while (1)
    {
        num_w++;//迭代轮次计数
        for (count = 1; count <= 10; count++)
            iteration(w_1, Field);//迭代10次

        /* 令Field_Prior等于迭代第10次的Field */
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }

        iteration(w_1, Field);//迭代第11次

        /* 求第11次迭代后电位值与第10次迭代后电位值的残差的平均值 */
        E_0 = 0;//残差平均值清零
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                E_0 = E_0 + fabs(Field[i][j] - Field_Prior[i][j]);
            }
        }
        E_0 = E_0 / (nRows*nCols);

        /* 令Field_Prior等于迭代第11次的Field */
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }

        iteration(w_1, Field);//迭代第12次

        /* 求第11次迭代后电位值与第12次迭代后电位值的残差的平均值 */
        Emax = 0;
        E_1 = 0;//残差平均值清零
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                if (fabs(Field[i][j] - Field_Prior[i][j]) > Emax)
                {
                    Emax = fabs(Field[i][j] - Field_Prior[i][j]);
                }
                E_1 = E_1 + fabs(Field[i][j] - Field_Prior[i][j]);
            }
        }
        E_1 = E_1 / (nRows*nCols);

        lmda = E_1 / E_0;
        u_lmda = (lmda + w_1 - 1) / (sqrt(lmda)*w_1);
        w_lmda = 2 / (1 + sqrt(fabs(1 - (u_lmda*u_lmda))));//这里我多加了一个取绝对值，不然有些情况下会出错
        w_m = 1.25*w_lmda - 0.5;

        if (outputFlag)
        {
            fprintf(out, "\n第%d轮迭代：\n", num_w);
            fprintf(out, "\t迭代次数：%d\n", 12 * num_w);
            fprintf(out, "\t迭代因子：%lf\n", w_m);
            fprintf(out, "\t平均残差：%lf\n", E_1);
            fprintf(out, "\t最大残差：%lf\n", Emax);
        }

        if (!(w_m == w_m))//如果数据溢出导致w_m不是一个数，那么w_m == w_m的结果是FALSE
        {
            printf("Error: 迭代因子异常！\n");
            fclose(out);
            system("pause");
            exit(1);
        }

        if (fabs((w_m - w_m_prior) / (2 - w_m)) < 0.05)
            break;
        else
        {
            w_m_prior = w_m;
            w_1 = w_m;
        }
    }

    /* 用最佳w值进行迭代，计算电位 */
    int e_flag;//用于判断迭代是否满足精度要求
    while (1)
    {
        num_f++;
        //令Field_Prior等于前一次迭代所得的Field
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }
        iteration(w_m, Field);

        Emax = 0;
        E_1 = 0;//残差平均值清零
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                if (fabs(Field[i][j] - Field_Prior[i][j]) > Emax)
                {
                    Emax = fabs(Field[i][j] - Field_Prior[i][j]);
                }
                E_1 = E_1 + fabs(Field[i][j] - Field_Prior[i][j]);
            }
        }
        E_1 = E_1 / (nRows*nCols);//残差

        if (outputFlag)
        {
            fprintf(out, "\n第%d轮迭代：\n", num_w + num_f);
            fprintf(out, "\t迭代次数：%d\n", 12 * num_w + num_f);
            fprintf(out, "\t迭代因子：%lf\n", w_m);
            fprintf(out, "\t平均残差：%lf\n", E_1);
            fprintf(out, "\t最大残差：%lf\n", Emax);
        }

        e_flag = 0;
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                if (fabs(Field[i][j] - Field_Prior[i][j]) < e)//判读改点是否满足迭代控制精度
                    e_flag++;
            }
        }
        if (e_flag == nRows*nCols)
            break;
    }


    return 0;
}

/* 利用w参数对整个电位空间进行一次迭代计算 */
int iteration(double w, double **Field)
{
    int a, i, j;
    for (i = nRows - 1; i >= 0; i--)
    {
        for (j = 1; j < nCols - 1; j++)
        {
            if (fabs(C[i][j].c0) > ACCURACY)
            {
                if (i == 0)
                {
                    Field[i][j] = (1 - w)*Field[i][j] + w*(C[i][j].c1 * Field[i][j - 1] + C[i][j].c2 * Field[i][j + 1] + C[i][j].c3 * Field[i + 1][j]) / C[i][j].c0;
                }
                else if (i == nRows - 1)
                {
                    Field[i][j] = (1 - w)*Field[i][j] + w*(C[i][j].c1 * Field[i][j - 1] + C[i][j].c2 * Field[i][j + 1] + C[i][j].c4 * Field[i - 1][j]) / C[i][j].c0;
                }
                else
                {
                    Field[i][j] = (1 - w)*Field[i][j] + w*(C[i][j].c1 * Field[i][j - 1] + C[i][j].c2 * Field[i][j + 1] + C[i][j].c3 * Field[i + 1][j] + C[i][j].c4 * Field[i - 1][j]) / C[i][j].c0;
                }
            }
        }
    }
    return 0;
}


/*
 * 将j坐标转换为z坐标
 * 输入参数为网格点横坐标（列数）j，返回值为z方向的距离坐标值
 */
double jtoz(int j)
{
    int a;
    double z_j = 0;//z坐标值

    for (a = 0; a <= n - 1; a++)
    {
        if (j == 0)
        {
            z_j = 0;
            break;
        }
        if (j >= j_v[a] - step[a] && j <= j_v[a])
        {
            z_j = z_v[a] - (j_v[a] - j)*(z[a] / step[a]);
        }
    }
    return z_j;
}

/*
 * 将i坐标转换为r坐标
 * 输入参数为网格点纵坐标（行数）i，返回值为r方向的距离坐标值
 */
double itor(int i)
{
    double r_i;//r坐标值
    if (i <= m2)
        r_i = r1 + r2 - i*(r2 / m2);
    else
        r_i = r1 - (i - m2)*(r1 / m1);
    return r_i;
}

/* 返回电位值为EV的点的z坐标值 */
double evZ(double EV, double **Field, int i, int j)
{
    if (fabs(Field[i][j + 1] - Field[i][j]) < ACCURACY)
    {
        return jtoz(j);
    }
    else if ((EV - Field[i][j]) / (Field[i][j + 1] - Field[i][j]) >= 0 && ((EV<Field[i][j + 1] && EV>Field[i][j]) || (EV > Field[i][j + 1] && EV < Field[i][j]) || (fabs(EV - Field[i][j]) < ACCURACY)))
    {
        return(jtoz(j) + ((EV - Field[i][j]) / (Field[i][j + 1] - Field[i][j]))*(jtoz(j + 1) - jtoz(j)));
    }
    else
    {
        return FAILURE;
    }
}

/* 返回电位值为EV的点的r坐标值 */
double evR(double EV, double **Field, int i, int j)
{
    if (fabs(Field[i][j] - Field[i + 1][j]) < ACCURACY)
    {
        return itor(i);
    }
    //判断EV是否在Field[i + 1][j]和Field[i][j]之间
    else if ((EV - Field[i + 1][j]) / (Field[i][j] - Field[i + 1][j]) >= 0 && ((EV<Field[i + 1][j] && EV>Field[i][j]) || (EV > Field[i + 1][j] && EV < Field[i][j]) || (fabs(EV - Field[i][j]) < ACCURACY)))
    {
        return(itor(i + 1) + ((EV - Field[i + 1][j]) / (Field[i][j] - Field[i + 1][j]))*(itor(i) - itor(i + 1)));
    }
    else
    {
        return FAILURE;
    }
}



/* 绘制电位空间 */
int DrawField(double height, double width, double asp, double vmax, double **Field, TCHAR *DrawFieldName)
{
    outtextxy((int)(0.5*width - 0.5*textwidth(DrawFieldName)), (int)(-3 * EDGEWIDTH), DrawFieldName);
    int a, i, j;
    setfillstyle(BS_HATCHED, HS_DIAGCROSS);
    solidrectangle(-EDGEWIDTH, -EDGEWIDTH, width + EDGEWIDTH, 0);//绘制上边界
    setfillstyle(BS_HATCHED, HS_BDIAGONAL);
    solidrectangle(-EDGEWIDTH, 0, 0, height + EDGEWIDTH);//绘制最左边0V阴极
    setfillstyle(BS_HATCHED, HS_HORIZONTAL);
    solidrectangle(width, 0, width + EDGEWIDTH, height + EDGEWIDTH);//绘制最右边荧光屏
    setfillstyle(BS_HATCHED, HS_VERTICAL);
    solidrectangle(0, height, width, height + EDGEWIDTH);//绘制z轴
    setfillstyle(BS_SOLID);//填充模式恢复默认

    /* 绘制伪彩色电位分布 */
    BYTE GrayValue;
    for (i = 0; i < nRows - 1; i++)
    {
        for (j = 0; j < nCols - 1; j++)
        {
            GrayValue = Field[i][j] / vmax * 255;
            PseudoColor(GrayValue, jtoz(j)*asp, height - itor(i)*asp, (jtoz(j + 1) - jtoz(j))*asp, (itor(i) - itor(i + 1))*asp);
        }
    }
    /* 绘制伪彩色电位分布的图例 */
    double legendsize = 0.8*height / 255;//用于控制图例长度，每个颜色长度为legendsize，总长度为0.8*height
    double legendwidth = EXTRAREGION > 100 ? 40 : 0.4*EXTRAREGION;
    for (a = 0; a < 256; a++)
    {
        PseudoColor(a, width + 0.2*EXTRAREGION, 0.1*height + (255 - a) * legendsize, legendwidth, legendsize);
    }
    TCHAR strlg01[20], strlg02[20], strlg03[20], strlg04[20], strlg05[20];
    _stprintf(strlg01, _T("%lf V"), 0);
    _stprintf(strlg02, _T("%lf V"), 0.25*vmax);
    _stprintf(strlg03, _T("%lf V"), 0.5*vmax);
    _stprintf(strlg04, _T("%lf V"), 0.75*vmax);
    _stprintf(strlg05, _T("%lf V"), vmax);
    outtextxy((int)(width + 0.2*EXTRAREGION + legendwidth + 0.1*legendwidth), (int)(0.9*height - 0.5*textheight(strlg01)), strlg01);
    outtextxy((int)(width + 0.2*EXTRAREGION + legendwidth + 0.1*legendwidth), (int)((0.9 - 0.8*0.25)*height - 0.5*textheight(strlg01)), strlg02);
    outtextxy((int)(width + 0.2*EXTRAREGION + legendwidth + 0.1*legendwidth), (int)((0.9 - 0.8*0.50)*height - 0.5*textheight(strlg01)), strlg03);
    outtextxy((int)(width + 0.2*EXTRAREGION + legendwidth + 0.1*legendwidth), (int)((0.9 - 0.8*0.75)*height - 0.5*textheight(strlg01)), strlg04);
    outtextxy((int)(width + 0.2*EXTRAREGION + legendwidth + 0.1*legendwidth), (int)(0.1*height - 0.5*textheight(strlg01)), strlg05);
    line(width + 0.2*EXTRAREGION + 0.5*legendwidth, (0.9 - 0.8*0.25)*height, width + 0.2*EXTRAREGION + legendwidth, (0.9 - 0.8*0.25)*height);
    line(width + 0.2*EXTRAREGION + 0.5*legendwidth, (0.9 - 0.8*0.50)*height, width + 0.2*EXTRAREGION + legendwidth, (0.9 - 0.8*0.50)*height);
    line(width + 0.2*EXTRAREGION + 0.5*legendwidth, (0.9 - 0.8*0.75)*height, width + 0.2*EXTRAREGION + legendwidth, (0.9 - 0.8*0.75)*height);

    /* 绘制电极,【solidrectangle】函数在旧版<graphics.h>中应改为【bar】 */
    setfillcolor(WHITE);//填充颜色白色
    for (a = 0; a < n - 1; a++)
        solidrectangle(z_v[a] * asp, 0, (z_v[a] + d)*asp, r2*asp);

    return 0;
}

/*
 * 伪彩色函数
 * 根据灰度值为一个矩形区域画出伪彩色
 * value: 灰度值
 * HoriL, VertiL: 矩形水平和竖直边长
 * x, y: 矩形区域坐标
 */
int PseudoColor(BYTE value, double x, double y, double HoriL, double VertiL)
{
    BYTE r, g, b;
    r = value;
    g = value < 128 ? 2 * value : (255 - 2 * value);
    b = 255 - value;
    setfillcolor(RGB(r, g, b));
    solidrectangle(x, y, x + HoriL, y + VertiL);

    return 0;
}

/* 绘制等位线 */
int DrawEquLines(double height, double width, double asp, double **Field)
{
    /*
     * 流程：
     * (除了连线时之外，其余过程均使用网格点坐标（i，j）体系)
     * 1. 获取当前需要扫描的电位值EV
     * 2. 扫描电位空间第0行，找到EV，记录其位置i，j
     *    如果在电极上，跳到电极开始位。
     *    如果逐点连电位空间线，电位空间线画完之后回到第0行，那么从这个回来点开始继续往后搜索；
     *    如果如果没有回来的点，那么连完电位空间线之后直接继续往后搜索。
     * 3. 判断当前一个Point的状态，然后对对应的状态进行扫描。上一个点是什么，下一个就不用扫什么。
     * 4. 若扫描到的点就是上一个点，则放弃该点，则继续扫描。记录下扫描到的点。
     * 5. 一般情况下扫描到1个点，直接与前一个点连线；
     *    若扫到2个点，进行判断连线
     *    若扫到3个及以上点
     * 6. 一直连线到遇到边界或是电极，结束当前连线
     * 7. 继续在第0行搜索等于EV的点，直到把第0行搜索完。
     * 8. 换下一个EV继续搜索。
     */
    int a, i, j;
    double EV;//当前待扫描的电位值
    int jofi0 = 1;//第0行中的j，用于扫描第0行中的电位

    //0为前一个扫描到的点，1为当前扫描到的点，2为下一个点
    tPoint point[] = { { 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 } };
    //特殊情况下额外的点,0为额外的前一个点，1为暂存，2为额外的下一个点
    tPoint ptemp[] = { { 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 } };
    tPoint pzero[] = { { 0,0,0,0,0 } };//专门用于重置归零
    int flagget = 0;//对第0行的当前扫描，扫描到点的标志
    int flagi0get = 0;//对整个第0行的全部次数扫描，扫描到点的标志
    int abegin = 0;//从第abegin个电极开始扫描
    int CountV = 0;
    int DrawLine = 1;
    int nPoints = 0;//中心点周边扫描到的点计数

    for (CountV = 0; CountV < m; CountV++)//扫描第CountV个电位值并绘制等位线
    {
        EV = ev[CountV];
        fprintf(out, "\n电位值为%.2lf时：\n", EV);

        jofi0 = 1;//从1开始，忽略第0列边界
        flagi0get = 0;//整个第0行扫描标志归零
        abegin = -1;//电极扫描起始位置重置
        while (jofi0 <= nCols - 2)//第0行从1一直扫描到nCols-2
        {
            flagget = 0;
            i = 0;
            j = 0;
            if (abegin != n - 2) //如果对n-1个电极的扫描没到头
            {
                //先扫描各个电极，确定要扫描的点是否在电极上。忽略第n-1个电极（边界）
                for (a = abegin + 1; a <= n - 2; a++)
                {
                    //如果当前电极上扫描到点
                    if (fabs(Field[0][jofi0] - EV) < ACCURACY && (jofi0 == j_v[a] || jofi0 == (j_v[a] + 1)))
                    {
                        point[1].status = is_H_E;
                        point[1].ip = m2;
                        point[1].jp = jofi0;
                        point[1].z = evZ(EV, Field, m2, jofi0) + 0.5*d;
                        point[1].r = itor(m2);
                        flagget = 1;
                        abegin = a;
                        jofi0++;//每个电极占据两个网格点，如果点在电极上，那么下次扫描就可以跳过一个网格点
                        break;
                    }
                }//for(a = 0; a <= n - 2; a++)扫描电极
            }//if (abegin != n-2)

            if (flagget == 0) //如果点不在电极上
            {
                //如果当前非电极的上边界位置扫描到点
                if (fabs(Field[0][jofi0] - EV) < ACCURACY || (Field[0][jofi0]<EV&&Field[0][jofi0 + 1]>EV) || (Field[0][jofi0] > EV&&Field[0][jofi0 + 1] < EV))
                {
                    if (fabs(Field[0][jofi0] - EV) < ACCURACY)
                        point[1].status = is_P_T;
                    else
                        point[1].status = is_H_T;
                    point[1].ip = 0;
                    point[1].jp = jofi0;
                    point[1].z = evZ(EV, Field, 0, jofi0);
                    point[1].r = itor(0);
                    flagget = 1;
                }
            }

            flagi0get = flagi0get + flagget;//每扫描完一轮记一次数

            if (flagget)
            {
                fprintf(out, "\n扫描%.2lf V电位时从(z, r)坐标为(%lf, %lf)、(i, j)坐标为(%d, %d)开始的等位线：\n", EV, point[1].z, point[1].r, point[1].ip, point[1].jp);

            }

            /* 开始逐点扫描并绘制等位线 */
            DrawLine = 1;
            while (DrawLine == 1 && flagget == 1)//第0行扫到点且满足画线标志时画线
            {
                if (fabs(point[1].r - FAILURE) < ACCURACY || fabs(point[1].z - FAILURE) < ACCURACY)
                {
                    printf("Error：当前(i, j)坐标为(%d, %d)的点的(z, r)坐标计算不正确，程序存在BUG，退出\n", point[1].ip, point[1].jp);
                    system("pause");
                    exit(1);
                }

                nPoints = 0;//中心点周边扫描到的点重置
                switch (point[1].status)//根据当前点的状态来进行扫描
                {
                case is_P:
                    //扫描两个列
                    for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                    {
                        for (i = point[1].ip - 1; i <= point[1].ip; i++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //扫描两个行
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i += 2)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //扫描四边上所有点
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j++)
                        {
                            if (i == point[1].ip&&j == point[1].jp)
                            {
                                continue;
                            }
                            else
                            {
                                nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                            }
                        }
                    }
                    break;
                case is_V:
                    //扫描两个列
                    i = point[1].ip;
                    for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                    {
                        nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                    }
                    //扫描两个行
                    for (i = point[1].ip; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //扫描四边上所有点
                    for (i = point[1].ip; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H:
                    //扫描两个列
                    for (j = point[1].jp; j <= point[1].jp + 1; j++)
                    {
                        for (i = point[1].ip - 1; i <= point[1].ip; i++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //扫描两个行
                    j = point[1].jp;
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i += 2)
                    {
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                    }
                    //扫描四边上所有点
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_P_T:
                    if (nPoints)//如果扫描过的点不为0，说明这个边界点是终点而不是起点
                    {
                        DrawLine = 0;//当点在边界上时，说明电位空间线逐点连线结束
                        break;
                    }
                    else //如果恰好从第0行某个网格点开始
                    {
                        //扫描两个列
                        i = point[1].ip;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //扫描一个行
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                        //扫描底边三个点
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H_T:
                    if (nPoints)//如果扫描过的点不为0，说明这个边界点是终点而不是起点
                    {
                        DrawLine = 0;//当点在边界上时，说明电位空间线逐点连线结束
                        break;
                    }
                    else //如果从第0行网格线或某个电极顶边开始
                    {
                        //扫描两个列
                        i = point[1].ip;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //扫描一个行
                        i = point[1].ip + 1;
                        j = point[1].jp;
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        //扫描底角两个点
                        i = point[1].ip + 1;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H_E:
                    if (nPoints)//如果扫描过的点不为0，说明这个边界点是终点而不是起点
                    {
                        DrawLine = 0;//当点在边界上时，说明电位空间线逐点连线结束
                        break;
                    }
                    else //如果从第0行网格线或某个电极顶边开始
                    {
                        //扫描两个列
                        i = point[1].ip;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //扫描一个行
                        i = point[1].ip + 1;
                        j = point[1].jp;
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        //扫描底角两个点
                        i = point[1].ip + 1;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    if (nPoints == 0)//如果最接近电极顶点的位置找不到等位点，扩大一圈范围
                    {
                        //扫描两个列
                        i = point[1].ip;
                        for (j = point[1].jp - 1; j <= point[1].jp + 2; j += 3)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //扫描一个行
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                        //扫描四个点
                        for (i = point[1].ip; i <= point[1].ip + 1; i++)
                        {
                            for (j = point[1].jp - 1; j <= point[1].jp + 2; j += 3)
                            {
                                nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                            }
                        }
                    }
                    break;
                case is_H_B:
                case is_V_L:
                case is_V_R:
                    DrawLine = 0;//左、右、下边界点不可能是起点，只可能是终点
                    fprintf(out, "(z, r)坐标为：(%lf, %lf)\t(i, j)坐标为：(%d, %d)【终点】\n", point[1].z, point[1].r, point[1].ip, point[1].jp);
                    PointsEqu(point, 0, pzero, 0);//暂存的“上一个点”清零
                    PointsEqu(ptemp, 0, pzero, 0);
                    break;
                default:
                    printf("Error：Point(%d, %d)的status错误\n", point[1].ip, point[1].jp);
                    break;
                }//switch

                if (DrawLine)//可以连线的时候再连线
                {
                    /* 根据扫描到的点的数量进行连线 */
                    if (nPoints >= 3)
                    {
                        printf("扫描电位%.2fV时在(i, j)坐标为(%d, %d)的位置附近遇到%d个等位点，疑似遇到等势区\n", EV, point[1].ip, point[1].jp, nPoints + 2);
                        system("pause");
                        break;
                    }
                    else if (nPoints == 2)
                    {
                        printf("遇到一次同一个网格内有三个等位点的情况\n");
                        if (ptemp[2].ip > point[2].ip)
                        {
                            PointsEqu(ptemp, 1, point, 2);
                            PointsEqu(point, 2, ptemp, 2);
                        }
                        else
                        {
                            PointsEqu(ptemp, 1, ptemp, 2);
                        }

                        fprintf(out, "(z, r)坐标为：(%lf, %lf)\t(i, j)坐标为：(%d, %d)\n", point[1].z, point[1].r, point[1].ip, point[1].jp);
                        fprintf(out, "(z, r)坐标为：(%lf, %lf)\t(i, j)坐标为：(%d, %d)\n", point[2].z, point[2].r, point[2].ip, point[2].jp);

                        line(point[1].z*asp, height - point[1].r*asp, point[2].z*asp, height - point[2].r*asp);
                        line(point[2].z*asp, height - point[2].r*asp, ptemp[1].z*asp, height - ptemp[1].r*asp);
                        PointsEqu(point, 0, point, 1);
                        PointsEqu(ptemp, 0, point, 2);
                        PointsEqu(point, 1, ptemp, 1);
                    }
                    else if (nPoints == 1)
                    {
                        fprintf(out, "(z, r)坐标为：(%lf, %lf)\t(i, j)坐标为：(%d, %d)\n", point[1].z, point[1].r, point[1].ip, point[1].jp);

                        line(point[1].z*asp, height - point[1].r*asp, point[2].z*asp, height - point[2].r*asp);
                        PointsEqu(point, 0, point, 1);
                        PointsEqu(point, 1, point, 2);
                    }
                    else if (nPoints == 0 && DrawLine != 0)//没有点，且不为退出判断
                    {
                        DrawLine = 0;
                        printf("扫描电位%.2fV时在(i, j)坐标为(%d, %d)的位置附近没有下一个等位点\n", EV, point[1].ip, point[1].jp);
                        break;
                    }
                }//if(DrawLine)让连线的时候才连线
            }//while(DrawLine==1&&flagget==1)


            PointsEqu(point, 0, pzero, 0);//画完一条等位线，重置归零
            PointsEqu(ptemp, 0, pzero, 0);

            if (flagget == 1 && (point[1].status == is_H_T || point[1].status == is_H_E || point[1].status == is_P_T) && point[1].jp > jofi0)//如果画的等位线最后回到第0行
            {
                jofi0 = point[1].jp;
            }

            if (jofi0 >= nCols - 2 && flagi0get == 0 && fabs(EV - v[n - 1]) > ACCURACY)
            {
                printf("没有找到电位为%lf的点，无法绘制该电位的电位空间线\n", EV);//没找到，则换下一个电位继续扫描

            }

            jofi0++;//连完线接着扫描第0行查找有无另外的点也等于EV，直到第0行所有点都扫描完
        }//while(jofi0<=nCols-2)
    }//for(CountV)

    return 0;
}

/* 在逐点连线绘制等位线的过程中对等位点的处理过程 */
int PointsProcessing(int nPoints, int status, tPoint *point, tPoint *ptemp, double EV, double **Field, int i, int j)
{
    //如果成功扫描到，结果暂存到ptemp[1]中，再进行判断
    if ((status == is_V && (((Field[i][j] - EV > ACCURACY) && (Field[i + 1][j] - EV < -ACCURACY)) || ((Field[i][j] - EV < -ACCURACY) && (Field[i + 1][j] - EV > ACCURACY)))) || (status == is_H && (((Field[i][j] - EV > ACCURACY) && (Field[i][j + 1] - EV < -ACCURACY)) || ((Field[i][j] - EV < -ACCURACY) && (Field[i][j + 1] - EV > ACCURACY)))) || (status == is_P && (fabs(Field[i][j] - EV) < ACCURACY)))
    {
        ptemp[1].status = isPointEdge(i, j);
        if (ptemp[1].status == FAILURE)
        {
            printf("Error：搜索(%d, %d)附近的等位点时遇到status错误\n", i, j);
        }
        else if (ptemp[1].status == 0)//不在边界上
        {
            ptemp[1].status = status;
        }
        else if ((ptemp[1].status == is_H_T&&status == is_V) || (ptemp[1].status == is_H_E&&status == is_V) || (ptemp[1].status == is_V_L&&status == is_H))
        {
            ptemp[1].status = status;
        }
        else if (ptemp[1].status == is_H_T&&status == is_P)
        {
            ptemp[1].status = is_P_T;
        }

        ptemp[1].ip = i;
        ptemp[1].jp = j;

        //电极右下角的点的状态单独判断
        if (ptemp[1].ip == m2&&ptemp[1].status == is_V_L&&ptemp[1].jp != 0)
        {
            ptemp[1].status = is_H_E;
        }


        //如果下一个点与前一个点（两种情况）相同（比较一个点内ip,jp,status三个成员）
        if (((point[0].ip == ptemp[1].ip) && (point[0].jp == ptemp[1].jp) && (point[0].status == ptemp[1].status)) || ((ptemp[0].ip == ptemp[1].ip) && (ptemp[0].jp == ptemp[1].jp) && (ptemp[0].status == ptemp[1].status)))
        {
            return nPoints;
        }
        //电极顶边上的点情况特殊，需要再单独比较,只要点在同一个电极顶边上，不论位置如何都视作同一个点
        else if ((point[0].status == is_H_E) && (ptemp[1].status == is_H_E))
        {
            if ((ptemp[1].jp - point[0].jp >= -1) && (ptemp[1].jp - point[0].jp <= 1))
            {
                return nPoints;
            }
        }
        else if (nPoints >= 2)
        {
            nPoints++;
        }
        else if (nPoints == 0)//如果这是扫描到的第一个等位点，则将ptemp[1]存入point[2]
        {
            point[2].status = ptemp[1].status;
            point[2].ip = ptemp[1].ip;
            point[2].jp = ptemp[1].jp;
            if (status == is_P || status == is_P_T || status == is_V_L || status == is_V_R)
            {
                point[2].z = jtoz(j);
                point[2].r = itor(i);
            }
            else if (status == is_V)
            {
                point[2].z = jtoz(j);
                point[2].r = evR(EV, Field, i, j);
            }
            else if (status == is_H || status == is_H_B || status == is_H_T)
            {
                point[2].z = evZ(EV, Field, i, j);
                point[2].r = itor(i);
            }
            else if (status == is_H_E)
            {
                point[2].z = evZ(EV, Field, i, j) + 0.5*d;
                point[2].r = itor(i);
            }
            else
            {
                printf("Something wrong in Function: PointsProcessing\n");
                system("pause");
                return nPoints;
            }

            nPoints++;
        }
        else if (nPoints == 1)//若已经扫描到一个点，那么再次扫描到的点存到ptemp[2]
        {
            ptemp[2].status = ptemp[1].status;
            ptemp[2].ip = ptemp[1].ip;
            ptemp[2].jp = ptemp[1].jp;
            if (status == is_P)
            {
                ptemp[2].z = jtoz(j);
                ptemp[2].r = itor(i);
            }
            else if (status == is_V)
            {
                ptemp[2].z = jtoz(j);
                ptemp[2].r = evR(EV, Field, i, j);
            }
            else if (status == is_H)
            {
                ptemp[2].z = evZ(EV, Field, i, j);
                ptemp[2].r = itor(i);
            }
            else
            {
                printf("Something wrong in Function: PointsProcessing\n");
                system("pause");
                return nPoints;
            }

            nPoints++;
        }
    }
    return nPoints;
}

/* 判断输入参数(i, j)所代表的点是否在边界上 */
int isPointEdge(int i, int j)
{
    int a;
    if (i == 0)
        return is_H_T;
    else if (i == nRows - 1)
        return is_H_B;
    else if (j == 0)
        return is_V_L;
    else if (j == nCols - 1)
        return is_V_R;
    else
    {
        for (a = 0; a < n; a++)
        {
            if ((j == j_v[a]) && i < m2)
            {
                return is_V_R;
            }
            else if ((j == j_v[a] + 1) && i <= m2)
            {
                return is_V_L;
            }
            else if (j == j_v[a] && i == m2)
            {
                return is_H_E;
            }
        }
        return 0;//不是以上的情况，即不在边界上，则返回0
    }
    return FAILURE;//以上情况均未返回，说明有错误
}

/* 点内容赋值，将p2[i2]的内容赋给p1[i1] */
int PointsEqu(tPoint *p1, int i1, tPoint *p2, int i2)
{
    p1[i1].status = p2[i2].status;
    p1[i1].ip = p2[i2].ip;
    p1[i1].jp = p2[i2].jp;
    p1[i1].z = p2[i2].z;
    p1[i1].r = p2[i2].r;
    return 0;
}


/* 初始化拉格朗日插值法求轴上电位的绘制区域 */
int InitDrawLagrange(double height, double width, double asp, double vmax, double **Field)
{
    int a;
    /* 绘制坐标网格 */
    solidrectangle(-0.2*EDGEWIDTH, 0, 0, height);//坐标轴
    solidrectangle(-0.2*EDGEWIDTH, height, width, 0.2*EDGEWIDTH + height);
    POINT tripoints01[] = { { -0.1*EDGEWIDTH,-2 * EDGEWIDTH },{ -0.5*EDGEWIDTH,0 },{ 0.5*EDGEWIDTH,0 } };
    POINT tripoints02[] = { { 2 * EDGEWIDTH + width,-0.1*EDGEWIDTH + height },{ width,-0.5*EDGEWIDTH + height },{ width,0.5*EDGEWIDTH + height } };
    solidpolygon(tripoints01, 3);//箭头
    solidpolygon(tripoints02, 3);
    setlinecolor(DARKGRAY);//画坐标网格                      
    TCHAR str03[20], str04[20], str05[20];//坐标注记
    TCHAR DrawLagrangeName[50];//图名
    _stprintf(str03, _T("O"));
    _stprintf(DrawLagrangeName, _T("拉格朗日插值法求轴上电位并绘制【轴上坐标 -- 轴上电位】曲线"));
    outtextxy(-0.5*textwidth(str03), 0.5*textheight(str03) + height, str03);
    outtextxy((int)(0.5*width - 0.5*textwidth(DrawLagrangeName)), (int)(-3 * EDGEWIDTH), DrawLagrangeName);
    for (a = 1; a <= AXESGRID_X_NUM; a++)
    {
        line(a*(width / AXESGRID_X_NUM), 0, a*(width / AXESGRID_X_NUM), height);
        _stprintf(str04, _T("%.1f"), a*((width / asp) / AXESGRID_X_NUM));//横坐标轴上位置
        outtextxy(-0.5*textwidth(str04) + a*(width / AXESGRID_X_NUM), 0.5*textheight(str04) + height, str04);
    }
    for (a = 0; a < AXESGRID_Y_NUM; a++)
    {
        line(0, a*(height / AXESGRID_Y_NUM), width, a*(height / AXESGRID_Y_NUM));
        _stprintf(str05, _T("%.1fV"), (AXESGRID_Y_NUM - a)*(vmax / AXESGRID_Y_NUM));//纵坐标轴上电位
        outtextxy(-1.1*textwidth(str05), -0.5*textheight(str05) + a*(height / AXESGRID_Y_NUM), str05);
    }
    return 0;
}
/*
 * 拉格朗日插值法绘制轴上点位曲线
 * 参数列表：
 * axis: 存储轴上点位的数组指针
 * height, width: 绘制坐标区域的高和宽
 * asp: 缩放因子
 * vmax: 电位空间中最大电位值
 */
double *DrawLagrange(double *axis, double height, double width, double asp, double vmax, double **Field)
{
    setlinecolor(WHITE);
    int num, k_i = 0;
    double x, x0, x1, x2;
    double y0, y1, y2;

    for (num = 1; num <= ins; num++)
    {
        x = num*(width / asp) / ins;
        if (OnGrid_j(x) == 0)
        {
            for (k_i = 0; k_i < nCols - 1; k_i++)
            {
                if (jtoz(k_i) < x&&jtoz(k_i + 1) > x&&k_i < nCols - 2)
                {
                    x0 = jtoz(k_i);
                    x1 = jtoz(k_i + 1);
                    x2 = jtoz(k_i + 2);
                    y0 = Field[nRows - 1][k_i];
                    y1 = Field[nRows - 1][k_i + 1];
                    y2 = Field[nRows - 1][k_i + 2];
                    break;
                }
                else if (jtoz(k_i) < x&&x < jtoz(k_i + 1) && k_i == nCols - 2)
                {
                    x0 = jtoz(k_i - 1);
                    x1 = jtoz(k_i);
                    x2 = jtoz(k_i + 1);
                    y0 = Field[nRows - 1][k_i - 1];
                    y1 = Field[nRows - 1][k_i];
                    y2 = Field[nRows - 1][k_i + 1];
                    break;
                }
            }
            axis[num] = (((x - x1)*(x - x2)) / ((x0 - x1)*(x0 - x2)))*y0 + (((x - x0)*(x - x2)) / ((x1 - x0)*(x1 - x2)))*y1 + (((x - x0)*(x - x1)) / ((x2 - x0)*(x2 - x1)))*y2;
        }
        else
        {
            axis[num] = Field[nRows - 1][OnGrid_j(x)];
        }
        line((x - (width / asp) / ins)*asp, height - (height / vmax)*axis[num - 1], x*asp, height - (height / vmax)*axis[num]);
    }
    return axis;
}

int OnGrid_j(double x)
{
    int j;
    for (j = 1; j < nCols; j++)
    {
        if (fabs(x - jtoz(j)) < ACCURACY)
            return j;//x坐标在某个网格点上
    }
    return 0;//x坐标不在网格点上
}

/*
 * 附录：一些说明
 * 1. 本程序中的问题包含三个坐标体系：
 * 一个是网格点坐标(i,j)，正方向分别为：i->向下，j->向右；坐标为int型；
 * 第二个是距离坐标(x,y)，正方向分别为：x->向右，y->向下；坐标为double型；
 * 第三个是距离坐标(z,r)，正方向分别为：z->向右，r->向上。坐标为double型；
 * 需要仔细注意区分方向
 *
 * 2. 关于逐点扫描连线的说明
 *
 * 逐点扫描连线，简单说就是找到一个电势等于EV的点，然后扫描这个点周边，在周边找到等电位点连线，就这样一个点接一个点地连出电位空间线
 * 如果扫描某个点地周边，可以将该点记作point[1]，由于电位空间线是一条连续曲线，所以一定能在这个点周边找到至少两个等电位点：
 * 一个是之前的连线过程中已经跟point[1]连接过的点，记作point[0]
 * 一个是还未跟point[1]连接过的点，记作point[2]，下一步就可以连接point[1]和point[2]
 * 存在鞍点的情况下，可能会在电位空间线的极点出现第三个等电位点
 *
 * 由于等电位点可能是在网格线上，也可能恰好在网格点上，所以对周边的扫描可以分成 扫描列、扫描行、扫描顶点 这三种方式
 * 而中心点的状态也因为所在位置不同而有 is_P is_V is_H is_H_T is_H_B is_V_L is_V_R 这7种状态，以及对于起点的 is_P_T is_H_E 这额外两种
 * 下面的图展示了几种情况：
 * 图中， | 表示待扫描的列， -- 表示待扫描的行， o 表示待扫描的顶点， # 是中心点， * * 是不需要扫描的网格线，可以看作虚线
 *
 * is_P:
 *
 *   o-------o-------o
 *   |       *       |
 *   |       *       |
 *   |       *       |
 *   o * * * # * * * o
 *   |       *       |
 *   |       *       |
 *   |       *       |
 *   o-------o-------o
 *
 * is_V:
 *
 *   o-------o-------o
 *   |       *       |
 *   |       #       |
 *   |       *       |
 *   o-------o-------o
 *
 * is_H:
 *
 *   o-------o
 *   |       |
 *   |       |
 *   |       |
 *   o * # * o
 *   |       |
 *   |       |
 *   |       |
 *   o-------o
 *
 * is_H_T:
 *
 *   * * * * * # * * * * * (边界线)
 *         |       |
 *         |       |
 *         |       |
 *         o-------o
 *
 * is_P_T:
 *
 *   * * * * * * * # * * * * * * *
 *         |       *       |
 *         |       *       |
 *         |       *       |
 *         o-------o-------o
 *
 *
 */