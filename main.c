/*Code by Cheng Xin*/
#pragma warning (disable:4996) //��Ҫ���VS��warning
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
//#include<graphics.h>

#define FILEPATH "C:\\BIT\\CAD\\" //�����������ݺ�������ݵ��ļ�·��
#define FILEIN "in.dat" //�������������ļ���
#define FILEOUT "out.dat" //������������ļ���

/*����ȫ�ֱ���*/
FILE *data;
FILE *out;

double **field;//�糡��λ��SOR������ã�
double *axis;//���ϵ�λ���������ղ�ֵ����ã�

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
double dv;//Ҫ��ɨ�������ȵ�λ�ߵĵ�λ���ֵ
int m;//Ҫ��ɨ�������ȵ�λ�ߵĵ�λֵ�ĸ���
double *ev;//Ҫ��ɨ�������ȵ�λ�ߵĵ�λֵ

int i;//һ������ѭ��ʱ�缫�����������������Ҳ������������
int j;//��ʾ�糡�еĵ�j+1��
int k;//��ʾ�糡�еĵ�k+1��
int row = 0;//�糡��λ��ά���������
int col = 0;//�糡��λ��ά���������
double *z_v;//�缫����һ�ߵ�z����ֵ
double **c1, **c2, **c3, **c4, **c0;//c1, c2, c3, c4�ֱ���SOR���е�ϵ��(coefficient)

double EV;//��ɨ��ĵ�λֵ
double z_for = 0;//֮ǰһ�����z����
double r_for = 0;//֮ǰһ�����r����

/* 
 * ȷ��һ�����ɨ����������������߶ε����ܣ�һ����ҵ���2�����ȵ�λ�㣬һ����֮ǰ�Ѿ��ҵ��������ߵĵ㣬��һ���Ǵ����ߵĵ㣬����Ҫ�����һ����
 * ��������£����а���ʱ�����ҵ���3�����ȵ�λ�㣬��ʱ��Ҫ�ж��ĸ���Ҫ�����ϣ��ĸ���Ҫ������	
 * ���������У�zp[0]�浱ǰ������꣬zp[1]����һ��������꣬�а���ļ�������»��õ�z[2]����������������ơ�
 * Ϊ�˷�ֹ��������µ�������������С��Ϊ4
 */ 
double zp[4];//ɨ����z����
double rp[4];//ɨ����r����
int jp[4];//ɨ��������߶ε���ʼ�����λ��
int kp[9];//ɨ��������߶ε���ʼ�����λ��
int v_flag;//�����ж��Ƿ�����Ӧ��Χ��ɨ�赽�˴�������λ������¼ɨ�赽�ķ��������ĵ�ĸ���
int point[4];//point[0]��ʾĳ������ܴ��ڵ�����������֣���������ߡ�r���򡿡�����ĺ�ߡ�z���򡿡�ĳ������㡾point����


/* �����ӳ��� */
int iteration(double w);//����һ�εĺ���
double coordinate_z(int kc);//���λ���z����ֵ
double coordinate_r(int jc);//���λ���r����ֵ
double coordinate_zp(int jm, int km);//���λ���z����ֵ
double coordinate_rp(int jm, int km);//���λ���r����ֵ
int direction_r(int jm, int km);//��������ߡ�r����
int direction_z(int jm, int km);//����ĺ�ߡ�z����
int direction_point(int jm, int km); //ĳ������㡾point��
int judgelocation(double x);//�ж�ĳ�������ϵ�λ��


