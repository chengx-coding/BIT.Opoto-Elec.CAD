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
 * �������ʼ�汾�����2014��5��
 *
 * 2016/10/29 ���£�
 * �����������õ�graphics.h�ļ�����http://www.easyx.cn/downloads/View.aspx?id=4��ΪEasyX��(����graphics.h) ���԰� (2015-10-15 ����)
 * �����򹹽�����ͱ������õ�IDEΪVisual Studio 2015
 * ��������Ӧ���˴�����ȫ�ֱ���������һ�ֺܲ��õı��ϰ�ߣ���ʱ��ѧ��֪��Ҫ�����Ļ������Ͼ�Ҫ�ع��ˡ�
 * �ڻ���λ�ߵ�ʱ�򣬸�������»���һ�������в��ִ���ԭ��δ֪
 *
 * 2016/10/30 ���£�
 * �Ż��˻�ͼ����
 *
 * 2016/10/31 ���£�
 * ���ּ򵥹���ģ�黯
 *
 * 2016/11/01 ���£�
 * �޸�����w_lmda�Ĺ�ʽ���ڿ���֮�ڼ��������ֵ����Ӧ�����������
 * �޸��˱���������������ɶ��ԣ�i-->a, j-->i, k-->j, row-->nRows, col-->nCols. �����в���ע��δ��
 *
 * 2016/11/05 ���£�
 * ��д��ɻ���λ��ģ�飬�������BUG��
 * ģ�黯���ִ��룬�����˽ӿ�
 *
 * 2016/11/06 ���£�
 * ģ�黯���ִ��룬�����˽ӿڣ�
 * �����˵��ӷ����λ�ռ�Ĺ��̣����Ƚ��˾�����
 * �Ż��˲��ֱ��������ݽṹ
 *
 *
 *
 */

 /*
  * ע��
  * �������ݺ���������ļ�·��ֱ��д������У��뽫�궨�壺FILEPATH��FILEIN��FILEOUT ����Ϊ�������������ļ�·��
  * ����������м�����ʾ��λ�ߣ��س���������ʾ���ϵ�λ�ֲ����ޱ�ȡ�
  */

#pragma warning (disable:4996) //��Ҫ���VS��warning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphics.h>
#include <conio.h>

#define FILEPATH "C:\\BIT\\CAD\\" //�����޸ġ������������ݺ�������ݵ��ļ�·��
#define FILEIN "in.dat" //�����޸ġ��������������ļ���
#define FILEOUT "out.dat" //�����޸ġ�������������ļ���
#define DISPLAYLIMIT 600 //�����޸ġ������ͼ��ʾ�����С������
#define EDGEWIDTH 10 //�����޸ġ���ͼ����λ�ռ�߽���
#define EXTRAREGION 160 //�����޸ġ���ͼ�����������ȣ�����������ͼ��
#define AXESGRID_X_NUM 15 //�����޸ġ��������ϵ�λ����ʱ����������x���򻮷ֵĿ̶�����
#define AXESGRID_Y_NUM 10 //�����޸ġ��������ϵ�λ����ʱ����������y���򻮷ֵĿ̶�����

#define FAILURE -1 //�����ʧ�ܵķ���ֵ
#define ACCURACY 1E-6 //���þ�ȷ�ȴ�С�������Ƚ�����double�������Ƿ���ȡ���Ϊ�������������ڶ����ƴ洢��ʽ���⣬�޷���==�ж��Ƿ���ȣ�ֻ����Ϊ����С��ĳ�����ȼ���Ϊ���

  /* ����������ĳ��������λ�õ�״̬��־ */
#define is_P 0 //���������
#define is_V 1 //�ڴ�ֱ������
#define is_H 2 //��ˮƽ�����
#define is_H_T 3 //���ϱ߽�
#define is_H_B 4 //���±߽磨�缫Ҳ��߽磩
#define is_V_L 5 //����߽磨�缫Ҳ��߽磩
#define is_V_R 6 //���ұ߽磨�缫Ҳ��߽磩
#define is_P_T 7 //���ϱ߽��е��������
#define is_H_E 8 //�ڵ缫����


/*
 *
 * c1, c2, c3, c4�ֱ���SOR���е�ϵ��(coefficient)��c0Ϊ4��ϵ��֮��
 * ��λ�ռ���ÿ������㶼��Ӧ4����ͬ��ϵ������ͬ��������Ӧ��ϵ����ͬ
 * ÿ�μ���һ��������ϵĵ�λֵ������Ҫ�õ�������Ӧ���ĸ���ͬϵ��
 * ������4��ϵ���������ִ�������
 * ��һ����ÿ��һ������㣬������һ�����������Ӧ���ĸ�ϵ��
 *     �ô���ֻ��4��double�ͱ���������Ҫ����Ĵ洢�ռ䣬�������ռ临�Ӷȣ�
 *     �����ǵ���������ÿ������㶼��Ҫ�����Σ���ÿ��ϵ����ͬ���ͻ���������ظ����㣬����ʱ�临�Ӷ�
 * �ڶ�����Ԥ�ȼ����ÿ��������Ӧ��4��ϵ��ֵ���Լ�4��ϵ��֮�ͣ��������ά����
 *     �ô��Ǳ������ظ����㣬������ʱ�临�Ӷȣ����Ҵ��������⣻
 *     �����Ǵ洢��ϵ�����д����ظ���ÿ��ϵ���Ķ�ά���鶼����λ�ռ������Ķ�ά����һ���󣬼�ռ�õĴ洢�ռ�������5��
 *         �������������Ŀ��������������������ռ�ݵĴ洢�ռ�ɺ��ԣ��ʶ��൱�ڳ���Ŀռ临�Ӷ�������5��
 * �����ָ��ݵ缫֮��ļ������²�������ĳ��ȵĻ��ֲ�ͬ����ÿ��������ϵ��ֵ��ͬ
 *     �ô�����������ϵ����������ֻ���缫��Ŀ�йأ������˴洢�ռ���˷ѣ�ͬʱҲ��ʡ�˼������������ۺ���ǰ���ַ������ŵ㣻
 *     �����ǵ�������ʱ��Ҫ�жϵ�ǰ�����λ���ĸ������ڣ�������һ������Ѷ�
 * �������в��õķ����ǵڶ��֣����Կռ任ʱ��ķ���
 * ֮���Բ��õ����֣�����Ϊc��ȡֵ���Ҳ�Ƚ϶࣬�ڶ��ָ��������������׳����͹����˷�һЩ�洢�ռ�
 * Ӧ�û����������Ż�����ϵ����������������õķ����򵥴ֱ���ֵ���ᳫ��ֻ��ʱ��ִ�δ�����Ż�
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
 * �ṹ����point
 * ip��jp�ǵ�λ�ռ���ĳ����(z,r)������������Ͻǵ�����꣬
 * status������㼰�����������״̬����7�֣�
 * ������� is_P=0�������� is_V=1������� is_H=2���ϱ߽� is_H_T=3���±� is_H_B=4����߽� is_V_L=5���ұ߽� is_V_R=6
 */
typedef struct PointType {
    int status;
    int ip;
    int jp;
    double z;
    double r;
}tPoint;

/* ��������ļ�Ҫ����ຯ�����ʲ�д�����ݣ��������Ϊȫ�ֱ��� */
FILE *data;
FILE *out;


