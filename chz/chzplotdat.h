#include "../matplotlibcpp.h"
namespace Chz
{
    constexpr double Control_T = 4e-3;

    constexpr int max_data_col = 600;
    constexpr int max_name_len = 40;
    constexpr int max_data_len = 20000;

    constexpr int max_command_col = 20;
    constexpr int max_command_len = 30;

    constexpr int max_plotted_data = 20;

    constexpr int max_defcommand_num = 50;
    constexpr int max_defcommand_len = 100;
    class LogData
    {
    public:
        void LoadFile(FILE*);
        int GetCommands(char* s);
        void LoadDefCommands();
    private:
        int DataCol, DataLen;
        char names[max_data_col][max_name_len];
        double data[max_data_col][max_data_len];
        
        int commandnum;
        char commands[max_command_col][max_command_len];

        int defcommand_num;
        char defcommand_name[max_defcommand_num][max_command_len];
        char defcommand_val[max_defcommand_num][max_defcommand_len];

        void anay_commands();
        int _command_defined();
        void _command_find();
        void _command_list();
        void _command_plot();

        int _chzstrcmp(char* a, char* b);
        int _ifstrisnum(char* s);
        void _lwstBT(char* s1, char* s2, int l1, int l2, int i, int j, double edist, double* res);
        std::vector<double> t_default;
        std::vector<double> plot_data;
    };
}