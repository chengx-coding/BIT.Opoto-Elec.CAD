/*Code by Cheng Xin*/
#pragma warning (disable:4996) //主要针对VS的warning
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
//#include<graphics.h>

#define FILEPATH "C:\\BIT\\CAD\\" //定义输入数据和输出数据的文件路径
#define FILEIN "in.dat" //定义输入数据文件名
#define FILEOUT "out.dat" //定义输出数据文件名

/*定义全局变量*/
FILE *data;
FILE *out;

double **field;//电场电位（SOR迭代求得）
double *axis;//轴上电位（拉格朗日差值法求得）

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
double dv;//要求扫描搜索等电位线的电位间隔值
int m;//要求扫描搜索等电位线的电位值的个数
double *ev;//要求扫描搜索等电位线的电位值

int i;//一般用于循环时电极数计数，个别情况下也用于其它计数
int j;//表示电场中的第j+1行
int k;//表示电场中的第k+1列
int row = 0;//电场电位二维数组的行数
int col = 0;//电场电位二维数组的列数
double *z_v;//电极靠左一边的z坐标值
double **c1, **c2, **c3, **c4, **c0;//c1, c2, c3, c4分别是SOR法中的系数(coefficient)

double EV;//待扫描的电位值
double z_for = 0;//之前一个点的z坐标
double r_for = 0;//之前一个点的r坐标

/* 
 * 确定一个点后，扫描这个点所在网格线段的四周，一般会找到【2个】等电位点，一个是之前已经找到并连过线的点，另一个是待连线的点，即需要求的下一个点
 * 个别情况下，如有鞍点时，会找到【3个】等电位点，这时就要判断哪个点要先连上，哪个点要后连上	
 * 以下数组中，zp[0]存当前点的坐标，zp[1]存下一个点的坐标，有鞍点的极少情况下会用到z[2]。其他数组与此类似。
 * 为了防止个别情况下的溢出，将数组大小设为4
 */ 
double zp[4];//扫描点的z坐标
double rp[4];//扫描点的r坐标
int jp[4];//扫描点所在线段的起始点的行位置
int kp[9];//扫描点所在线段的起始点的列位置
int v_flag;//用于判断是否在相应范围内扫描到了待搜索电位，并记录扫描到的符合条件的点的个数
int point[4];//point[0]表示某个点可能存在的情况（共三种：网格的竖边【r方向】、网格的横边【z方向】、某个网格点【point】）


/* 声明子程序 */
int iteration(double w);//迭代一次的函数
double coordinate_z(int kc);//求电位点的z坐标值
double coordinate_r(int jc);//求电位点的r坐标值
double coordinate_zp(int jm, int km);//求电位点的z坐标值
double coordinate_rp(int jm, int km);//求电位点的r坐标值
int direction_r(int jm, int km);//网格的竖边【r方向】
int direction_z(int jm, int km);//网格的横边【z方向】
int direction_point(int jm, int km); //某个网格点【point】
int judgelocation(double x);//判断某点在轴上的位置