int main()
{
    char fname[128] = "C:\\BIT\\CAD\\in.dat";//�ļ�·��
    //char fname[128];
    //strcat(fname, FILEPATH);
    //strcat(fname, FILEIN);

    errno_t err1;
    err1 = fopen_s(&data, fname, "r");

    if (data == NULL)
    {
        printf("�޷����ļ�:%s\n", fname);
        system("pause");
        exit(1);
    }
    //Ϊn��ֵ
    fscanf(data, "n\t�缫����\n");
    fscanf(data, "%d\n", &n);
    //Ϊd��ֵ
    fscanf(data, "\nd\t�缫���\n");
    fscanf(data, "%lf\n", &d);
    //Ϊz��ֵ
    z = (double *)malloc(n * sizeof(double));
    fscanf(data, "\nz\t���ڵ缫֮��ľ���\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%lf ", &z[i]);
    //Ϊstep��ֵ
    step = (int *)malloc(n * sizeof(int));
    fscanf(data, "\n\nstep\t���ڵ缫֮��Ҫ���ֵĲ�����\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%d ", &step[i]);
    //Ϊv��ֵ
    v = (double *)malloc(n * sizeof(double));
    fscanf(data, "\n\nv\t�缫��λ\n");
    for (i = 0; i < n; i++)
        fscanf(data, "%lf ", &v[i]);
    //Ϊr1��ֵ
    fscanf(data, "\n\nr1\t�缫�ڿ׾��뾶\n");
    fscanf(data, "%lf\n", &r1);
    //Ϊm1��ֵ
    fscanf(data, "\nm1\tr1��Χ�ڵȲ������ֵ�������\n");
    fscanf(data, "%d\n", &m1);
    //Ϊr2��ֵ
    fscanf(data, "\nr2\t�ӵ缫�ڿױߵ���ձ߽紦�ľ������\n");
    fscanf(data, "%lf\n", &r2);
    //Ϊm2��ֵ
    fscanf(data, "\nm2\tr2��Χ�ڵȲ������ֵ�������\n");
    fscanf(data, "%d\n", &m2);
    //Ϊe��ֵ
    fscanf(data, "\ne\t�������ƾ���\n");
    fscanf(data, "%lf\n", &e);
    //Ϊnst��ֵ
    fscanf(data, "\nnst\t�����ӡ�ռ��λʱ�����������\n");
    fscanf(data, "%d\n", &nst);
    //Ϊins��ֵ
    fscanf(data, "\nins\t���ϵ�λ���Ⱦ��ֵʱ�Ĳ�����\n");
    fscanf(data, "%d\n", &ins);
    //Ϊdv��ֵ
    fscanf(data, "\ndv\tҪ��ɨ�������ȵ�λ�ߵĵ�λ���ֵ\n");
    fscanf(data, "%lf\n", &dv);
    //Ϊm��ֵ
    fscanf(data, "\nm\tҪ��ɨ�������ȵ�λ�ߵĵ�λֵ�ĸ���\n");
    fscanf(data, "%d\n", &m);
    //Ϊev��ֵ
    if (m > 0)
    {
        ev = (double *)malloc(m * sizeof(double));
        fscanf(data, "\nev\tҪ��ɨ�������ȵ�λ�ߵĵ�λֵ\n");
        for (i = 0; i < m; i++)
            fscanf(data, "%lf ", &ev[i]);
    }

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
    	printf("�ɹ���ȡ%s",fname);
    }

    row = m1 + m2 + 1;//��������
    for (i = 0; i < n; i++)
        col = col + step[i];
    col = col + n;//��������

                  //�����糡��ά����
    field = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        field[j] = (double *)malloc(sizeof(double)*col);

    /*����ֵ*/
    //�Ƚ���λȫ��Ϊ0
    for (j = 0; j < row; j++)
    {
        for (k = 0; k < col; k++)
        {
            field[j][k] = 0;
        }
    }

    /*���缫��ֵ���Լ��ò�ֵ����ÿ���㸳��ֵ*/
    int l;//�������缫֮�������ļ���
    double dif;//������֮��Ĳ�ֵ

    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//��i+1���缫�Ŀ���һ��(�����һ����缫�����б�ʾ)
        for (j = 0; j < m2 + 1; j++)
        {
            if (k < col)
            {
                field[j][k - 1] = v[i];
                field[j][k] = field[j][k - 1];//�������һ���缫��ÿ���缫��ֵ(���һ���缫��һ�б�ʾ)
            }
            if (i == 0)
                dif = v[i] / step[i];
            else
                dif = (v[i] - v[i - 1]) / step[i];
            for (l = k - step[i]; l < k - 1; l++)//���缫�������(�������缫)
            {
                field[j][l] = field[j][l - 1] + dif;
            }
        }
    }
    for (j = 0; j < row; j++)
        field[j][col - 1] = v[n - 1];//�����һ���缫��ֵ

                                     //����缫����һ�ߵ�z����
    z_v = (double *)malloc(n * sizeof(double));
    for (i = 0, z_v[0] = 0; i < n; i++)
    {
        if (i == 0)
            z_v[i] = z_v[i] + z[i];
        else
            z_v[i] = z_v[i - 1] + z[i] + d;
    }

    /*Ԥ�����ÿ�����Ӧ��ϵ��c1,c2,c3,c4,c0��ֵ���ֱ�����Ӧ�Ķ�ά�����У����ڵ���ʱ���м���*/
    double h1, h2, h3, h4;//h1,h2,h3,h4�ֱ���ĳ��������Χ�ĸ���ľ���
    double r0;//ĳ������ױߵľ���

              //����c1����,c2����,c3����,c4,c0����
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

    //�Ƚ�c1,c2,c3,c4,c0ȫ��Ϊ0
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

    //���ݾ����������c1��c2
    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//��i+1���缫�Ŀ���һ��(�����һ����缫�����б�ʾ)
        for (j = 1; j < row; j++)
        {
            for (l = k - step[i]; l < k - 1; l++)//���缫�������(�������缫)
            {
                h1 = z[i] / step[i];
                h2 = z[i] / step[i];
                c1[j][l] = 2 / (h1*(h1 + h2));
                c2[j][l] = 2 / (h2*(h1 + h2));//���缫֮�����������е�
            }
            if (j>m2&&k < col)
            {
                h1 = z[i] / step[i];
                h2 = d;
                c1[j][k - 1] = 2 / (h1*(h1 + h2));
                c2[j][k - 1] = 2 / (h2*(h1 + h2));//��i+1���缫����һ����r1��Χ�ڵĵ�(�ų����һ���缫)
                h1 = d;
                h2 = z[i + 1] / step[i + 1];
                c1[j][k] = 2 / (h1*(h1 + h2));
                c2[j][k] = 2 / (h2*(h1 + h2));//��i+1���缫����һ����r1��Χ�ڵĵ�(�ų����һ���缫)
            }
        }
    }

    //���ݾ����������c3��c4
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

    /* ������� */	
    char outname[128] = "C:\\BIT\\CAD\\out.dat";
