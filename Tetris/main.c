#include<stdio.h>
#include<stdlib.h> //包含system()
#include<time.h>   //包含以时间为依据的随机数
#include<windows.h>//包含隐藏光标
#include<conio.h>

#define SPACE 32    //ASCII码
#define LEFT  75
#define RIGHT 77
#define DOWN 80
#define ESC 27
#define Wall 2      //墙是2
#define Box 1       //方块是1
#define Kong 0      //空是0
#define FACE_X 30
#define FACE_Y 20   //界面尺寸
////////////////声明函数部分/////////////////////
void gotoxy(int x,int y);//移动光标
int  color(int c);       //颜色
void hidden_cursor();    //隐藏光标
void inter_face();       //初始化界面
void init_dia();         //初始化方块信息
void draw_dia(int base,int space_c,int x,int y);  //覆盖方块
void draw_kong(int base,int space_c,int x,int y); //画方块
int  pd(int n,int space_c,int x,int y);  //判断是否到底
void start_game();  //开始游戏
int  xc();          //消除
void read_file();   //读写最高记录
void write_file();  //写最高纪录

int grade=0;    //当前分数
int max=0;      //最高记录
int nn=0;       //用于产生0-6的随机数

struct Face
{
    int data[FACE_X][FACE_Y+10];    //数值，为1是方块，为0是空格
    int color[FACE_X][FACE_Y+10];   //对应方块的颜色
} face;

typedef struct Diamonds
{
    int space[4][4];        //4*4矩阵，为1为方块，为0 为空
} Dia;

Dia dia[7][4];  //一维基础7个方块，二维表示旋转次数