int main()
{
    char fname[128] = "C:\\BIT\\CAD\\in.dat";//文件路径
    //char fname[128];
    //strcat(fname, FILEPATH);
    //strcat(fname, FILEIN);

    errno_t err1;
    err1 = fopen_s(&data, fname, "r");

    if (data == NULL)
    {
        printf("无法打开文件:%s\n", fname);
        system("pause");
        exit(1);
    }
    //为n赋值
    fscanf(data, "n\t电极总数\n");
    fscanf(data, "%d\n", &n);
    //为d赋值
    fscanf(data, "\nd\t电极厚度\n");
    fscanf(data, "%lf\n", &d);
    //为z赋值
    z = (double *)malloc(n * sizeof(double));
    fscanf(data, "\nz\t相邻电极之间的距离\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%lf ", &z[i]);
    //为step赋值
    step = (int *)malloc(n * sizeof(int));
    fscanf(data, "\n\nstep\t相邻电极之间要划分的步长数\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%d ", &step[i]);
    //为v赋值
    v = (double *)malloc(n * sizeof(double));
    fscanf(data, "\n\nv\t电极电位\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%lf ", &v[i]);
    //为r1赋值
    fscanf(data, "\n\nr1\t电极内孔径半径\n");
    fscanf(data, "%lf\n", &r1);
    //为m1赋值
    fscanf(data, "\nm1\tr1范围内等步长划分的网格数\n");
    fscanf(data, "%d\n", &m1);
    //为r2赋值
    fscanf(data, "\nr2\t从电极内孔边到封闭边界处的径向距离\n");
    fscanf(data, "%lf\n", &r2);
    //为m2赋值
    fscanf(data, "\nm2\tr2范围内等步长划分的网格数\n");
    fscanf(data, "%d\n", &m2);
    //为e赋值
    fscanf(data, "\ne\t迭代控制精度\n");
    fscanf(data, "%lf\n", &e);
    //为nst赋值
    fscanf(data, "\nnst\t输出打印空间电位时的网格点间格数\n");
    fscanf(data, "%d\n", &nst);
    //为ins赋值
    fscanf(data, "\nins\t轴上电位作等距插值时的步长数\n");
    fscanf(data, "%d\n", &ins);
    //为dv赋值
    fscanf(data, "\ndv\t要求扫描搜索等电位线的电位间隔值\n");
    fscanf(data, "%lf\n", &dv);
    //为m赋值
    fscanf(data, "\nm\t要求扫描搜索等电位线的电位值的个数\n");
    fscanf(data, "%d\n", &m);
    //为ev赋值
    if (m > 0)
    {
        ev = (double *)malloc(m * sizeof(double));
        fscanf(data, "\nev\t要求扫描搜索等电位线的电位值\n");
        for (i = 0; i < m; i++)
            fscanf(data, "%lf ", &ev[i]);
    }

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
    	printf("成功读取%s",fname);
    }

    row = m1 + m2 + 1;//求总行数
    for (i = 0; i < n; i++)
        col = col + step[i];
    col = col + n;//求总列数

                  //建立电场二维数组
    field = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        field[j] = (double *)malloc(sizeof(double)*col);

    /*赋初值*/
    //先将电位全赋为0
    for (j = 0; j < row; j++)
    {
        for (k = 0; k < col; k++)
        {
            field[j][k] = 0;
        }
    }

    /*给电极赋值，以及用插值法给每个点赋初值*/
    int l;//用于两电极之间列数的计数
    double dif;//两个点之间的插值

    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//第i+1个电极的靠右一列(除最后一个外电极用两列表示)
        for (j = 0; j < m2 + 1; j++)
        {
            if (k < col)
            {
                field[j][k - 1] = v[i];
                field[j][k] = field[j][k - 1];//给除最后一个电极外每个电极赋值(最后一个电极用一列表示)
            }
            if (i == 0)
                dif = v[i] / step[i];
            else
                dif = (v[i] - v[i - 1]) / step[i];
            for (l = k - step[i]; l < k - 1; l++)//两电极间的区域(不包括电极)
            {
                field[j][l] = field[j][l - 1] + dif;
            }
        }
    }
    for (j = 0; j < row; j++)
        field[j][col - 1] = v[n - 1];//给最后一个电极赋值

                                     //求各电极靠左一边的z坐标
    z_v = (double *)malloc(n * sizeof(double));
    for (i = 0, z_v[0] = 0; i < n; i++)
    {
        if (i == 0)
            z_v[i] = z_v[i] + z[i];
        else
            z_v[i] = z_v[i - 1] + z[i] + d;
    }

    /*预先求出每个点对应的系数c1,c2,c3,c4,c0的值并分别存入对应的二维数组中，便于迭代时进行计算*/
    double h1, h2, h3, h4;//h1,h2,h3,h4分别是某个点与周围四个点的距离
    double r0;//某个点与底边的距离

              //建立c1数组,c2数组,c3数组,c4,c0数组
    c1 = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        c1[j] = (double *)malloc(sizeof(double)*col);

    c2 = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        c2[j] = (double *)malloc(sizeof(double)*col);

    c3 = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        c3[j] = (double *)malloc(sizeof(double)*col);

    c4 = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        c4[j] = (double *)malloc(sizeof(double)*col);

    c0 = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        c0[j] = (double *)malloc(sizeof(double)*col);

    //先将c1,c2,c3,c4,c0全赋为0
    for (j = 0; j < row; j++)
    {
        for (k = 0; k < col; k++)
        {
            c1[j][k] = 0;
            c2[j][k] = 0;
            c3[j][k] = 0;
            c4[j][k] = 0;
            c0[j][k] = 0;
        }
    }

    //根据具体情况计算c1和c2
    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//第i+1个电极的靠右一列(除最后一个外电极用两列表示)
        for (j = 1; j < row; j++)
        {
            for (l = k - step[i]; l < k - 1; l++)//两电极间的区域(不包括电极)
            {
                h1 = z[i] / step[i];
                h2 = z[i] / step[i];
                c1[j][l] = 2 / (h1*(h1 + h2));
                c2[j][l] = 2 / (h2*(h1 + h2));//两电极之间的区域的所有点
            }
            if (j>m2&&k < col)
            {
                h1 = z[i] / step[i];
                h2 = d;
                c1[j][k - 1] = 2 / (h1*(h1 + h2));
                c2[j][k - 1] = 2 / (h2*(h1 + h2));//第i+1个电极靠左一列在r1范围内的点(排除最后一个电极)
                h1 = d;
                h2 = z[i + 1] / step[i + 1];
                c1[j][k] = 2 / (h1*(h1 + h2));
                c2[j][k] = 2 / (h2*(h1 + h2));//第i+1个电极靠右一列在r1范围内的点(排除最后一个电极)
            }
        }
    }

    //根据具体情况计算c3和c4
    for (j = 1; j < row; j++)
    {
        for (k = 1; k < col - 1; k++)
        {
            if (j<m2)
            {
                h3 = r2 / m2;
                h4 = r2 / m2;
            }
            if (j == m2)
            {
                h3 = r1 / m1;
                h4 = r2 / m2;
            }
            if (j>m2&&j < row)
            {
                h3 = r1 / m1;
                h4 = r1 / m1;
            }
            if (j <= m2)
                r0 = r1 + r2 - j*(r2 / m2);
            else
                r0 = r1 - (j - m2)*(r1 / m1);
            if (j == row - 1)
            {
                h3 = 0;
                c3[j][k] = 0;
                c4[j][k] = 4 / (h4*h4);
            }
            else
            {
                c3[j][k] = (2 * r0 - h4) / (r0*h3*(h3 + h4));
                c4[j][k] = (2 * r0 + h3) / (r0*h4*(h3 + h4));
            }
        }
    }
    for (j = 0; j < row; j++)
    {
        for (k = 0; k < col; k++)
        {
            c0[j][k] = c1[j][k] + c2[j][k] + c3[j][k] + c4[j][k];
        }
    }

    /* 输出数据 */	
    char outname[128] = "C:\\BIT\\CAD\\out.dat";