//    char outname[128];
//    strcat(outname, FILEPATH);
//    strcat(outname, FILEOUT);

    errno_t err2;
    err2 = fopen_s(&out, outname, "w");
    //������������ݡ�
    fprintf(out, "�缫����n:");
    fprintf(out, "%d\n", n);
    fprintf(out, "�缫���d:");
    fprintf(out, "%lf\n", d);
    fprintf(out, "���ڵ缫֮��ľ���z:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%lf ", z[i]);
    fprintf(out, "\n���ڵ缫֮��Ҫ���ֵĲ�����step:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%d ", step[i]);
    fprintf(out, "\n�缫��λv:\n");
    for (i = 0; i < n; i++)
        fprintf(out, "%lf ", v[i]);
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
        fprintf(out, "Ҫ��ɨ�������ȵ�λ�ߵĵ�λ���ֵdv:");
        fprintf(out, "%lf\n", dv);
    }
    else
    {
        fprintf(out, "Ҫ��ɨ�������ȵ�λ�ߵĵ�λֵ�ĸ���m:");
        fprintf(out, "%d\n", m);
        fprintf(out, "Ҫ��ɨ�������ȵ�λ�ߵĵ�λֵev:\n");
        for (i = 0; i < m; i++)
            fprintf(out, "%lf ", ev[i]);
    }
    //������������
    fprintf(out, "\n���r������������������:\n");
    for (j = 0; j <row; j++)
    {
        fprintf(out, "��%d����:\t", j);
        fprintf(out, "%lf\n", coordinate_r(j));
    }
    fprintf(out, "\n���z������������������:\n");
    for (k = 0; k < col; k++)
    {
        fprintf(out, "��%d����:\t", k);
        fprintf(out, "%lf\n", coordinate_z(k));
    }

    /*����糡�еĵ�λֵ*/
    int num_w, num_f;//������ʾ��w�����λʱ�����ִ�
    num_w = 0;
    num_f = 0;

    double w_0, w_1, w_lmda, w_m, w_m_for;//���ųڵ�������
    double lmda;//��ķ��
    double u_lmda;
    double E_0, E_1;
    double Emax;//ÿ�����в�

    double **field_for;//ǰһ������õ��ĵ糡��λ

    field_for = (double **)malloc(sizeof(double *)*row);
    for (j = 0; j < row; j++)
        field_for[j] = (double *)malloc(sizeof(double)*col);
    //���㳬�ųڵ�������w���ֵ�Ĺ���ֵ
    w_m_for = 0;
    w_0 = 1;
    iteration(w_0);//����
    w_1 = 1.375;//Ϊw_1����ֵ
    int count;//�����ƴ�
    while (1)
    {
        num_w++;//�����ִμ���
        for (count = 1; count <= 10; count++)
            iteration(w_1);//����10��

                           //��field_for���ڵ�����10�ε�field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }

        iteration(w_1);//������11��

                       //���11�ε������λֵ���10�ε������λֵ�Ĳв��ƽ��ֵ
        E_0 = 0;//�в�ƽ��ֵ����
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                E_0 = E_0 + fabs(field[j][k] - field_for[j][k]);
            }
        }
        E_0 = E_0 / (row*col);

        //��field_for���ڵ�����11�ε�field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }

        iteration(w_1);//������12��

                       //���11�ε������λֵ���12�ε������λֵ�Ĳв��ƽ��ֵ
        Emax = 0;
        E_1 = 0;//�в�ƽ��ֵ����
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

        fprintf(out, "\n��%d�ֵ�����\n", num_w);
        fprintf(out, "\t����������%d\n", 12 * num_w);
        fprintf(out, "\t�������ӣ�%lf\n", w_m);
        fprintf(out, "\tƽ���в%lf\n", E_1);
        fprintf(out, "\t���в%lf\n", Emax);

        if (fabs((w_m - w_m_for) / (2 - w_m)) < 0.05)
            break;
        else
        {
            w_m_for = w_m;
            w_1 = w_m;
        }

    }

    //�����wֵ���е����������λ
    int e_flag;//�����жϵ����Ƿ����㾫��Ҫ��
    while (1)
    {
        num_f++;
        //��field_for����ǰһ�ε������õ�field
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                field_for[j][k] = field[j][k];
            }
        }
        iteration(w_m);

        Emax = 0;
        E_1 = 0;//�в�ƽ��ֵ����
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
        E_1 = E_1 / (row*col);//�в�

        fprintf(out, "\n��%d�ֵ�����\n", num_w + num_f);
        fprintf(out, "\t����������%d\n", 12 * num_w + num_f);
        fprintf(out, "\t�������ӣ�%lf\n", w_m);
        fprintf(out, "\tƽ���в%lf\n", E_1);
        fprintf(out, "\t���в%lf\n", Emax);

        e_flag = 0;
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < col; k++)
            {
                if (fabs(field[j][k] - field_for[j][k]) < e)//�ж��ĵ��Ƿ�����������ƾ���
                    e_flag++;
            }
        }
        if (e_flag == row*col)
            break;
    }
    //��ӡ�����ϵ�λ
    fprintf(out, "\n��ӡ�����ϵĵ�λ�������NSTΪ%d:\n", nst);
    for (j = 0; j < row; j = j + nst)
    {
        fprintf(out, "\n");
        for (k = 0; k < col; k = k + nst)
        {
            fprintf(out, "%lf \t", field[j][k]);
        }
    }