int main()
{
    system("cls");//清屏
    system("title 俄罗斯方块");
    color(15);  //15号为白色
    system("mode con cols=60 lines=30");
    //调整系统控制台显示的宽度和高度，高度为30个字符，宽度为60个字符
    hidden_cursor();//隐藏光标
    srand(time(NULL));//以现在的系统时间作为随机数的种子来产生随机数
    read_file();
    grade=0;//分数记为0
    inter_face();//构造界面
    init_dia();//构造不同种方块
    nn=rand()%7;//产生0-6的随机数
    printf("%d",nn);
    while(1)   //一直运行除非跳出
    {
        start_game();
    }
    return 0;
}
///////////////////开始游戏//////////////////////////////
void start_game()
{
    int n,ch,t=0,x=0,y=FACE_Y/2-2,i,j;
    int space_c=0;  //旋转次数
    draw_kong(nn,space_c,4,FACE_Y+3);
    n=nn;
    nn=rand()%7;    //随机生成下一块
    color(nn);//颜色随机，nn随机数确定
    draw_dia(nn,space_c,4,FACE_Y+3);
    while(1)
    {
        color(n);
        draw_dia(n,space_c,x,y);//画方块
        if(t==0)
            t=8000;//规定方块下落的速度
        while(--t)
        {
            if(kbhit()!=0)//有输入就跳出
                break;
        }
        if(t==0)
        {//方块落下时
            if(pd(n,space_c,x+1,y)==1)
            {//如果没到底
                draw_kong(n,space_c,x,y);
                x++;   //向下降落
            }
            else
            {//如果到底
             //描绘方块：包含颜色信息、方向信息、位置信息
                for(i=0; i<4; i++)
                {
                    for(j=0; j<4; j++)
                    {
                        if(dia[n][space_c].space[i][j]==1)
                        {
                            face.data[x+i][y+j]=Box;
                            face.color[x+i][y+j]=n;
                            while(xc());//清除
                        }//
                    }
                }
                return;
            }
        }
        else
        {//方块在落下的过程中
            ch=getch();
        /*会等待你按下任意键,再继续执行下面的语句;用ch=getch();
        会等待你按下任意键之后,把该键字符所对应的ASCII码赋给ch,
        再执行下面的语句。*/
            switch(ch)     //移动
            {
            case LEFT://判断是否可以向左移动
                if(pd(n,space_c,x,y-1)==1)
                {
                    draw_kong(n,space_c,x,y);
                    y--;
                }
                break;
            case RIGHT://判断是否可以向右移动
                if(pd(n,space_c,x,y+1)==1)
                {
                    draw_kong(n,space_c,x,y);
                    y++;
                }
                break;
            case DOWN://判断是否可以向下移动
                if(pd(n,space_c,x+1,y)==1)
                {
                    draw_kong(n,space_c,x,y);
                    x++;
                }
                break;
            case SPACE://判断方向
                if(pd(n,(space_c+1)%4,x+1,y)==1)
                {
                    draw_kong(n,space_c,x,y);
                    space_c=(space_c+1)%4;//按下的次数定义为四种方向
                }
                break;
            case ESC ://显示退出界面
                system("cls");
                gotoxy(FACE_X/2,FACE_Y);
                printf("---游戏结束!---\n\n");
                gotoxy(FACE_X/2+2,FACE_Y);
                printf("---按任意键退出!---\n");
                getch();
                exit(0);
                break;
            case  'R':
            case  'r':
                main();//重新运行
                exit(0);
            case  'S':
            case  's':
                while(1)
                {//暂停状态一直在循环内，按任意键继续游戏
                    if(kbhit()!=0)//有输入就跳出
                        break;
                }
                break;
            }
        }
    }
}
//////////////清除/////////////
int xc()
{
    int i,j,k,sum;
    for(i=FACE_X-2; i>4; i--)
    {
        sum=0;
        for(j=1; j<FACE_Y-1; j++)
        {
            sum+=face.data[i][j];
        }
        if(sum==0)
            break;
        if(sum==FACE_Y-2)   //满一行，减掉
        {
            grade+=100;
            color(7);
            gotoxy(FACE_X-4,2*FACE_Y+2);
            printf("分数：%d",grade);
            for(j=1; j<FACE_Y-1; j++)
            {
                face.data[i][j]=Kong;
                gotoxy(i,2*j);
                printf(" ");
            }
            for(j=i; j>1; j--)
            {
                sum=0;
                for(k=1; k<FACE_Y-1; k++)
                {
                    sum+=face.data[j-1][k]+face.data[j][k];
                    face.data[j][k]=face.data[j-1][k];
                    if(face.data[j][k]==Kong)
                    {
                        gotoxy(j,2*k);
                        printf(" ");
                    }
                    else
                    {
                        gotoxy(j,2*k);
                        color(face.color[j][k]);
                        printf("■");//消除之后剩余的方块
                    }
                }
                if(sum==0)
                    return 1;
            }

        }
    }
    for(i=1; i<FACE_Y-1; i++)
    {
        if(face.data[1][i]==Box)
        {
            char n;
            Sleep(2000); //延时判断到顶
            system("cls");
            color(7);
            gotoxy(FACE_X/2-2,2*(FACE_Y/3));//移动光标
            if(grade>max)
            {
                printf("恭喜您打破记录，目前最高纪录为：%d",grade);
                write_file();
            }
            else if(grade==max)
                printf("与纪录持平，请突破你的极限！");
            else
                printf("请继续努力，你与最高记录只差：%d",max-grade);
            gotoxy(FACE_X/2,2*(FACE_Y/3));
            printf("GAME OVER!\n");
            do
            {
                gotoxy(FACE_X/2+2,2*(FACE_Y/3));
                printf("是否重新开始游戏(y/n): ");
                scanf("%c",&n);
                gotoxy(FACE_X/2+4,2*(FACE_Y/3));
                if(n!='n' && n!='N' && n!='y' && n!='Y')
                    printf("输入错误，请重新输入!");
                else
                    break;
            }
            while(1);
            if(n=='n' || n=='N')
            {
                gotoxy(FACE_X/2+4,2*(FACE_Y/3));
                printf("按任意键退出游戏！");
                exit(0);
            }
            else if(n=='y' || n=='Y')
                main();
        }
    }
    return 0;
}
//////////////////读取最高记录///////////////////////
void read_file()
{
    FILE *fp;
    fp=fopen("俄罗斯方块记录.txt","r+");
    if(fp==NULL)
    {
        fp=fopen("俄罗斯方块记录.txt","w+");
        fwrite(&max,sizeof(int),1,fp);
    }
    fseek(fp,0,0);
    fread(&max,sizeof(int),1,fp);
    fclose(fp);
}
////////////////保存最高记录////////////////////////
void write_file()
{
    FILE *fp;
    fp=fopen("俄罗斯方块记录.txt","r+");
    fwrite(&grade,sizeof(int),1,fp);
    fclose(fp);
}
////////////////判断是否到底/////////////////////
int pd(int n,int space_c,int x,int y) //检验是否可以继续向下移动
{
    int i,j;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            if(dia[n][space_c].space[i][j]==0)
                continue;//没到底继续
            else if(face.data[x+i][y+j]==Wall || face.data[x+i][y+j]==Box)
                return 0;//到底（遇到墙或方块）返回0
        }
    }
    return 1;
}

void draw_kong(int base,int space_c,int x,int y)
{//补充空白
    int i,j;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            gotoxy(x+i,2*(y+j));
            if(dia[base][space_c].space[i][j]==1)
                printf("  ");
        }
    }

}

void draw_dia(int base,int space_c,int x,int y)
{//覆盖方块
    int i,j;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            gotoxy(x+i,2*(y+j));
            if(dia[base][space_c].space[i][j]==1)
                printf("■");
        }
    }
}