/* �����ļ�FILEIN�е����� */
int n;//�缫����
double d;//�缫���
double *z;//���ڵ缫֮��ľ���
int *step;//���ڵ缫֮��Ҫ���ֵĲ�����
double *v;//�缫��λ
double r1;//�缫�ڿ׾��뾶
int m1;//r1��Χ�ڵȲ������ֵ�������
double r2;//�ӵ缫�ڿױߵ���ձ߽紦�ľ������
int m2;//r2��Χ�ڵȲ������ֵ�������
double e;//�������ƾ���
int nst;//�����ӡ�ռ��λʱ�����������
int ins;//���ϵ�λ���Ⱦ��ֵʱ�Ĳ�����
double dv;//Ҫ��ɨ��������λ�ߵĵ�λ���ֵ
int m;//Ҫ��ɨ��������λ�ߵĵ�λֵ�ĸ���
double *ev;//Ҫ��ɨ��������λ�ߵĵ�λֵ

int nRows = 0;//��λ�ռ��λ��ά���������
int nCols = 0;//��λ�ռ��λ��ά���������
int *j_v;//�缫����һ�ߵ�j����ֵ
double *z_v;//�缫����һ�ߵ�z����ֵ


/******************************************** �������� ***********************************************/
/* �����������ļ��г�ʼ�������������ݱ��� */
double InitData();
/* ���ݱ߽�������ʼ����λ�ռ䣬�����λ�ռ��С���Լ�Ϊ������λ�ռ丳��ֵ */
int InitField(double **Field);
/* ֻ�е����缫����ʱ��ʼ����λ�ռ䣬a��ʾ��a���缫���� */
int InitFieldSingleV(double **FieldSingleV, int a);
/* ������ļ���ӡ���������ݡ�*/
int PrintData();
/* ������ļ���ӡ��������� */
int PrintGridCoordinates(double **Field);
/* ������ļ���ӡ�����ϵ�λ */
int PrintGridValues(double **Field);
/* ������ļ���ӡ�����ϵ�λ������� */
int PrintGridValuesE(double **Field);

/* ������˵缫λ��������Ӧ��ϵ��C��EdgeClosure��ʾ�Ƿ����ϱ߽� */
int ComputeCwithoutV(int EdgeClosure);
/* �����a���缫λ��������Ӧ��ϵ��C */
int CleanCofV(int a);

/* �����λ�ռ��еĵ�λֵ��������ļ���ӡ�����Ϣ */
int ComputeFieldValues(double **Field, int outputFlag);
/* ����w������������λ�ռ����һ�ε������� */
int iteration(double w, double **Field);

/* ��j����ת��Ϊz���� */
double jtoz(int j);
/* ��i����ת��Ϊr���� */
double itor(int i);
/* ���ص�λֵΪEV�ĵ��z����ֵ */
double evZ(double EV, double **Field, int i, int j);
/* ���ص�λֵΪEV�ĵ��r����ֵ */
double evR(double EV, double **Field, int i, int j);

/* ���Ƶ�λ�ռ� */
int DrawField(double height, double width, double asp, double vmax, double **Field, TCHAR *DrawFieldName);
/* α��ɫ���� */
int PseudoColor(BYTE value, double x, double y, double HoriL, double VertiL);

/* ���Ƶ�λ�� */
int DrawEquLines(double height, double width, double asp, double **Field);
/* �����ݸ�ֵ����p2[i2]�����ݸ���p1[i1] */
int PointsEqu(tPoint *p1, int i1, tPoint *p2, int i2);
/* ��������߻��Ƶ�λ�ߵĹ����жԵ�λ��Ĵ������ */
int PointsProcessing(int nPoints, int status, tPoint *point, tPoint *ptemp, double EV, double **Field, int i, int j);
/* �ж��������(i, j)������ĵ��Ƿ��ڱ߽��� */
int isPointEdge(int i, int j);

/* ��ʼ���������ղ�ֵ�������ϵ�λ�Ļ������� */
int InitDrawLagrange(double height, double width, double asp, double vmax, double **Field);
/* �������ղ�ֵ���������ϵ�λ���������� */
double *DrawLagrange(double *axis, double height, double width, double asp, double vmax, double **Field);
/* �ж�ĳ�������ϵ�λ�� */
int OnGrid_j(double x);


