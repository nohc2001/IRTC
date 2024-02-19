#include <iostream>
#include "IRTC.h"
using namespace std;

FM_System0* fm;

constexpr char filename[128] = "episode.txt";
int main(){
    fm = new FM_System0();
    fm->SetHeapData(4096, 4096, 40960, 40960);
    IRTC_Table table;
    table.init();
    table.LoadData(filename);

    while (true)
    {
        cout << "Listing ChangeV - ChangeV array size = " << table.table.size() << endl;
        for(int i=0;i<table.table.size();++i){
            ChangingValue* cv = table.table.at(i);
            wcout << L"Name : " << cv->name.c_str() << L"\tstartTime : " << cv->startTime << L"\tOnlyTurnToIntro Mod : " << cv->onlyTI << endl;
        }
        cout << "choose behavier] \n1] add ChangeV\n2] seek value with time in ChangeV\n3] add value to ChangeV\n0] end program \n : " << endl;
        int input = 0;
        cin >> input;
        switch (input)
        {
        case 1:
        {
            ChangingValue* cv = (ChangingValue*)fm->_New(sizeof(ChangingValue), true);
            
            wchar_t NameOfChangeV[128] = {};
            int start_time = 0;
            bool turn_to_intro = false;
            cout << "Enter Name Of ChangeV : " << endl;
            wcin >> NameOfChangeV;

            cout << "Enter Start Time Of ChangeV : " << endl;
            cin >> start_time;

            cout << "If you want active turn_to_intro option, input 1. : " << endl;
            cin >> turn_to_intro;

            cv->Init(NameOfChangeV, start_time, turn_to_intro);
            table.table.push_back(cv);
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