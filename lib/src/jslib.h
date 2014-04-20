#ifndef GAME_LIB_H_
#define GAME_LIB_H

#include <string>
#include <vector>

using namespace std;

namespace jslib {

    void init_debug(string s);
    void deinit_debug();
    void debug(string s);
    void debug(string s,int lvl);
    void dappend(string s, int lvl);
    void dappend(string s);

    float load_float(std::string str);
    double load_double(std::string str);
    bool load_bool(std::string str);
    int load_int(std::string str);
    std::string load_string(std::string str);

    int get_debug_lvl_main();
    int set_debug_lvl_main(int lvl);
    int get_debug_lvl();
    int set_debug_lvl(int lvl);
    int reset_debug_lvl();

    typedef vector<string *> StringArray;
    typedef StringArray::iterator SAi;

    bool to_bool(string s);
    string to_str(int i);
    string to_str(float f);
    string to_str(double f);
    string to_str(bool b);
    string to_str(string s);
    int mstime();
    StringArray * explode(string sep, string s);
    string implode(string sep, StringArray * s);
    string get_line(FILE * f);
}
#endif