int main()
{
    int a;//һ������ѭ��ʱ�缫�����������������Ҳ������������
    int i;//��ʾ��λ�ռ��еĵ�i(start from 0)��
    int j;//��ʾ��λ�ռ��еĵ�j(start from 0)��

    double vmax = 0;//������λ�ռ��λ�����ֵ
    vmax = InitData();//�����������ļ��г�ʼ�������������ݱ�������������λֵ

    /********************************* ����һЩ���ڵ�λ�ռ�ı�Ҫ���� *****************************/
    nRows = m1 + m2 + 1;//��������
    for (a = 0; a < n; a++)
        nCols = nCols + step[a];
    nCols = nCols + n;//��������

    /* ������λ�ռ��ά���飬����ʼ����λΪ0 */
    double **Field;//��λ�ռ��λ��SOR������ã�
    Field = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        Field[i] = (double *)malloc(sizeof(double)*nCols);
        memset(Field[i], 0, sizeof(double)*nCols);
    }

    InitField(Field);//��ʼ����λ�ռ�

    /* ����缫����һ�ߵ�j���� */
    j_v = (int *)malloc(n * sizeof(int));
    for (a = 0, j_v[0] = 0; a < n; a++)
    {
        if (a == 0)
            j_v[a] = j_v[a] + step[a];
        else
            j_v[a] = j_v[a - 1] + step[a] + 1;
    }
    /* ����缫����һ�ߵ�z���� */
    z_v = (double *)malloc(n * sizeof(double));
    for (a = 0, z_v[0] = 0; a < n; a++)
    {
        if (a == 0)
            z_v[a] = z_v[a] + z[a];
        else
            z_v[a] = z_v[a - 1] + z[a] + d;
    }

    /* ��άϵ���ṹ��C����ʾÿ�������������Ӧ��ϵ�� */
    C = (tCoefficient **)malloc(sizeof(tCoefficient *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        C[i] = (tCoefficient *)malloc(sizeof(tCoefficient)*nCols);
        memset(C[i], 0, sizeof(tCoefficient)*nCols);
    }

    /* ����C��ֵ */
    ComputeCwithoutV(1);//1Ϊ����ϱ߽磬0Ϊ�����ϱ߽�

    /**************************************** ��ʼ������ļ�ָ�� **********************************/
    char outname[128] = FILEPATH;
    strcat(outname, FILEOUT);
    errno_t err2;
    err2 = fopen_s(&out, outname, "w");
    if (out == NULL || err2)
    {
        printf("�޷�д���ļ���%s����ȷ��·�����ļ���������ȷ�����߸��ļ��Ƿ����ڱ�ʹ�ã�\n", outname);
        system("pause");
        exit(1);
    }
    else
    {
        printf("�ɹ����ļ���%s\n", outname);
    }

    PrintData();//������������ݡ�
    PrintGridCoordinates(Field);//������������

/**************************************** �����λ�ռ��еĵ�λֵ��������ļ���ӡ�����Ϣ **********************************/
    ComputeFieldValues(Field, 1);//�����λ�ռ��еĵ�λֵ��������ļ���ӡ�����Ϣ

    PrintGridValues(Field);//��������ϵ�λ

    printf("\n�밴�������������������ʾ����λ�ռ估��λ�ߡ����ƣ�\n�ٰ��������������������ʾ�����ϵ�λֵ���ߡ����ơ�\n\n");
    system("pause");

    /********************************** ��ʼ��ͼ **********************************************/
    double width;//���Ƶ�λ�ռ���
    double height;//���Ƶ�λ�ռ�߶�

    double asp;//�������ӡ�����ֻ��Ҫ�Ŵ��λ�ռ������Ҫ�Ŵ��λ�ߣ���˲��������ź��������ֶ��������ű�����
    for (a = 0, width = 0; a < n; a++)
    {
        width = width + z[a];
    }
    asp = DISPLAYLIMIT / (r1 + r2);//����DISPLAYLIMIT����ʾ�����С�����ƾ����������Ӵ�С 
    width = (width + d*(n - 1))*asp;
    height = (r1 + r2)*asp;


    TCHAR DrawFieldName[50];//��ͼ����
    _stprintf(DrawFieldName, _T("��λ�ռ��λֵα��ɫ�Լ���λ�߻���"));
    initgraph(width + 3 * EDGEWIDTH + EXTRAREGION, height + 6 * EDGEWIDTH, SHOWCONSOLE);//��ʼ����ͼ����
    setorigin(2 * EDGEWIDTH, 4 * EDGEWIDTH);//ԭ������Ϊ(10,10)������ߺ��ϱ��������߽�Ŀռ�
    DrawField(height, width, asp, vmax, Field, DrawFieldName);//���Ƶ�λ�ռ�

    system("pause");


    /************************************************ ����λ�� ***************************************************/
    fprintf(out, "\n\n\n��ӡ��λ�ߵĵ�λֵ�͸�������(z,r)\n");
    DrawEquLines(height, width, asp, Field);
    system("pause");

    closegraph();//�ر�ͼ�λ���

    /************************************* �������ղ�ֵ�������ϵ�λ ********************************************/
    double *axis;//���ϵ�λ���������ղ�ֵ����ã�

    height = 0.8*height;
    initgraph((int)width + 10 * EDGEWIDTH, (int)height + 8 * EDGEWIDTH, SHOWCONSOLE);//��ʼ����ͼ����
    setorigin(6 * EDGEWIDTH, 4 * EDGEWIDTH);

    InitDrawLagrange(height, width, asp, vmax, Field);//��ʼ������������������

    /* ���㲢�������ϵ�λ���� */
    axis = (double *)malloc((ins + 1) * sizeof(double));
    axis[0] = 0;
    axis[ins] = v[n - 1];
    axis = DrawLagrange(axis, height, width, asp, vmax, Field);//�������ߣ��������ϵ�λ

    /* ������ļ���ӡ���ϵ�λֵ */
    fprintf(out, "\n������ϵ�λֵ����λ�ã�\n\n");
    for (a = 0; a <= ins; a++)
        fprintf(out, "����λ��: %lf \t��λֵΪ: %lf\n", a*(width / asp) / ins, axis[a]);
    system("pause");
    closegraph();//�ر�ͼ�λ���

    /***************************************** ֱ�ӷ��͵��ӷ��ıȽ� ******************************************/

    fprintf(out, "\n\n********** ֱ�ӷ�����ӷ��Ƚ� **********\n");

    double **FieldSingleV;//ֻ��һ���缫����ʱ�ĵ�λ�ռ�ֲ�
    FieldSingleV = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
    {
        FieldSingleV[i] = (double *)malloc(sizeof(double)*nCols);
        memset(FieldSingleV[i], 0, sizeof(double)*nCols);
    }

    double AE;//������λ�ľ������
    double MaxAE = 0;//���������
    double MAE = 0;//ƽ�����
    double RMSE = 0;//���������

    for (a = 0, width = 0; a < n; a++)
    {
        width = width + z[a];
    }
    width = (width + d*(n - 1))*asp;
    height = (r1 + r2)*asp;
    initgraph(width + 3 * EDGEWIDTH + EXTRAREGION, height + 6 * EDGEWIDTH, SHOWCONSOLE);//��ʼ����ͼ����
    setorigin(2 * EDGEWIDTH, 4 * EDGEWIDTH);//ԭ������Ϊ(10,10)������ߺ��ϱ��������߽�Ŀռ�

    ComputeCwithoutV(0);//�����ϱ߽�

    for (a = 0; a < n; a++)
    {
        InitFieldSingleV(FieldSingleV, a);
        ComputeFieldValues(FieldSingleV, 0);
        //_stprintf(DrawFieldName, _T("��%d���缫V%d��������ʱ�ĵ�λ�ֲ�"), a, a + 1);
        //DrawField(height, width, asp, vmax, FieldSingleV, DrawFieldName);//���Ƶ�λ�ռ�
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
    PrintGridValuesE(Field);//������ļ���ӡ�������
    fprintf(out, "\n\nͳ�ƽ����\n");
    fprintf(out, "����������\t%lf\n", MaxAE);
    fprintf(out, "��ƽ��������\t%lf\n", MAE);
    fprintf(out, "����������\t\t%lf\n", RMSE);

    clearcliprgn();
    _stprintf(DrawFieldName, _T("ֱ�ӷ�����ӷ���λ�ռ��λֵ�����������Ϊ%lf V����ͼ����ʾ��"), MaxAE);
    DrawField(height, width, asp, vmax, Field, DrawFieldName);//���Ƶ�λ�ռ�
    system("pause");
    closegraph();//�ر�ͼ�λ���

    free(FieldSingleV);

    /***************************************** ����д�����,��β ***********************************/
    printf("���ļ���%s ����д�����\n", outname);
    fclose(out);
    free(Field);//�ϸ���˵Ӧ��ÿһ��malloc����Ӧһ��free����ֹ�ڴ�й©
    free(C);
    *C = NULL;
    system("pause");

    return 0;
}



/********************************************** ���庯�� *********************************************************/
/* �����������ļ��г�ʼ�������������ݱ��� */
double InitData()
{
    int a;
    char fname[128] = FILEPATH;
    strcat(fname, FILEIN);
    errno_t err1;
    err1 = fopen_s(&data, fname, "r");

    if (data == NULL || err1)
    {
        printf("�޷����ļ���%s����ȷ��·�����ļ���������ȷ�����߸��ļ��Ƿ����ڱ�ʹ�ã�\n", fname);
        system("pause");
        exit(1);
    }
    else
    {
        printf("�ɹ����ļ�:%s\n", fname);
    }
    int datanum[15] = { 0 };//�������ݸ�������Ϊ0��˵��δ��������
    char *datastr[15] = {
        "n\t�缫����\n",
        "\nd\t�缫���\n",
        "\nz\t���ڵ缫֮��ľ���\n",
        "\n\nstep\t���ڵ缫֮��Ҫ���ֵĲ�����\n",
        "\n\nv\t�缫��λ\n",
        "\n\nr1\t�缫�ڿ׾��뾶\n",
        "\nm1\tr1��Χ�ڵȲ������ֵ�������\n",
        "\nr2\t�ӵ缫�ڿױߵ���ձ߽紦�ľ������\n",
        "\nm2\tr2��Χ�ڵȲ������ֵ�������\n",
        "\ne\t�������ƾ���\n",
        "\nnst\t�����ӡ�ռ��λʱ�����������\n",
        "\nins\t���ϵ�λ���Ⱦ��ֵʱ�Ĳ�����\n",
        "\ndv\tҪ��ɨ��������λ�ߵĵ�λ���ֵ\n",
        "\nm\tҪ��ɨ��������λ�ߵĵ�λֵ�ĸ���\n",
        "\nev\tҪ��ɨ��������λ�ߵĵ�λֵ\n"
    };
    /* Ϊn��ֵ */
    fscanf(data, datastr[0]);
    datanum[0] = fscanf(data, "%d\n", &n);
    /* Ϊd��ֵ */
    fscanf(data, datastr[1]);
    datanum[1] = fscanf(data, "%lf\n", &d);
    /* Ϊz��ֵ */
    z = (double *)malloc(n * sizeof(double));
    fscanf(data, datastr[2]);
    for (a = 0; a < n; a++)
        datanum[2] = fscanf(data, "%lf ", &z[a]) + datanum[2];
    if (datanum[2] != n)
        datanum[2] = 0;
    /* Ϊstep��ֵ */
    step = (int *)malloc(n * sizeof(int));
    fscanf(data, datastr[3]);
    for (a = 0; a < n; a++)
        datanum[3] = fscanf(data, "%d ", &step[a]) + datanum[3];
    if (datanum[3] != n)
        datanum[3] = 0;
    /* Ϊv��ֵ */
    v = (double *)malloc(n * sizeof(double));
    fscanf(data, datastr[4]);
    for (a = 0; a < n; a++)
        datanum[4] = fscanf(data, "%lf ", &v[a]) + datanum[4];
    if (datanum[4] != n)
        datanum[4] = 0;
    /* Ϊr1��ֵ */
    fscanf(data, datastr[5]);
    datanum[5] = fscanf(data, "%lf\n", &r1);
    /* Ϊm1��ֵ */
    fscanf(data, datastr[6]);
    datanum[6] = fscanf(data, "%d\n", &m1);
    /* Ϊr2��ֵ */
    fscanf(data, datastr[7]);
    datanum[7] = fscanf(data, "%lf\n", &r2);
    /* Ϊm2��ֵ */
    fscanf(data, datastr[8]);
    datanum[8] = fscanf(data, "%d\n", &m2);
    /* Ϊe��ֵ */
    fscanf(data, datastr[9]);
    datanum[9] = fscanf(data, "%lf\n", &e);
    /* Ϊnst��ֵ */
    fscanf(data, datastr[10]);
    datanum[10] = fscanf(data, "%d\n", &nst);
    /* Ϊins��ֵ */
    fscanf(data, datastr[11]);
    datanum[11] = fscanf(data, "%d\n", &ins);
    /* Ϊdv��ֵ */
    fscanf(data, datastr[12]);
    datanum[12] = fscanf(data, "%lf\n", &dv);
    /* Ϊm��ֵ */
    fscanf(data, datastr[13]);
    datanum[13] = fscanf(data, "%d\n", &m);
    /* Ϊev��ֵ */
    if (m > 0) //m��Ϊ0ʱ��ev�����ݣ�ֱ������ev��ֵ
    {
        ev = (double *)malloc(m * sizeof(double));
        fscanf(data, datastr[14]);
        for (a = 0; a < m; a++)
            datanum[14] = fscanf(data, "%lf ", &ev[a]) + datanum[14];
        if (datanum[14] != m)
            datanum[14] = 0;
        printf("m�����㣬���ո����ĵ�λ�ߵĵ�λֵ����ɨ������\n");
    }

    /* ��������λ�ռ��λ�����ֵ������Ϊ����ֵ */
    double vmax = 0;
    for (a = 0; a < n; a++)
    {
        if (v[a] > vmax)
            vmax = v[a];
    }

    if (m == 0)//mΪ0ʱ��ev�����ݣ�����λ���ֵdv����ev��ֵ
    {
        m = vmax / dv;
        datanum[14] = m;
        ev = (double *)malloc(m * sizeof(double));
        for (a = 0; a < m; a++)
            ev[a] = (a + 1)*dv;
        printf("m�����㣬���ո����ĵ�λ�ߵ�λ���ֵdv����ɨ������\n");
    }
    if (datanum[14] == 0)
        printf("Error: mֵ����\n");

    fclose(data);

    //����NULL�������������δ������ȡ
    if (&n == NULL || &d == NULL || z == NULL || step == NULL || v == NULL || &r1 == NULL || &m1 == NULL || &r2 == NULL || &m2 == NULL || &e == NULL || &nst == NULL || &ins == NULL)
    {
        printf("�޷�������ȡ����\n");
        system("pause");//��ͣ���ڿ���̨����ʾ��������
        exit(1);
    }
    else
    {
        for (a = 0; a < 15; a++)
        {
            if (datanum[a] == 0)
            {
                printf("��������δ�ܶ�ȡ�ɹ�:\n");
                printf(datastr[14]);
                printf("\n���������ļ��и��������Լ���ǰһ�����ݵĸ�ʽ�Ƿ���ȷ��\n");
                system("pause");
                exit(1);
            }
        }
        printf("�ɹ���ȡ�ļ���%s �е�����\n", fname);
    }


    return vmax;
}

/* ���ݱ߽�������ʼ����λ�ռ䣬�����λ�ռ��С���Լ�Ϊ������λ�ռ丳��ֵ */
int InitField(double **Field)
{
    int a, i, j;
    /* ���缫��ֵ���Լ��ò�ֵ����ÿ���㸳��ֵ */
    int l;//�������缫֮�������ļ���
    double dif;//������֮��Ĳ�ֵ

    for (a = 0, j = 0; a < n; a++)
    {
        j = j + step[a] + 1;//��a+1���缫�Ŀ���һ��(�����һ����缫�����б�ʾ)
        for (i = 0; i < m2 + 1; i++)
        {
            if (j < nCols)
            {
                Field[i][j - 1] = v[a];
                Field[i][j] = Field[i][j - 1];//�������һ���缫��ÿ���缫��ֵ(���һ���缫��һ�б�ʾ)
            }
            if (a == 0)
                dif = v[a] / step[a];
            else
                dif = (v[a] - v[a - 1]) / step[a];
            for (l = j - step[a]; l < j - 1; l++)//���缫�������(�������缫)
            {
                Field[i][l] = Field[i][l - 1] + dif;
            }
        }
    }
    for (i = 0; i < nRows; i++)
        Field[i][nCols - 1] = v[n - 1];//�����һ���缫��ֵ

    return 0;
}

/* ֻ�е����缫����ʱ��ʼ����λ�ռ䣬a��ʾ��a���缫���� */
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

/* ������ļ���ӡ���������ݡ�*/
int PrintData()
{
    int a;
    fprintf(out, "�缫����n:");
    fprintf(out, "%d\n", n);
    fprintf(out, "�缫���d:");
    fprintf(out, "%lf\n", d);
    fprintf(out, "���ڵ缫֮��ľ���z:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%lf ", z[a]);
    fprintf(out, "\n���ڵ缫֮��Ҫ���ֵĲ�����step:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%d ", step[a]);
    fprintf(out, "\n�缫��λv:\n");
    for (a = 0; a < n; a++)
        fprintf(out, "%lf ", v[a]);
    fprintf(out, "\n�缫�ڿ׾��뾶r1:");
    fprintf(out, "%lf\n", r1);
    fprintf(out, "r1��Χ�ڵȲ������ֵ�������m1:");
    fprintf(out, "%d\n", m1);
    fprintf(out, "�ӵ缫�ڿױߵ���ձ߽紦�ľ������r2:");
    fprintf(out, "%lf\n", r2);
    fprintf(out, "r2��Χ�ڵȲ������ֵ�������m2:");
    fprintf(out, "%d\n", m2);
    fprintf(out, "�������ƾ���e:");
    fprintf(out, "%lf\n", e);
    fprintf(out, "�����ӡ�ռ��λʱ�����������nst:");
    fprintf(out, "%d\n", nst);
    fprintf(out, "���ϵ�λ���Ⱦ��ֵʱ�Ĳ�����ins:");
    fprintf(out, "%d\n", ins);
    if (m == 0)
    {
        fprintf(out, "Ҫ��ɨ��������λ�ߵĵ�λ���ֵdv:");
        fprintf(out, "%lf\n", dv);
    }
    else
    {
        fprintf(out, "Ҫ��ɨ��������λ�ߵĵ�λֵ�ĸ���m:");
        fprintf(out, "%d\n", m);
        fprintf(out, "Ҫ��ɨ��������λ�ߵĵ�λֵev:\n");
        for (a = 0; a < m; a++)
            fprintf(out, "%lf ", ev[a]);
    }

    return 0;
}

/* ������ļ���ӡ��������� */
int PrintGridCoordinates(double **Field)
{
    int i, j;
    fprintf(out, "\n���r������������������:\n");
    for (i = 0; i < nRows; i++)
    {
        fprintf(out, "��%d����:\t", i);
        fprintf(out, "%lf\n", itor(i));
    }
    fprintf(out, "\n���z������������������:\n");
    for (j = 0; j < nCols; j++)
    {
        fprintf(out, "��%d����:\t", j);
        fprintf(out, "%lf\n", jtoz(j));
    }

    return 0;
}

/* ������ļ���ӡ�����ϵ�λ */
int PrintGridValues(double **Field)
{
    int i, j;
    fprintf(out, "\n��ӡ�����ϵĵ�λ�������NSTΪ%d:\n", nst);
    for (i = 0; i < nRows; i = i + nst)
    {
        fprintf(out, "\n�ڡ�%d���е�λ��", i);
        fprintf(out, "\n");
        for (j = 0; j < nCols; j = j + nst)
        {
            fprintf(out, "%lf \t", Field[i][j]);
        }
    }

    return 0;
}

/* ������ļ���ӡ�����ϵ�λ������� */
int PrintGridValuesE(double **Field)
{
    int i, j;
    fprintf(out, "\n��ӡ�����ϵ�ֱ�ӷ��͵��ӷ��ĵ�λ�����������NSTΪ%d:\n", nst);
    for (i = 0; i < nRows; i = i + nst)
    {
        fprintf(out, "\n�ڡ�%d���о�����", i);
        fprintf(out, "\n");
        for (j = 0; j < nCols; j = j + nst)
        {
            fprintf(out, "%lf \t", Field[i][j]);
        }
    }

    return 0;
}


/* ������˵缫λ��������Ӧ��ϵ��C��EdgeClosure��ʾ�Ƿ����ϱ߽� */
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

/* �����a���缫λ��������Ӧ��ϵ��C */
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

/* �����λ�ռ��еĵ�λֵ��������ļ���ӡ�����Ϣ */
int ComputeFieldValues(double **Field, int outputFlag)
{
    int a, i, j;
    int num_w, num_f;//������ʾ��w�����λʱ�����ִ�
    num_w = 0;
    num_f = 0;

    double w_0, w_1, w_lmda, w_m, w_m_prior;//���ųڵ�������
    double lmda;//��ķ��
    double u_lmda;
    double E_0, E_1;
    double Emax;//ÿ�����в�

    double **Field_Prior;//ǰһ������õ��ĵ�λ�ռ��λ

    Field_Prior = (double **)malloc(sizeof(double *)*nRows);
    for (i = 0; i < nRows; i++)
        Field_Prior[i] = (double *)malloc(sizeof(double)*nCols);
    /* ���㳬�ųڵ�������w���ֵ�Ĺ���ֵ */
    w_m_prior = 0;
    w_0 = 1;
    iteration(w_0, Field);//����
    w_1 = 1.375;//Ϊw_1����ֵ
    int count;//�����ƴ�
    while (1)
    {
        num_w++;//�����ִμ���
        for (count = 1; count <= 10; count++)
            iteration(w_1, Field);//����10��

        /* ��Field_Prior���ڵ�����10�ε�Field */
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }

        iteration(w_1, Field);//������11��

        /* ���11�ε������λֵ���10�ε������λֵ�Ĳв��ƽ��ֵ */
        E_0 = 0;//�в�ƽ��ֵ����
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                E_0 = E_0 + fabs(Field[i][j] - Field_Prior[i][j]);
            }
        }
        E_0 = E_0 / (nRows*nCols);

        /* ��Field_Prior���ڵ�����11�ε�Field */
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }

        iteration(w_1, Field);//������12��

        /* ���11�ε������λֵ���12�ε������λֵ�Ĳв��ƽ��ֵ */
        Emax = 0;
        E_1 = 0;//�в�ƽ��ֵ����
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
        w_lmda = 2 / (1 + sqrt(fabs(1 - (u_lmda*u_lmda))));//�����Ҷ����һ��ȡ����ֵ����Ȼ��Щ����»����
        w_m = 1.25*w_lmda - 0.5;

        if (outputFlag)
        {
            fprintf(out, "\n��%d�ֵ�����\n", num_w);
            fprintf(out, "\t����������%d\n", 12 * num_w);
            fprintf(out, "\t�������ӣ�%lf\n", w_m);
            fprintf(out, "\tƽ���в%lf\n", E_1);
            fprintf(out, "\t���в%lf\n", Emax);
        }

        if (!(w_m == w_m))//��������������w_m����һ��������ôw_m == w_m�Ľ����FALSE
        {
            printf("Error: ���������쳣��\n");
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

    /* �����wֵ���е����������λ */
    int e_flag;//�����жϵ����Ƿ����㾫��Ҫ��
    while (1)
    {
        num_f++;
        //��Field_Prior����ǰһ�ε������õ�Field
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                Field_Prior[i][j] = Field[i][j];
            }
        }
        iteration(w_m, Field);

        Emax = 0;
        E_1 = 0;//�в�ƽ��ֵ����
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
        E_1 = E_1 / (nRows*nCols);//�в�

        if (outputFlag)
        {
            fprintf(out, "\n��%d�ֵ�����\n", num_w + num_f);
            fprintf(out, "\t����������%d\n", 12 * num_w + num_f);
            fprintf(out, "\t�������ӣ�%lf\n", w_m);
            fprintf(out, "\tƽ���в%lf\n", E_1);
            fprintf(out, "\t���в%lf\n", Emax);
        }

        e_flag = 0;
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                if (fabs(Field[i][j] - Field_Prior[i][j]) < e)//�ж��ĵ��Ƿ�����������ƾ���
                    e_flag++;
            }
        }
        if (e_flag == nRows*nCols)
            break;
    }


    return 0;
}

