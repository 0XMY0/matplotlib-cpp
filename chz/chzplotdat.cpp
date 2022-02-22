#include "chzplotdat.h"
#include <fstream>
#include <cstring>
namespace plt = matplotlibcpp;

char strtemp[400];
namespace Chz
{
    void LogData::LoadFile(FILE* f)
    {
        DataCol = DataLen = commandnum = 0;
        char namenow[max_name_len];
        // load names
        while(1)
        {
            fscanf(f, "%s", namenow);
            strcat(names[DataCol++], namenow);
            char c; fscanf(f, "%c", &c);
            if(c == '\n') break;
        }
        while(1)
        {
            double temp;
            if(fscanf(f, "%lf", &temp) == EOF) break;
            else
            {
                data[0][DataLen] = temp;
                for(int i = 1; i != DataCol; i++) fscanf(f, "%lf", &data[i][DataLen]);
                DataLen++;
            }
        }
        fclose(f);
        printf("Successful loaded %d Rows x %d Columns\n", DataLen, DataCol);
        t_default.clear();
        for(int i = 0; i != DataLen; i++) t_default.push_back(Control_T * i);
    }
    
    int LogData::GetCommands(char* s)
    {
        commandnum = 0;
        int templen = 0;
        while(1)
        {
            if(*s == '\0') break;
            while(1)
            {
                commands[commandnum][templen] = *s;
                templen++; s++;
                if(*s == ' ' || *s == '\0') break;
            }
            commands[commandnum][templen] = '\0';
            commandnum++; templen = 0;
            if(*s == ' ') s++; 
        }
        if(commandnum != 0 && _chzstrcmp(commands[0], "exit") == 0) return 1;
        anay_commands();
        return 0;
    }

    void LogData::LoadDefCommands()
    {
        FILE* f;
        fopen_s(&f, "_DefCommands.txt", "r");
        defcommand_num = 0;
        while(1)
        {
            if(fgets(defcommand_name[defcommand_num], max_command_len, f) == nullptr) break;
            fgets(defcommand_val[defcommand_num], max_defcommand_len, f);
            int i = strlen(defcommand_name[defcommand_num]);
            if(defcommand_name[defcommand_num][i - 1] == '\n') defcommand_name[defcommand_num][i - 1] = '\0';
            i = strlen(defcommand_val[defcommand_num]);
            if(defcommand_val[defcommand_num][i - 1] == '\n') defcommand_val[defcommand_num][i - 1] = '\0';
            defcommand_num++;
        }
        fclose(f);
    }

    void LogData::anay_commands()
    {
        if(commandnum == 0) return;
        if(_command_defined()) return;
        if(_chzstrcmp(commands[0], "find") == 0) _command_find();
        else if(_chzstrcmp(commands[0], "list") == 0) _command_list();
        else if(_chzstrcmp(commands[0], "plot") == 0) _command_plot();
        else printf("command \"%s\" not found!\n", commands[0]);
    }

    int LogData::_command_defined()
    {
        for(int i = 0; i != defcommand_num; i++)
            if(_chzstrcmp(commands[0], defcommand_name[i]) == 0) { GetCommands(defcommand_val[i]); return 1; }
        return 0;
    }

    void LogData::_command_find()
    {
        double dis[max_data_col];
        for(int i = 1; i != commandnum; i++)
        {
            int l1 = strlen(commands[i]), l2;
            for(int j = 0; j != DataCol; j++) 
            {
                l2 = strlen(names[j]);
                dis[j] = 100000;
                _lwstBT(commands[i], names[j], l1, l2, 0, 0, 0.0, &dis[j]);
            }

            constexpr int find_num = 5;
            int minpos[find_num];
            for(int j = 0; j != find_num; j++) minpos[j] = j;
            for(int k = find_num; k != DataCol; k++)
            {
                int maxpos = 0;
                for(int j = 1; j != find_num; j++) if(dis[minpos[j]] > dis[minpos[maxpos]]) maxpos = j;
                if(dis[k] < dis[minpos[maxpos]]) minpos[maxpos] = k;
            }
            for(int i = 0; i != find_num - 1; i++)
                for(int j = i + 1; j != find_num; j++)
                    if(dis[minpos[i]] > dis[minpos[j]])
                    {
                        int temp = minpos[i]; minpos[i] = minpos[j]; minpos[j] = temp;
                    }
            printf("Most similar results for \"%s\":\n", commands[i]);
            for(int j = 0; j != find_num; j++) printf("  No %d: col = %d, name = \"%s\";\n", j + 1, minpos[j], names[minpos[j]]);
            printf("\n");
        }
    }