//    char outname[128];
//    strcat(outname, FILEPATH);
//    strcat(outname, FILEOUT);

    errno_t err2;
    err2 = fopen_s(&out, outname, "w");
    //输出“输入数据”
    fprintf(out, "电极总数n:");
    fprintf(out, "%d\n", n);
    fprintf(out, "电极厚度d:");
    fprintf(out, "%lf\n", d);
    fprintf(out, "相邻电极之间的距离z:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%lf ", z[i]);
    fprintf(out, "\n相邻电极之间要划分的步长数step:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%d ", step[i]);
    fprintf(out, "\n电极电位v:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%lf ", v[i]);
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
        fprintf(out, "要求扫描搜索等电位线的电位间隔值dv:");
        fprintf(out, "%lf\n", dv);
    }
    else
    {
        fprintf(out, "要求扫描搜索等电位线的电位值的个数m:");
        fprintf(out, "%d\n", m);
        fprintf(out, "要求扫描搜索等电位线的电位值ev:\n");
        for (i = 0; i < m; i++)
            fprintf(out, "%lf ", ev[i]);
    }
    //输出网格点坐标
    fprintf(out, "\n输出r方向的网格点坐标如下:\n");
    for (j = 0; j <row; j++)
    {
        fprintf(out, "第%d个点:\t", j);
        fprintf(out, "%lf\n", coordinate_r(j));
    }
    fprintf(out, "\n输出z方向的网格点坐标如下:\n");
    for (k = 0; k < col; k++)
    {
        fprintf(out, "第%d个点:\t", k);
        fprintf(out, "%lf\n", coordinate_z(k));
    }

    /*计算电场中的电位值*/
    int num_w, num_f;//用于显示求w和求电位时迭代轮次
    num_w = 0;
    num_f = 0;

    double w_0, w_1, w_lmda, w_m, w_m_for;//超张弛迭代因子
    double lmda;//拉姆达
    double u_lmda;
    double E_0, E_1;
    double Emax;//每轮最大残差

    double **field_for;//前一次运算得到的电场电位

    field_for = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        field_for[j] = (double *)malloc(sizeof(double)*col);
    //计算超张弛迭代因子w最佳值的估计值
    w_m_for = 0;
    w_0 = 1;
    iteration(w_0);//迭代
    w_1 = 1.375;//为w_1赋初值
    int count;//迭代计次
    while (1)
    {
        num_w++;//迭代轮次计数
        for (count = 1; count <= 10; count++)
            iteration(w_1);//迭代10次

                           //令field_for等于迭代第10次的field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }

        iteration(w_1);//迭代第11次

                       //求第11次迭代后电位值与第10次迭代后电位值的残差的平均值
        E_0 = 0;//残差平均值清零
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                E_0 = E_0 + fabs(field[j][k] - field_for[j][k]);
            }
        }
        E_0 = E_0 / (row*col);

        //令field_for等于迭代第11次的field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }

        iteration(w_1);//迭代第12次

                       //求第11次迭代后电位值与第12次迭代后电位值的残差的平均值
        Emax = 0;
        E_1 = 0;//残差平均值清零
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                if (fabs(field[j][k] - field_for[j][k])>Emax)
                {
                    Emax = fabs(field[j][k] - field_for[j][k]);
                }
                E_1 = E_1 + fabs(field[j][k] - field_for[j][k]);
            }
        }
        E_1 = E_1 / (row*col);

        lmda = E_1 / E_0;
        u_lmda = (lmda + w_1 - 1) / (sqrt(lmda)*w_1);
        w_lmda = 2 / (1 + sqrt(1 - (u_lmda*u_lmda)));
        w_m = 1.25*w_lmda - 0.5;

        fprintf(out, "\n第%d轮迭代：\n", num_w);
        fprintf(out, "\t迭代次数：%d\n", 12 * num_w);
        fprintf(out, "\t迭代因子：%lf\n", w_m);
        fprintf(out, "\t平均残差：%lf\n", E_1);
        fprintf(out, "\t最大残差：%lf\n", Emax);

        if (fabs((w_m - w_m_for) / (2 - w_m)) < 0.05)
            break;
        else
        {
            w_m_for = w_m;
            w_1 = w_m;
        }

    }

    //用最佳w值进行迭代，计算电位
    int e_flag;//用于判断迭代是否满足精度要求
    while (1)
    {
        num_f++;
        //令field_for等于前一次迭代所得的field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }
        iteration(w_m);

        Emax = 0;
        E_1 = 0;//残差平均值清零
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                if (fabs(field[j][k] - field_for[j][k])>Emax)
                {
                    Emax = fabs(field[j][k] - field_for[j][k]);
                }
                E_1 = E_1 + fabs(field[j][k] - field_for[j][k]);
            }
        }
        E_1 = E_1 / (row*col);//残差

        fprintf(out, "\n第%d轮迭代：\n", num_w + num_f);
        fprintf(out, "\t迭代次数：%d\n", 12 * num_w + num_f);
        fprintf(out, "\t迭代因子：%lf\n", w_m);
        fprintf(out, "\t平均残差：%lf\n", E_1);
        fprintf(out, "\t最大残差：%lf\n", Emax);

        e_flag = 0;
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                if (fabs(field[j][k] - field_for[j][k]) < e)//判读改点是否满足迭代控制精度
                    e_flag++;
            }
        }
        if (e_flag == row*col)
            break;
    }
    //打印网格上电位
    fprintf(out, "\n打印网格上的电位，间隔数NST为%d:\n", nst);
    for (j = 0; j < row; j = j + nst)
    {
        fprintf(out, "\n");
        for (k = 0; k < col; k = k + nst)
        {
            fprintf(out, "%lf \t", field[j][k]);
        }
    }