/* ����w������������λ�ռ����һ�ε������� */
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
 * ��j����ת��Ϊz����
 * �������Ϊ���������꣨������j������ֵΪz����ľ�������ֵ
 */
double jtoz(int j)
{
    int a;
    double z_j = 0;//z����ֵ

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
 * ��i����ת��Ϊr����
 * �������Ϊ����������꣨������i������ֵΪr����ľ�������ֵ
 */
double itor(int i)
{
    double r_i;//r����ֵ
    if (i <= m2)
        r_i = r1 + r2 - i*(r2 / m2);
    else
        r_i = r1 - (i - m2)*(r1 / m1);
    return r_i;
}

/* ���ص�λֵΪEV�ĵ��z����ֵ */
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

/* ���ص�λֵΪEV�ĵ��r����ֵ */
double evR(double EV, double **Field, int i, int j)
{
    if (fabs(Field[i][j] - Field[i + 1][j]) < ACCURACY)
    {
        return itor(i);
    }
    //�ж�EV�Ƿ���Field[i + 1][j]��Field[i][j]֮��
    else if ((EV - Field[i + 1][j]) / (Field[i][j] - Field[i + 1][j]) >= 0 && ((EV<Field[i + 1][j] && EV>Field[i][j]) || (EV > Field[i + 1][j] && EV < Field[i][j]) || (fabs(EV - Field[i][j]) < ACCURACY)))
    {
        return(itor(i + 1) + ((EV - Field[i + 1][j]) / (Field[i][j] - Field[i + 1][j]))*(itor(i) - itor(i + 1)));
    }
    else
    {
        return FAILURE;
    }
}



/* ���Ƶ�λ�ռ� */
int DrawField(double height, double width, double asp, double vmax, double **Field, TCHAR *DrawFieldName)
{
    outtextxy((int)(0.5*width - 0.5*textwidth(DrawFieldName)), (int)(-3 * EDGEWIDTH), DrawFieldName);
    int a, i, j;
    setfillstyle(BS_HATCHED, HS_DIAGCROSS);
    solidrectangle(-EDGEWIDTH, -EDGEWIDTH, width + EDGEWIDTH, 0);//�����ϱ߽�
    setfillstyle(BS_HATCHED, HS_BDIAGONAL);
    solidrectangle(-EDGEWIDTH, 0, 0, height + EDGEWIDTH);//���������0V����
    setfillstyle(BS_HATCHED, HS_HORIZONTAL);
    solidrectangle(width, 0, width + EDGEWIDTH, height + EDGEWIDTH);//�������ұ�ӫ����
    setfillstyle(BS_HATCHED, HS_VERTICAL);
    solidrectangle(0, height, width, height + EDGEWIDTH);//����z��
    setfillstyle(BS_SOLID);//���ģʽ�ָ�Ĭ��

    /* ����α��ɫ��λ�ֲ� */
    BYTE GrayValue;
    for (i = 0; i < nRows - 1; i++)
    {
        for (j = 0; j < nCols - 1; j++)
        {
            GrayValue = Field[i][j] / vmax * 255;
            PseudoColor(GrayValue, jtoz(j)*asp, height - itor(i)*asp, (jtoz(j + 1) - jtoz(j))*asp, (itor(i) - itor(i + 1))*asp);
        }
    }
    /* ����α��ɫ��λ�ֲ���ͼ�� */
    double legendsize = 0.8*height / 255;//���ڿ���ͼ�����ȣ�ÿ����ɫ����Ϊlegendsize���ܳ���Ϊ0.8*height
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

    /* ���Ƶ缫,��solidrectangle�������ھɰ�<graphics.h>��Ӧ��Ϊ��bar�� */
    setfillcolor(WHITE);//�����ɫ��ɫ
    for (a = 0; a < n - 1; a++)
        solidrectangle(z_v[a] * asp, 0, (z_v[a] + d)*asp, r2*asp);

    return 0;
}

/*
 * α��ɫ����
 * ���ݻҶ�ֵΪһ���������򻭳�α��ɫ
 * value: �Ҷ�ֵ
 * HoriL, VertiL: ����ˮƽ����ֱ�߳�
 * x, y: ������������
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

/* ���Ƶ�λ�� */
int DrawEquLines(double height, double width, double asp, double **Field)
{
    /*
     * ���̣�
     * (��������ʱ֮�⣬������̾�ʹ����������꣨i��j����ϵ)
     * 1. ��ȡ��ǰ��Ҫɨ��ĵ�λֵEV
     * 2. ɨ���λ�ռ��0�У��ҵ�EV����¼��λ��i��j
     *    ����ڵ缫�ϣ������缫��ʼλ��
     *    ����������λ�ռ��ߣ���λ�ռ��߻���֮��ص���0�У���ô����������㿪ʼ��������������
     *    ������û�л����ĵ㣬��ô�����λ�ռ���֮��ֱ�Ӽ�������������
     * 3. �жϵ�ǰһ��Point��״̬��Ȼ��Զ�Ӧ��״̬����ɨ�衣��һ������ʲô����һ���Ͳ���ɨʲô��
     * 4. ��ɨ�赽�ĵ������һ���㣬������õ㣬�����ɨ�衣��¼��ɨ�赽�ĵ㡣
     * 5. һ�������ɨ�赽1���㣬ֱ����ǰһ�������ߣ�
     *    ��ɨ��2���㣬�����ж�����
     *    ��ɨ��3�������ϵ�
     * 6. һֱ���ߵ������߽���ǵ缫��������ǰ����
     * 7. �����ڵ�0����������EV�ĵ㣬ֱ���ѵ�0�������ꡣ
     * 8. ����һ��EV����������
     */
    int a, i, j;
    double EV;//��ǰ��ɨ��ĵ�λֵ
    int jofi0 = 1;//��0���е�j������ɨ���0���еĵ�λ

    //0Ϊǰһ��ɨ�赽�ĵ㣬1Ϊ��ǰɨ�赽�ĵ㣬2Ϊ��һ����
    tPoint point[] = { { 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 } };
    //��������¶���ĵ�,0Ϊ�����ǰһ���㣬1Ϊ�ݴ棬2Ϊ�������һ����
    tPoint ptemp[] = { { 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 } };
    tPoint pzero[] = { { 0,0,0,0,0 } };//ר���������ù���
    int flagget = 0;//�Ե�0�еĵ�ǰɨ�裬ɨ�赽��ı�־
    int flagi0get = 0;//��������0�е�ȫ������ɨ�裬ɨ�赽��ı�־
    int abegin = 0;//�ӵ�abegin���缫��ʼɨ��
    int CountV = 0;
    int DrawLine = 1;
    int nPoints = 0;//���ĵ��ܱ�ɨ�赽�ĵ����

    for (CountV = 0; CountV < m; CountV++)//ɨ���CountV����λֵ�����Ƶ�λ��
    {
        EV = ev[CountV];
        fprintf(out, "\n��λֵΪ%.2lfʱ��\n", EV);

        jofi0 = 1;//��1��ʼ�����Ե�0�б߽�
        flagi0get = 0;//������0��ɨ���־����
        abegin = -1;//�缫ɨ����ʼλ������
        while (jofi0 <= nCols - 2)//��0�д�1һֱɨ�赽nCols-2
        {
            flagget = 0;
            i = 0;
            j = 0;
            if (abegin != n - 2) //�����n-1���缫��ɨ��û��ͷ
            {
                //��ɨ������缫��ȷ��Ҫɨ��ĵ��Ƿ��ڵ缫�ϡ����Ե�n-1���缫���߽磩
                for (a = abegin + 1; a <= n - 2; a++)
                {
                    //�����ǰ�缫��ɨ�赽��
                    if (fabs(Field[0][jofi0] - EV) < ACCURACY && (jofi0 == j_v[a] || jofi0 == (j_v[a] + 1)))
                    {
                        point[1].status = is_H_E;
                        point[1].ip = m2;
                        point[1].jp = jofi0;
                        point[1].z = evZ(EV, Field, m2, jofi0) + 0.5*d;
                        point[1].r = itor(m2);
                        flagget = 1;
                        abegin = a;
                        jofi0++;//ÿ���缫ռ����������㣬������ڵ缫�ϣ���ô�´�ɨ��Ϳ�������һ�������
                        break;
                    }
                }//for(a = 0; a <= n - 2; a++)ɨ��缫
            }//if (abegin != n-2)

            if (flagget == 0) //����㲻�ڵ缫��
            {
                //�����ǰ�ǵ缫���ϱ߽�λ��ɨ�赽��
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

            flagi0get = flagi0get + flagget;//ÿɨ����һ�ּ�һ����

            if (flagget)
            {
                fprintf(out, "\nɨ��%.2lf V��λʱ��(z, r)����Ϊ(%lf, %lf)��(i, j)����Ϊ(%d, %d)��ʼ�ĵ�λ�ߣ�\n", EV, point[1].z, point[1].r, point[1].ip, point[1].jp);

            }

            /* ��ʼ���ɨ�貢���Ƶ�λ�� */
            DrawLine = 1;
            while (DrawLine == 1 && flagget == 1)//��0��ɨ���������㻭�߱�־ʱ����
            {
                if (fabs(point[1].r - FAILURE) < ACCURACY || fabs(point[1].z - FAILURE) < ACCURACY)
                {
                    printf("Error����ǰ(i, j)����Ϊ(%d, %d)�ĵ��(z, r)������㲻��ȷ���������BUG���˳�\n", point[1].ip, point[1].jp);
                    system("pause");
                    exit(1);
                }

                nPoints = 0;//���ĵ��ܱ�ɨ�赽�ĵ�����
                switch (point[1].status)//���ݵ�ǰ���״̬������ɨ��
                {
                case is_P:
                    //ɨ��������
                    for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                    {
                        for (i = point[1].ip - 1; i <= point[1].ip; i++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //ɨ��������
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i += 2)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //ɨ���ı������е�
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
                    //ɨ��������
                    i = point[1].ip;
                    for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                    {
                        nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                    }
                    //ɨ��������
                    for (i = point[1].ip; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //ɨ���ı������е�
                    for (i = point[1].ip; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H:
                    //ɨ��������
                    for (j = point[1].jp; j <= point[1].jp + 1; j++)
                    {
                        for (i = point[1].ip - 1; i <= point[1].ip; i++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                    }
                    //ɨ��������
                    j = point[1].jp;
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i += 2)
                    {
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                    }
                    //ɨ���ı������е�
                    for (i = point[1].ip - 1; i <= point[1].ip + 1; i++)
                    {
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_P_T:
                    if (nPoints)//���ɨ����ĵ㲻Ϊ0��˵������߽�����յ���������
                    {
                        DrawLine = 0;//�����ڱ߽���ʱ��˵����λ�ռ���������߽���
                        break;
                    }
                    else //���ǡ�ôӵ�0��ĳ������㿪ʼ
                    {
                        //ɨ��������
                        i = point[1].ip;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ��һ����
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ��ױ�������
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H_T:
                    if (nPoints)//���ɨ����ĵ㲻Ϊ0��˵������߽�����յ���������
                    {
                        DrawLine = 0;//�����ڱ߽���ʱ��˵����λ�ռ���������߽���
                        break;
                    }
                    else //����ӵ�0�������߻�ĳ���缫���߿�ʼ
                    {
                        //ɨ��������
                        i = point[1].ip;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ��һ����
                        i = point[1].ip + 1;
                        j = point[1].jp;
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        //ɨ��׽�������
                        i = point[1].ip + 1;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    break;
                case is_H_E:
                    if (nPoints)//���ɨ����ĵ㲻Ϊ0��˵������߽�����յ���������
                    {
                        DrawLine = 0;//�����ڱ߽���ʱ��˵����λ�ռ���������߽���
                        break;
                    }
                    else //����ӵ�0�������߻�ĳ���缫���߿�ʼ
                    {
                        //ɨ��������
                        i = point[1].ip;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ��һ����
                        i = point[1].ip + 1;
                        j = point[1].jp;
                        nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        //ɨ��׽�������
                        i = point[1].ip + 1;
                        for (j = point[1].jp; j <= point[1].jp + 1; j++)
                        {
                            nPoints = PointsProcessing(nPoints, is_P, point, ptemp, EV, Field, i, j);
                        }
                    }
                    if (nPoints == 0)//�����ӽ��缫�����λ���Ҳ�����λ�㣬����һȦ��Χ
                    {
                        //ɨ��������
                        i = point[1].ip;
                        for (j = point[1].jp - 1; j <= point[1].jp + 2; j += 3)
                        {
                            nPoints = PointsProcessing(nPoints, is_V, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ��һ����
                        i = point[1].ip + 1;
                        for (j = point[1].jp - 1; j <= point[1].jp + 1; j += 2)
                        {
                            nPoints = PointsProcessing(nPoints, is_H, point, ptemp, EV, Field, i, j);
                        }
                        //ɨ���ĸ���
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
                    DrawLine = 0;//���ҡ��±߽�㲻��������㣬ֻ�������յ�
                    fprintf(out, "(z, r)����Ϊ��(%lf, %lf)\t(i, j)����Ϊ��(%d, %d)���յ㡿\n", point[1].z, point[1].r, point[1].ip, point[1].jp);
                    PointsEqu(point, 0, pzero, 0);//�ݴ�ġ���һ���㡱����
                    PointsEqu(ptemp, 0, pzero, 0);
                    break;
                default:
                    printf("Error��Point(%d, %d)��status����\n", point[1].ip, point[1].jp);
                    break;
                }//switch

                if (DrawLine)//�������ߵ�ʱ��������
                {
                    /* ����ɨ�赽�ĵ�������������� */
                    if (nPoints >= 3)
                    {
                        printf("ɨ���λ%.2fVʱ��(i, j)����Ϊ(%d, %d)��λ�ø�������%d����λ�㣬��������������\n", EV, point[1].ip, point[1].jp, nPoints + 2);
                        system("pause");
                        break;
                    }
                    else if (nPoints == 2)
                    {
                        printf("����һ��ͬһ����������������λ������\n");
                        if (ptemp[2].ip > point[2].ip)
                        {
                            PointsEqu(ptemp, 1, point, 2);
                            PointsEqu(point, 2, ptemp, 2);
                        }
                        else
                        {
                            PointsEqu(ptemp, 1, ptemp, 2);
                        }

                        fprintf(out, "(z, r)����Ϊ��(%lf, %lf)\t(i, j)����Ϊ��(%d, %d)\n", point[1].z, point[1].r, point[1].ip, point[1].jp);
                        fprintf(out, "(z, r)����Ϊ��(%lf, %lf)\t(i, j)����Ϊ��(%d, %d)\n", point[2].z, point[2].r, point[2].ip, point[2].jp);

                        line(point[1].z*asp, height - point[1].r*asp, point[2].z*asp, height - point[2].r*asp);
                        line(point[2].z*asp, height - point[2].r*asp, ptemp[1].z*asp, height - ptemp[1].r*asp);
                        PointsEqu(point, 0, point, 1);
                        PointsEqu(ptemp, 0, point, 2);
                        PointsEqu(point, 1, ptemp, 1);
                    }
                    else if (nPoints == 1)
                    {
                        fprintf(out, "(z, r)����Ϊ��(%lf, %lf)\t(i, j)����Ϊ��(%d, %d)\n", point[1].z, point[1].r, point[1].ip, point[1].jp);

                        line(point[1].z*asp, height - point[1].r*asp, point[2].z*asp, height - point[2].r*asp);
                        PointsEqu(point, 0, point, 1);
                        PointsEqu(point, 1, point, 2);
                    }
                    else if (nPoints == 0 && DrawLine != 0)//û�е㣬�Ҳ�Ϊ�˳��ж�
                    {
                        DrawLine = 0;
                        printf("ɨ���λ%.2fVʱ��(i, j)����Ϊ(%d, %d)��λ�ø���û����һ����λ��\n", EV, point[1].ip, point[1].jp);
                        break;
                    }
                }//if(DrawLine)�����ߵ�ʱ�������
            }//while(DrawLine==1&&flagget==1)


            PointsEqu(point, 0, pzero, 0);//����һ����λ�ߣ����ù���
            PointsEqu(ptemp, 0, pzero, 0);

            if (flagget == 1 && (point[1].status == is_H_T || point[1].status == is_H_E || point[1].status == is_P_T) && point[1].jp > jofi0)//������ĵ�λ�����ص���0��
            {
                jofi0 = point[1].jp;
            }

            if (jofi0 >= nCols - 2 && flagi0get == 0 && fabs(EV - v[n - 1]) > ACCURACY)
            {
                printf("û���ҵ���λΪ%lf�ĵ㣬�޷����Ƹõ�λ�ĵ�λ�ռ���\n", EV);//û�ҵ�������һ����λ����ɨ��

            }

            jofi0++;//�����߽���ɨ���0�в�����������ĵ�Ҳ����EV��ֱ����0�����е㶼ɨ����
        }//while(jofi0<=nCols-2)
    }//for(CountV)

    return 0;
}

/* ��������߻��Ƶ�λ�ߵĹ����жԵ�λ��Ĵ������ */
int PointsProcessing(int nPoints, int status, tPoint *point, tPoint *ptemp, double EV, double **Field, int i, int j)
{
    //����ɹ�ɨ�赽������ݴ浽ptemp[1]�У��ٽ����ж�
    if ((status == is_V && (((Field[i][j] - EV > ACCURACY) && (Field[i + 1][j] - EV < -ACCURACY)) || ((Field[i][j] - EV < -ACCURACY) && (Field[i + 1][j] - EV > ACCURACY)))) || (status == is_H && (((Field[i][j] - EV > ACCURACY) && (Field[i][j + 1] - EV < -ACCURACY)) || ((Field[i][j] - EV < -ACCURACY) && (Field[i][j + 1] - EV > ACCURACY)))) || (status == is_P && (fabs(Field[i][j] - EV) < ACCURACY)))
    {
        ptemp[1].status = isPointEdge(i, j);
        if (ptemp[1].status == FAILURE)
        {
            printf("Error������(%d, %d)�����ĵ�λ��ʱ����status����\n", i, j);
        }
        else if (ptemp[1].status == 0)//���ڱ߽���
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

        //�缫���½ǵĵ��״̬�����ж�
        if (ptemp[1].ip == m2&&ptemp[1].status == is_V_L&&ptemp[1].jp != 0)
        {
            ptemp[1].status = is_H_E;
        }


        //�����һ������ǰһ���㣨�����������ͬ���Ƚ�һ������ip,jp,status������Ա��
        if (((point[0].ip == ptemp[1].ip) && (point[0].jp == ptemp[1].jp) && (point[0].status == ptemp[1].status)) || ((ptemp[0].ip == ptemp[1].ip) && (ptemp[0].jp == ptemp[1].jp) && (ptemp[0].status == ptemp[1].status)))
        {
            return nPoints;
        }
        //�缫�����ϵĵ�������⣬��Ҫ�ٵ����Ƚ�,ֻҪ����ͬһ���缫�����ϣ�����λ����ζ�����ͬһ����
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
        else if (nPoints == 0)//�������ɨ�赽�ĵ�һ����λ�㣬��ptemp[1]����point[2]
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
        else if (nPoints == 1)//���Ѿ�ɨ�赽һ���㣬��ô�ٴ�ɨ�赽�ĵ�浽ptemp[2]
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

/* �ж��������(i, j)������ĵ��Ƿ��ڱ߽��� */
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
        return 0;//�������ϵ�����������ڱ߽��ϣ��򷵻�0
    }
    return FAILURE;//���������δ���أ�˵���д���
}

/* �����ݸ�ֵ����p2[i2]�����ݸ���p1[i1] */
int PointsEqu(tPoint *p1, int i1, tPoint *p2, int i2)
{
    p1[i1].status = p2[i2].status;
    p1[i1].ip = p2[i2].ip;
    p1[i1].jp = p2[i2].jp;
    p1[i1].z = p2[i2].z;
    p1[i1].r = p2[i2].r;
    return 0;
}


/* ��ʼ���������ղ�ֵ�������ϵ�λ�Ļ������� */
int InitDrawLagrange(double height, double width, double asp, double vmax, double **Field)
{
    int a;
    /* ������������ */
    solidrectangle(-0.2*EDGEWIDTH, 0, 0, height);//������
    solidrectangle(-0.2*EDGEWIDTH, height, width, 0.2*EDGEWIDTH + height);
    POINT tripoints01[] = { { -0.1*EDGEWIDTH,-2 * EDGEWIDTH },{ -0.5*EDGEWIDTH,0 },{ 0.5*EDGEWIDTH,0 } };
    POINT tripoints02[] = { { 2 * EDGEWIDTH + width,-0.1*EDGEWIDTH + height },{ width,-0.5*EDGEWIDTH + height },{ width,0.5*EDGEWIDTH + height } };
    solidpolygon(tripoints01, 3);//��ͷ
    solidpolygon(tripoints02, 3);
    setlinecolor(DARKGRAY);//����������                      
    TCHAR str03[20], str04[20], str05[20];//����ע��
    TCHAR DrawLagrangeName[50];//ͼ��
    _stprintf(str03, _T("O"));
    _stprintf(DrawLagrangeName, _T("�������ղ�ֵ�������ϵ�λ�����ơ��������� -- ���ϵ�λ������"));
    outtextxy(-0.5*textwidth(str03), 0.5*textheight(str03) + height, str03);
    outtextxy((int)(0.5*width - 0.5*textwidth(DrawLagrangeName)), (int)(-3 * EDGEWIDTH), DrawLagrangeName);
    for (a = 1; a <= AXESGRID_X_NUM; a++)
    {
        line(a*(width / AXESGRID_X_NUM), 0, a*(width / AXESGRID_X_NUM), height);
        _stprintf(str04, _T("%.1f"), a*((width / asp) / AXESGRID_X_NUM));//����������λ��
        outtextxy(-0.5*textwidth(str04) + a*(width / AXESGRID_X_NUM), 0.5*textheight(str04) + height, str04);
    }
    for (a = 0; a < AXESGRID_Y_NUM; a++)
    {
        line(0, a*(height / AXESGRID_Y_NUM), width, a*(height / AXESGRID_Y_NUM));
        _stprintf(str05, _T("%.1fV"), (AXESGRID_Y_NUM - a)*(vmax / AXESGRID_Y_NUM));//���������ϵ�λ
        outtextxy(-1.1*textwidth(str05), -0.5*textheight(str05) + a*(height / AXESGRID_Y_NUM), str05);
    }
    return 0;
}
/*
 * �������ղ�ֵ���������ϵ�λ����
 * �����б�
 * axis: �洢���ϵ�λ������ָ��
 * height, width: ������������ĸߺͿ�
 * asp: ��������
 * vmax: ��λ�ռ�������λֵ
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
            return j;//x������ĳ���������
    }
    return 0;//x���겻���������
}

/*
 * ��¼��һЩ˵��
 * 1. �������е������������������ϵ��
 * һ�������������(i,j)��������ֱ�Ϊ��i->���£�j->���ң�����Ϊint�ͣ�
 * �ڶ����Ǿ�������(x,y)��������ֱ�Ϊ��x->���ң�y->���£�����Ϊdouble�ͣ�
 * �������Ǿ�������(z,r)��������ֱ�Ϊ��z->���ң�r->���ϡ�����Ϊdouble�ͣ�
 * ��Ҫ��ϸע�����ַ���
 *
 * 2. �������ɨ�����ߵ�˵��
 *
 * ���ɨ�����ߣ���˵�����ҵ�һ�����Ƶ���EV�ĵ㣬Ȼ��ɨ��������ܱߣ����ܱ��ҵ��ȵ�λ�����ߣ�������һ�����һ�����������λ�ռ���
 * ���ɨ��ĳ������ܱߣ����Խ��õ����point[1]�����ڵ�λ�ռ�����һ���������ߣ�����һ������������ܱ��ҵ����������ȵ�λ�㣺
 * һ����֮ǰ�����߹������Ѿ���point[1]���ӹ��ĵ㣬����point[0]
 * һ���ǻ�δ��point[1]���ӹ��ĵ㣬����point[2]����һ���Ϳ�������point[1]��point[2]
 * ���ڰ��������£����ܻ��ڵ�λ�ռ��ߵļ�����ֵ������ȵ�λ��
 *
 * ���ڵȵ�λ����������������ϣ�Ҳ����ǡ����������ϣ����Զ��ܱߵ�ɨ����Էֳ� ɨ���С�ɨ���С�ɨ�趥�� �����ַ�ʽ
 * �����ĵ��״̬Ҳ��Ϊ����λ�ò�ͬ���� is_P is_V is_H is_H_T is_H_B is_V_L is_V_R ��7��״̬���Լ��������� is_P_T is_H_E ���������
 * �����ͼչʾ�˼��������
 * ͼ�У� | ��ʾ��ɨ����У� -- ��ʾ��ɨ����У� o ��ʾ��ɨ��Ķ��㣬 # �����ĵ㣬 * * �ǲ���Ҫɨ��������ߣ����Կ�������
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
 *   * * * * * # * * * * * (�߽���)
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