void init_dia()
{//初始化方块信息
    int i,j,k,z;
    int tmp[4][4];
    for(i=0; i<3; i++)
        dia[0][0].space[1][i]=1;
        dia[0][0].space[2][1]=1;     //土形

    for(i=1; i<4; i++)
        dia[1][0].space[i][1]=1;
        dia[1][0].space[1][2]=1;    //L形--1

    for(i=1; i<4; i++)
        dia[2][0].space[i][2]=1;
        dia[2][0].space[1][1]=1;    //L形--2

    for(i=0; i<2; i++)
    {
        dia[3][0].space[1][i]=1;
        dia[3][0].space[2][i+1]=1; //Z形--1

        dia[4][0].space[1][i+1]=1;
        dia[4][0].space[2][i]=1;//Z形--2

        dia[5][0].space[1][i+1]=1;
        dia[5][0].space[2][i+1]=1;//田字形
    }
    for(i=0; i<4; i++)
        dia[6][0].space[i][2]=1;//1形
    /////////////////////基础7个形状///////////////////
    for(i=0; i<7; i++)
    {
        for(z=0; z<3; z++)
        {
            for(j=0; j<4; j++)
            {
                for(k=0; k<4; k++)
                {
                    tmp[j][k]=dia[i][z].space[j][k];
                }
            }
            for(j=0; j<4; j++)
            {
                for(k=0; k<4; k++)
                {
                    dia[i][z+1].space[j][k]=tmp[4-k-1][j];
                }
            }
        }
    }
    ///////////////旋转后的21个形状////////////////
}
//////////////////构造游戏界面//////////////////////
void inter_face()//界面
{
    int i,j;
    for(i=0; i<FACE_X; i++)//从上到下每一行扫描
    {
        for(j=0; j<FACE_Y+10; j++)//从左到右每一列扫描
        {
            if(j==0 || j==FACE_Y-1 || j==FACE_Y+9)
            {//在第0列、第19列、第29列处，设为墙
                face.data[i][j]=Wall;
                gotoxy(i,2*j);
                printf("■");
            }
            else if(i==FACE_X-1)
            {//在第29行处，设为墙
                face.data[i][j]=Wall;//Box;
                gotoxy(i,2*j);
                printf("■");
            }
            else//其余设为空
                face.data[i][j]=Kong;
        }
    }
    gotoxy(FACE_X-18,2*FACE_Y+2);//（12，42）
    printf("左移：←");

    gotoxy(FACE_X-16,2*FACE_Y+2);//（14，42）
    printf("右移：→");

    gotoxy(FACE_X-14,2*FACE_Y+2);//（16，42）
    printf("旋转：space");

    gotoxy(FACE_X-12,2*FACE_Y+2);//（18，42）
    printf("暂停: S");

    gotoxy(FACE_X-10,2*FACE_Y+2);//（20，42）
    printf("退出: ESC");

    gotoxy(FACE_X-8,2*FACE_Y+2);//（22，42）
    printf("重新开始:R");

    gotoxy(FACE_X-6,2*FACE_Y+2);//（24，42）
    printf("最高纪录:%d",max);

    gotoxy(FACE_X-4,2*FACE_Y+2);//（26，42）
    printf("分数：%d",grade);

}
///////////////////移动坐标///////////////////////////////
void gotoxy(int x,int y) //移动坐标，实现光标控制所在位置
{
    COORD coord;    //coord为结构体变量成员包含x,y
    coord.X=y;
    coord.Y=x;
    SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), coord );
    //这个函数使光标 （x,y）位置函数.在#include<windows.h>内
    //SetConsoleCursorPosition是API中定位光标位置的函数
    //如果用户定义了 COORD coord，那么coord其实是一个结构体变量，
    //其中X和Y是它的成员，通过修改coord.X和coord.Y的值就可以实现光标的位置控制。
/*定位光标位置的函数，坐标为GetStdHandle()返回标准的输出的句柄，
也就是获得输出屏幕缓冲区的句柄，并赋值给对象c*/
}
///////////////////隐藏光标/////////////////////////////////
void hidden_cursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    //从一个特定的标准设备(标准输入、标准输出或标准错误)中取得一个句柄(用来标识不同设备的数值)。
    //句柄，是整个Windows编程的基础。
    //一个句柄是指使用的一个唯一的整数值，即一个4字节(64位程序中为8字节)长的数值，
    //来标识应用程序中的不同对象和同类对象中的不同的实例，
    //诸如，一个窗口，按钮，图标，滚动条，输出设备，控件或者文件等。
    CONSOLE_CURSOR_INFO cci;//光标信息
    GetConsoleCursorInfo(hOut,&cci);//
    cci.bVisible=0;//赋1为显示，赋0为隐藏，如果没赋值的话，隐藏光标无效
    SetConsoleCursorInfo(hOut,&cci);
}
int color(int c)
{//传入的C是一个随机数
    switch(c)
    {
    case 0:     //┬形
        c=9;    //淡兰
        break;
    case 1:     //┌形
        c=11;   //淡青
        break;
    case 2:     //┐形
        c=12;   //淡红
        break;
    case 3:     //Z形--1
        c=6;    //棕
        break;
    case 4:     //Z形--2(反)
        c=14;   //黄
        break;
    case 5:     //田字形
        c=10;   //淡绿
        break;
    case 6:     //1形
        c=13;   //淡洋红
        break;
    default:
        c=15;   //白
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);//更改文字颜色
    //GetStdHandle获取标准输出设备句柄;两个参数分别是指定哪个窗体，具体位置
    //SetConsoleTextAttribute是一个可以在API（应用程序编程接口）中
	//设置控制台窗口字体颜色和背景色的计算机函数
    return 0;
}
