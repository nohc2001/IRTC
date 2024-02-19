#include <iostream>
#include "IRTC.h"
using namespace std;

FM_System0* fm;

constexpr char* filename = "episode.txt";
int main(){
    fm = new FM_System0();
    fm->SetHeapData(4096, 4096, 40960, 40960);
    IRTC_Table table;
    table.init();
    table.LoadData(filename);

    while (true)
    {
        cout << "choose behavier] \n1] add ChangeV\n2] seek value with time in ChangeV\n3] add value to ChangeV\n0] end program \n : " << endl;
        int input = 0;
        cin >> input;
        switch (input)
        {
        case 1:
        {
            ChangingValue* cv = (ChangingValue*)fm->_New(sizeof(ChangingValue), true);
            
            wchar_t NameOfChangeV[128] = {};
            cout << "Enter Name Of ChangeV : " << endl;
            wcin >> NameOfChangeV;

            cv->Init(NameOfChangeV, 0, false);
            
        }
        break;
        case 2:
        {
        }
        break;
        case 3:
        {
        }
        break;
        default:
        table.SaveData(filename);
        return 0;
        }
    }

    return 0;
}