//    
///**********************************开始画图**********************************************/
//    double width;//绘制电场宽度
//    double height;//绘制电场高度
//    int asp;//缩放因子
//    for (i = 0, width = 0; i < n; i++)
//    {
//        width = width + z[i];
//    }
//    asp = 640 / (r1 + r2);
//    width = (width + d*(n - 1))*asp;
//    height = (r1 + r2)*asp;
//
//    initgraph(width + 2, height + 2, SHOWCONSOLE);//初始化绘图环境
//    line(0, 0, width, 0);//绘制上边界
//    line(0, 0, 0, height);//绘制最左边0V阴极
//    line(width, 0, width, height);//绘制最右边荧光屏
//    line(0, height, width, height);//绘制z轴
//    for (i = 0; i < n - 1; i++)
//        solidrectangle(z_v[i] * asp, 0, (z_v[i] + d)*asp, r2*asp);//绘制电极,【solidrectangle】函数在旧版<graphics.h>中应改为【bar】
//
//    int i_m;//扫描电位个数计次
//    //另一种输入扫描搜索等电位值的方式
//    if (m == 0)
//    {
//        m = v[n - 1] / dv;
//        ev = (double *)malloc(m * sizeof(double));
//        for (i_m = 0; i_m < m; i_m++)
//            ev[i_m] = (i_m + 1)*dv;
//    }
//
//    /*************画等位线*********************/
//    fprintf(out, "\n\n\n打印等位线的电位值和各点坐标(z,r)\n", EV);
//    int firstrow;//第一行满足扫描电位值的点的个数
//    int firstrow_i, firstrow_j;
//    for (i_m = 0; i_m < m; i_m++)
//    {
//        EV = ev[i_m];
//
//        if (EV == v[n - 1])	continue;//不能等于最后一个电极的值
//
//        fprintf(out, "\n电位值为%.1lf时：\n", EV);
//
//        firstrow = 0;
//        for (k = 0; k<col - 1; k++)
//        {
//            if ((field[0][k] <= EV&&field[0][k + 1] > EV) || (field[0][k] >= EV&&field[0][k + 1] < EV))//搜索第1个点所在位置的个数【不考虑靠近荧光屏的情况】
//            {
//                firstrow++;
//            }
//        }
//
//        int jm, km;//用于简化判断过程
//
//        for (firstrow_i = 1; firstrow_i <= firstrow; firstrow_i++)
//        {
//            point[0] = 0;
//            v_flag = 0;
//            firstrow_j = 0;
//            for (k = 0; k<col - 1; k++)
//            {
//                if ((field[0][k] <= EV&&field[0][k + 1] > EV) || (field[0][k] >= EV&&field[0][k + 1] < EV))//搜索第1个点所在位置【不考虑靠近荧光屏的情况】
//                {
//                    firstrow_j++;
//                    if (firstrow_j == firstrow_i)
//                    {
//                        zp[0] = coordinate_zp(0, k);
//                        rp[0] = coordinate_r(0);
//                        jp[0] = 0;
//                        kp[0] = k;
//                        break;
//                    }
//                }
//            }
//
//            for (i = 0; i < n; i++)
//            {
//                if (fabs(zp[0] - z_v[i] - d)<0.0001)
//                {
//                    zp[0] = zp[0] - d;
//                    kp[0]--;
//                    v_flag = -1;
//                    break;
//                }
//            }
//            fprintf(out, "\t%lf,%lf\n", zp[0], rp[0]);
//            if (v_flag != -1)
//            {
//                //寻找第2个点的位置
//                v_flag = 0;
//                if (field[0][k] == EV)//第1个点第1种情况
//                {
//                    //是否在竖边上
//                    for (jm = jp[0], km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                    //是否在横边上
//                    for (jm = jp[0] + 1, km = kp[0] - 1; km <= kp[0]; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                    //是否在网格点上
//                    for (jm = jp[0] + 1, km = kp[0] - 1; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_point(jm, km) == -1)	continue;
//                    }
//                }
//                else//第1个点第2种情况
//                {
//                    //是否在竖边上
//                    for (jm = jp[0], km = kp[0]; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                    //是否在横边上
//                    for (jm = jp[0] + 1, km = kp[0]; km <= kp[0]; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                    //是否在网格点上
//                    for (jm = jp[0] + 1, km = kp[0]; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_point(jm, km) == -1)	continue;
//                    }
//                }
//                line(zp[0] * asp, height - rp[0] * asp, zp[1] * asp, height - rp[1] * asp);
//                z_for = zp[0];
//                r_for = rp[0];
//                rp[0] = rp[1];
//                zp[0] = zp[1];
//                jp[0] = jp[1];
//                kp[0] = kp[1];
//                point[0] = point[1];
//                fprintf(out, "\t%lf,%lf\n", zp[0], rp[0]);
//            }
//
//            else//扫描等位线电位等于某个电极电位的情况
//            {
//                v_flag = 0;
//                //是否在竖边上
//                for (jm = jp[0]; jm <= m2; jm++)
//                {
//                    for (km = kp[0] - 1; km <= kp[0] + 2; km = km + 3)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                }
//                //是否在横边上
//                for (jm = m2 + 1; jm <= m2 + 1; jm++)
//                {
//                    for (km = kp[0] - 1; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                }
//                //是否在网格点上
//                for (jm = m2 + 1; jm <= m2 + 1; jm++)
//                {
//                    for (km = kp[0] - 1; km <= kp[0] + 2; km = km + 3)
//                    {
//                        if (direction_point(jm, km) == -1)	continue;
//                    }
//                }
//                if (v_flag == 0)
//                {
//                    continue;
//                }
//                line((coordinate_z(kp[0]) + coordinate_z(kp[0] + 1)) / 2 * asp, height - coordinate_r(m2)*asp, zp[1] * asp, height - rp[1] * asp);//将点连到电极上
//                z_for = zp[0];
//                r_for = rp[0];
//                rp[0] = rp[1];
//                zp[0] = zp[1];
//                jp[0] = jp[1];
//                kp[0] = kp[1];
//                point[0] = point[1];
//                fprintf(out, "\t%lf,%lf\n", zp[0], rp[0]);
//            }
//
//            //逐个扫点逐段连线
//            while (1)
//            {
//                v_flag = 0;
//                switch (point[0])
//                {
//
//                case 1:
//                    //是否在竖边上
//                    for (jm = jp[0]; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在横边上
//                    for (jm = jp[0]; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在网格点上
//                    for (jm = jp[0]; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km++)
//                        {
//                            if (direction_point(jm, km) == -1)	continue;
//                        }
//                    }
//                    break;
//
//                case 2:
//                    //是否在竖边上
//                    for (jm = jp[0] - 1; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0]; km <= kp[0] + 1; km++)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在横边上
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm = jm + 2)
//                    {
//                        for (km = kp[0]; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在网格点上
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0]; km <= kp[0] + 1; km++)
//                        {
//                            if (direction_point(jm, km) == -1)	continue;
//                        }
//                    }
//                    break;
//
//                case 3:
//                    //是否在竖边上
//                    for (jm = jp[0] - 1; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在横边上
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm = jm + 2)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //是否在网格点上
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km++)
//                        {
//                            //只考虑四周的点，中心点不考虑
//                            if (km == kp[0] && jm == jp[0])
//                            {
//                                continue;
//                            }
//
//                            if (direction_point(jm, km) == -1)	continue;
//                        }
//                    }
//                    break;
//
//
//                default:
//                    printf("/nswitch语句中有错误/n");
//                    break;
//                }
//
//                line(zp[0] * asp, height - rp[0] * asp, zp[1] * asp, height - rp[1] * asp);
//
//                fprintf(out, "\t%lf,%lf\n", zp[1], rp[1]);
//
//                if (jp[1] == 0 || jp[1] == row - 1)
//                {
//                    break;
//                }
//                z_for = zp[0];
//                r_for = rp[0];
//                rp[0] = rp[1];
//                zp[0] = zp[1];
//                jp[0] = jp[1];
//                kp[0] = kp[1];
//                point[0] = point[1];
//
//            }
//        }
//    }
//    system("pause");
//
//    closegraph();//关闭图形环境
//
//    initgraph(width + 5, height + 5, SHOWCONSOLE);//初始化绘图环境
//
// /******************************************拉格朗日插值法算轴上电位***********************************************/
//
// /* 求电位的最大值 */
//    double vmax = 0;
//    for (i = 0; i < n; i++)
//    {
//        if (v[i]>vmax)
//            vmax = v[i];
//    }
//
//    int num, k_i, countx = 0;
//    double x, x0, x1, x2;
//    double y0, y1, y2;
//    axis = (double *)malloc((ins + 1) * sizeof(double));
//    axis[0] = 0;
//    axis[ins] = v[n - 1];
//    for (num = 1; num <= ins; num++)
//    {
//        x = num*(width / asp) / ins;
//        if (judgelocation(x) == 0)
//        {
//            for (k_i = 0; k_i < col - 1; k_i++)
//            {
//                if (coordinate_z(k_i) < x&&coordinate_z(k_i + 1)>x&&k_i<col - 2)
//                {
//                    x0 = coordinate_z(k_i);
//                    x1 = coordinate_z(k_i + 1);
//                    x2 = coordinate_z(k_i + 2);
//                    y0 = field[row - 1][k_i];
//                    y1 = field[row - 1][k_i + 1];
//                    y2 = field[row - 1][k_i + 2];
//                    break;
//                }
//                else if (coordinate_z(k_i)<x&&x<coordinate_z(k_i + 1) && k_i == col - 2)
//                {
//                    x0 = coordinate_z(k_i - 1);
//                    x1 = coordinate_z(k_i);
//                    x2 = coordinate_z(k_i + 1);
//                    y0 = field[row - 1][k_i - 1];
//                    y1 = field[row - 1][k_i];
//                    y2 = field[row - 1][k_i + 1];
//                    break;
//                }
//            }
//            axis[num] = (((x - x1)*(x - x2)) / ((x0 - x1)*(x0 - x2)))*y0 + (((x - x0)*(x - x2)) / ((x1 - x0)*(x1 - x2)))*y1 + (((x - x0)*(x - x1)) / ((x2 - x0)*(x2 - x1)))*y2;
//        }
//        else
//        {
//            axis[num] = field[row - 1][judgelocation(x)];
//        }
//        line((x - (width / asp) / ins)*asp, height - (height / vmax)*axis[num - 1] - 3, x*asp, height - (height / vmax)*axis[num] - 3);
//    }
//    line(0, 0, 0, height - 3);
//    line(0, height - 3, width, height - 3);
//    fprintf(out, "\n输出轴上电位值及其位置：\n\n");
//    for (num = 0; num <= ins; num++)
//        fprintf(out, "轴上位置: %lf \t电位值为: %lf\n", num*(width / asp) / ins, axis[num]);
//    fclose(out);
//    system("pause");
//    return 0;




}