//    
///**********************************��ʼ��ͼ**********************************************/
//    double width;//���Ƶ糡���
//    double height;//���Ƶ糡�߶�
//    int asp;//��������
//    for (i = 0, width = 0; i < n; i++)
//    {
//        width = width + z[i];
//    }
//    asp = 640 / (r1 + r2);
//    width = (width + d*(n - 1))*asp;
//    height = (r1 + r2)*asp;
//
//    initgraph(width + 2, height + 2, SHOWCONSOLE);//��ʼ����ͼ����
//    line(0, 0, width, 0);//�����ϱ߽�
//    line(0, 0, 0, height);//���������0V����
//    line(width, 0, width, height);//�������ұ�ӫ����
//    line(0, height, width, height);//����z��
//    for (i = 0; i < n - 1; i++)
//        solidrectangle(z_v[i] * asp, 0, (z_v[i] + d)*asp, r2*asp);//���Ƶ缫,��solidrectangle�������ھɰ�<graphics.h>��Ӧ��Ϊ��bar��
//
//    int i_m;//ɨ���λ�����ƴ�
//    //��һ������ɨ�������ȵ�λֵ�ķ�ʽ
//    if (m == 0)
//    {
//        m = v[n - 1] / dv;
//        ev = (double *)malloc(m * sizeof(double));
//        for (i_m = 0; i_m < m; i_m++)
//            ev[i_m] = (i_m + 1)*dv;
//    }
//
//    /*************����λ��*********************/
//    fprintf(out, "\n\n\n��ӡ��λ�ߵĵ�λֵ�͸�������(z,r)\n", EV);
//    int firstrow;//��һ������ɨ���λֵ�ĵ�ĸ���
//    int firstrow_i, firstrow_j;
//    for (i_m = 0; i_m < m; i_m++)
//    {
//        EV = ev[i_m];
//
//        if (EV == v[n - 1])	continue;//���ܵ������һ���缫��ֵ
//
//        fprintf(out, "\n��λֵΪ%.1lfʱ��\n", EV);
//
//        firstrow = 0;
//        for (k = 0; k<col - 1; k++)
//        {
//            if ((field[0][k] <= EV&&field[0][k + 1] > EV) || (field[0][k] >= EV&&field[0][k + 1] < EV))//������1��������λ�õĸ����������ǿ���ӫ�����������
//            {
//                firstrow++;
//            }
//        }
//
//        int jm, km;//���ڼ��жϹ���
//
//        for (firstrow_i = 1; firstrow_i <= firstrow; firstrow_i++)
//        {
//            point[0] = 0;
//            v_flag = 0;
//            firstrow_j = 0;
//            for (k = 0; k<col - 1; k++)
//            {
//                if ((field[0][k] <= EV&&field[0][k + 1] > EV) || (field[0][k] >= EV&&field[0][k + 1] < EV))//������1��������λ�á������ǿ���ӫ�����������
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
//                //Ѱ�ҵ�2�����λ��
//                v_flag = 0;
//                if (field[0][k] == EV)//��1�����1�����
//                {
//                    //�Ƿ���������
//                    for (jm = jp[0], km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                    //�Ƿ��ں����
//                    for (jm = jp[0] + 1, km = kp[0] - 1; km <= kp[0]; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                    //�Ƿ����������
//                    for (jm = jp[0] + 1, km = kp[0] - 1; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_point(jm, km) == -1)	continue;
//                    }
//                }
//                else//��1�����2�����
//                {
//                    //�Ƿ���������
//                    for (jm = jp[0], km = kp[0]; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                    //�Ƿ��ں����
//                    for (jm = jp[0] + 1, km = kp[0]; km <= kp[0]; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                    //�Ƿ����������
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
//            else//ɨ���λ�ߵ�λ����ĳ���缫��λ�����
//            {
//                v_flag = 0;
//                //�Ƿ���������
//                for (jm = jp[0]; jm <= m2; jm++)
//                {
//                    for (km = kp[0] - 1; km <= kp[0] + 2; km = km + 3)
//                    {
//                        if (direction_r(jm, km) == -1)	continue;
//                    }
//                }
//                //�Ƿ��ں����
//                for (jm = m2 + 1; jm <= m2 + 1; jm++)
//                {
//                    for (km = kp[0] - 1; km <= kp[0] + 1; km++)
//                    {
//                        if (direction_z(jm, km) == -1)	continue;
//                    }
//                }
//                //�Ƿ����������
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
//                line((coordinate_z(kp[0]) + coordinate_z(kp[0] + 1)) / 2 * asp, height - coordinate_r(m2)*asp, zp[1] * asp, height - rp[1] * asp);//���������缫��
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
//            //���ɨ���������
//            while (1)
//            {
//                v_flag = 0;
//                switch (point[0])
//                {
//
//                case 1:
//                    //�Ƿ���������
//                    for (jm = jp[0]; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ��ں����
//                    for (jm = jp[0]; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ����������
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
//                    //�Ƿ���������
//                    for (jm = jp[0] - 1; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0]; km <= kp[0] + 1; km++)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ��ں����
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm = jm + 2)
//                    {
//                        for (km = kp[0]; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ����������
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
//                    //�Ƿ���������
//                    for (jm = jp[0] - 1; jm <= jp[0]; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km = km + 2)
//                        {
//                            if (direction_r(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ��ں����
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm = jm + 2)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0]; km++)
//                        {
//                            if (direction_z(jm, km) == -1)	continue;
//                        }
//                    }
//                    //�Ƿ����������
//                    for (jm = jp[0] - 1; jm <= jp[0] + 1; jm++)
//                    {
//                        for (km = kp[0] - 1; km <= kp[0] + 1; km++)
//                        {
//                            //ֻ�������ܵĵ㣬���ĵ㲻����
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
//                    printf("/nswitch������д���/n");
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
//    closegraph();//�ر�ͼ�λ���
//
//    initgraph(width + 5, height + 5, SHOWCONSOLE);//��ʼ����ͼ����
//
// /******************************************�������ղ�ֵ�������ϵ�λ***********************************************/
//
// /* ���λ�����ֵ */
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
//    fprintf(out, "\n������ϵ�λֵ����λ�ã�\n\n");
//    for (num = 0; num <= ins; num++)
//        fprintf(out, "����λ��: %lf \t��λֵΪ: %lf\n", num*(width / asp) / ins, axis[num]);
//    fclose(out);
//    system("pause");
//    return 0;




}



