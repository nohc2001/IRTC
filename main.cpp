#include <iostream>
#include "IRTC.h"
using namespace std;

FM_System0* fm;

int main(){
    fm = new FM_System0();
    fm->SetHeapData(4096, 4096, 40960, 40960);
    IRTC_Table table;
    table.init();
    table.LoadData("episode.txt");
    return 0;
}