/******************************************子程序*****************************************************/
//迭代函数
int iteration(double w)
{
    //计算r1范围内的电位值
    for (j = row - 1; j>m2; j--)
        for (k = 1; k<col - 1; k++)
        {
            if (j == row - 1)
                field[j][k] = (1 - w)*field[j][k] + w*(c1[j][k] * field[j][k - 1] + c2[j][k] * field[j][k + 1] + c4[j][k] * field[j - 1][k]) / c0[j][k];
            else
                field[j][k] = (1 - w)*field[j][k] + w*(c1[j][k] * field[j][k - 1] + c2[j][k] * field[j][k + 1] + c3[j][k] * field[j + 1][k] + c4[j][k] * field[j - 1][k]) / c0[j][k];
        }

    //计算r2范围内的电位值
    int l;
    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//第i+1个电极的靠右一列(除最后一个外电极用两列表示)
        for (j = m2; j >0; j--)
            for (l = k - step[i]; l < k - 1; l++)//两电极间的区域(不包括电极)
                field[j][l] = (1 - w)*field[j][l] + w*(c1[j][l] * field[j][l - 1] + c2[j][l] * field[j][l + 1] + c3[j][l] * field[j + 1][l] + c4[j][l] * field[j - 1][l]) / c0[j][l];
    }
    return 0;
}