/******************************************�ӳ���*****************************************************/
//��������
int iteration(double w)
{
    //����r1��Χ�ڵĵ�λֵ
    for (j = row - 1; j>m2; j--)
        for (k = 1; k<col - 1; k++)
        {
            if (j == row - 1)
                field[j][k] = (1 - w)*field[j][k] + w*(c1[j][k] * field[j][k - 1] + c2[j][k] * field[j][k + 1] + c4[j][k] * field[j - 1][k]) / c0[j][k];
            else
                field[j][k] = (1 - w)*field[j][k] + w*(c1[j][k] * field[j][k - 1] + c2[j][k] * field[j][k + 1] + c3[j][k] * field[j + 1][k] + c4[j][k] * field[j - 1][k]) / c0[j][k];
        }

    //����r2��Χ�ڵĵ�λֵ
    int l;
    for (i = 0, k = 0; i < n; i++)
    {
        k = k + step[i] + 1;//��i+1���缫�Ŀ���һ��(�����һ����缫�����б�ʾ)
        for (j = m2; j >0; j--)
            for (l = k - step[i]; l < k - 1; l++)//���缫�������(�������缫)
                field[j][l] = (1 - w)*field[j][l] + w*(c1[j][l] * field[j][l - 1] + c2[j][l] * field[j][l + 1] + c3[j][l] * field[j + 1][l] + c4[j][l] * field[j - 1][l]) / c0[j][l];
    }
    return 0;
}

//������ֵ
double coordinate_z(int kc)
{
    double z_k = 0;//z����ֵ
    int v_step = step[0];//��ʾ��i+1���缫����һ�����ڵ�����

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
    double r_j;//r����ֵ
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

        //�ж��Ƿ��Ǹ���ǰ�����ĵ��ظ�
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

        //�ж��Ƿ��Ǹ���ǰ�����ĵ��ظ�
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

        //�ж��Ƿ��Ǹ���ǰ�����ĵ��ظ�
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
            return k;//x������ĳ���������
    }
    return 0;//x���겻���������
}