    void LogData::_command_list()
    {
        for(int i = 1; i != commandnum; i++)
        {
            if(!_ifstrisnum(commands[i]))
            {
                printf("Command \"%s\" is not a number!\n");
                continue;
            }
            int numcol = -1;
            for(int j = 0; j != DataCol; j++) 
                if(strcmp(names[j], commands[i]) == 0) 
                {
                    numcol = j;
                    break;
                }
            if(numcol == -1)
            {
                printf("Number \"%s\" not found!\n", commands[i]);
                continue;
            }

            printf("Columns after \"%s\":\n", commands[i]);
            for(int j = numcol + 1; j != DataCol; j++)
            {
                if(_ifstrisnum(names[j])) break;
                printf("  No %d: \"%s\";\n", j - numcol, names[j]);
            }
        }
    }

    void LogData::_command_plot()
    {
        int ifplot = 0;
        for(int i = 1; i != commandnum; i++) 
        {
            int opt1 = -1, opt2 = -1, optnum = 0;
            if(i != commandnum - 1 && commands[i + 1][0] == '-') opt1 = int(commands[i + 1][1]) - int('0'), optnum++;
            if(i < commandnum - 2 && commands[i + 2][0] == '-') opt2 = int(commands[i + 2][1]) - int('0'), optnum++;
            if(opt1 != -1 && opt2 == -1) opt2 = opt1;
            if(opt1 == -1 && opt2 != -1) opt1 = opt2;

            int findnum = 0, plotnum = 0;
            for(int col = 0; col != DataCol; col++)
            {
                int ifmatch = (_chzstrcmp(commands[i], names[col]) == 0);
                if(ifmatch) findnum++;
                if(ifmatch && ((findnum >= opt1 && findnum <= opt2) || optnum == 0))
                {
                    if(!ifplot) plt::figure_size(600, 400);
                    ifplot = 1;
                    plotnum++;
                    plot_data = std::vector<double>(&data[col][0], &data[col][DataLen]);
                    std::string plotname(names[col]);
                    plotname.push_back(char(plotnum + '0'));
                    plt::named_plot(plotname, t_default, plot_data);
                }
            }
            if(findnum == 0) printf("\"%s\" not found!\n", commands[i]);
            if(findnum && plotnum == 0) 
            {
                if(optnum == 1) printf("\"%s\" %d not found!\n", commands[i], opt1, opt2);
                else if(optnum == 2) printf("\"%s\" %d ~ %d not found!\n", commands[i], opt1, opt2);
            }
            i += optnum;
        }
        if(ifplot) plt::legend(), plt::show();
    }
    
    int LogData::_chzstrcmp(char* a, char* b)
    {
        int i = 0;
        while(1)
        {
            if(a[i] != b[i] && a[i] != b[i] + 32 && a[i] != b[i] - 32) return 1;
            if(a[i] == b[i] && a[i] == '\0') return 0;
            i++;
        }
    }
    
    int LogData::_ifstrisnum(char* s)
    {
        if(strlen(s) == 0) return 0;
        int i = 0;
        while(1)
        {
            if(s[i] == '\0') return 1;
            if(s[i] < 48 || s[i] > 57) return 0;
            i++;
        }
    }
    
    void LogData::_lwstBT(char* s1, char* s2, int l1, int l2, int i, int j, double edist, double* res)
    {
        if (i == l1 || j == l2) 
        {
            if (i < l1)  edist += 1.0 * (l1 - i);    // a长，直接加上后续多余长度
            if (j < l2)  edist += 1.0 * (l2 - j);    // 同上
            if (edist < *res) *res = edist;  // 更新最大值
            return;
        }
        if (s1[i] == s2[j] || s1[i] == s2[j] + 32 || s1[i] == s2[j] - 32)           // 两个字符匹配
            _lwstBT(s1, s2, l1, l2, i + 1, j + 1, edist, res);
        else 
        {                               // 两个字符不匹配
            _lwstBT(s1, s2, l1, l2, i + 1, j, edist + 1.0, res);   // 删除a[i]或者b[j]前添加一个字符
            _lwstBT(s1, s2, l1, l2, i, j + 1, edist + 1.0, res);   // 删除b[j]或者a[i]前添加一个字符
            _lwstBT(s1, s2, l1, l2, i + 1, j + 1, edist + 1.5, res); // 将a[i]和b[j]替换为相同字符
        }
    }
    LogData LogData1;
}

int main(int argc, char** argv)
{
    FILE* f;
    Chz::LogData1.LoadDefCommands();
    if(argc > 1) strcpy(strtemp, argv[1]);
    else
    {
        printf("Input file name: \n");
        gets_s(strtemp);
    }
    fopen_s(&f, strtemp, "r");
    if(f == nullptr)
    {
        printf("Can not open \"%s\"!\n", strtemp);
        fclose(f);
        gets_s(strtemp);
        return 0;
    }
    Chz::LogData1.LoadFile(f);
    while(1)
    {
        gets_s(strtemp);
        if (Chz::LogData1.GetCommands(strtemp)) break;
    }
}