//求坐标值
double coordinate_z(int kc)
{
    double z_k = 0;//z坐标值
    int v_step = step[0];//表示第i+1个电极靠左一边所在的列数

    for (i = 0; i <= n - 1; i++)
    {
        if (kc == 0)
        {
            z_k = 0;
            break;
        }
        if (kc >= v_step - step[i] && kc <= v_step)
        {
            z_k = z_v[i] - (v_step - kc)*(z[i] / step[i]);
        }
        if (i != n - 1)
        {
            v_step = v_step + step[i + 1] + 1;
        }
    }
    return z_k;
}

double coordinate_r(int jc)
{
    double r_j;//r坐标值
    if (jc <= m2)
        r_j = r1 + r2 - jc*(r2 / m2);
    else
        r_j = r1 - (jc - m2)*(r1 / m1);
    return r_j;
}

double coordinate_zp(int jm, int km)
{
    return(coordinate_z(km) + fabs((EV - field[jm][km]) / (field[jm][km + 1] - field[jm][km]))*(coordinate_z(km + 1) - coordinate_z(km)));
}

double coordinate_rp(int jm, int km)
{
    return(coordinate_r(jm + 1) + fabs((EV - field[jm + 1][km]) / (field[jm][km] - field[jm + 1][km]))*(coordinate_r(jm) - coordinate_r(jm + 1)));
}


int direction_r(int jm, int km)
{
    if ((EV > field[jm][km] && EV < field[jm + 1][km]) || (EV < field[jm][km] && EV > field[jm + 1][km]))
    {
        v_flag++;
        zp[v_flag] = coordinate_z(km);
        rp[v_flag] = coordinate_rp(jm, km);

        //判断是否是跟先前连过的点重复
        if (z_for == zp[v_flag] && r_for == rp[v_flag])
        {
            v_flag--;
            return (-1);
        }

        jp[v_flag] = jm;
        kp[v_flag] = km;
        point[v_flag] = 1;
        return 1;
    }
    else
    {
        return 0;
    }
}
int direction_z(int jm, int km)
{
    if ((EV > field[jm][km] && EV < field[jm][km + 1]) || (EV < field[jm][km] && EV > field[jm][km + 1]))
    {
        v_flag++;
        zp[v_flag] = coordinate_zp(jm, km);
        rp[v_flag] = coordinate_r(jm);

        //判断是否是跟先前连过的点重复
        if (z_for == zp[v_flag] && r_for == rp[v_flag])
        {
            v_flag--;
            return(-1);
        }

        jp[v_flag] = jm;
        kp[v_flag] = km;
        point[v_flag] = 2;
        return 1;
    }
    else
    {
        return 0;
    }
}
int direction_point(int jm, int km)
{
    if (EV == field[jm][km])
    {
        v_flag++;
        zp[v_flag] = coordinate_z(km);
        rp[v_flag] = coordinate_r(jm);

        //判断是否是跟先前连过的点重复
        if (z_for == zp[v_flag] && r_for == rp[v_flag])
        {
            v_flag--;
            return(-1);
        }

        jp[v_flag] = jm;
        kp[v_flag] = km;
        point[v_flag] = 3;
        return 1;
    }
    else
    {
        return 0;
    }
}

int judgelocation(double x)
{
    for (k = 1; k < col; k++)
    {
        if (fabs(x - coordinate_z(k))<0.00001)
            return k;//x坐标在某个网格点上
    }
    return 0;//x坐标不在网格点上
}


