#include <iostream>
#include "IRTC.h"
using namespace std;

FM_System0* fm;

//constexpr char filename[128] = "episode.txt";
//constexpr char filename[128] = "episode.txt";
//constexpr char filename[128] = "episode.txt";
//constexpr char filename[128] = "episode.txt";
constexpr char filename[128] = "episode.txt";

int main(){
    std::wcout.sync_with_stdio(false);
    std::wcout.imbue(std::locale("en_US.utf8"));
    std::wcin.sync_with_stdio(false);
    std::wcin.imbue(std::locale("en_US.utf8"));
    
    fm = new FM_System0();
    fm->SetHeapData(4096, 4096, 40960, 40960);
    IRTC_Table table;
    table.init();
    table.LoadData(filename);

    while (true)
    {
        cout << "\n--------------------------------------------------------\n" << endl;
        cout << "File : " << filename << endl;
        cout << "Listing ChangeV - ChangeV array size = " << table.table.size() << endl;
        for(int i=0;i<table.table.size();++i){
            ChangingValue* cv = table.table.at(i);
            int tn = (cv->use_2time_len) ? 2 : 1;
            wcout << L"|Name : " << cv->name.c_str() << L"\t|startTime : " << cv->startTime << L"\t|TurnToIntro : " << cv->onlyTI << L"\t|Mul : " << tn << L"\t|ValueCount : " << cv->staticInput.size() << endl;
        }
        cout << "choose behavier] \n1] add ChangeV\n2] seek value with time in ChangeV\n3] add value to ChangeV\n0] save and end program \n : " << endl;
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
            bool twotime = false;
            cout << "Enter Name Of ChangeV : " << endl;
            wcin >> NameOfChangeV;
            
            cout << "Enter Start Time Of ChangeV : " << endl;
            cin >> start_time;

            cout << "If you want active turn_to_intro option, input 1. : " << endl;
            cin >> turn_to_intro;

            cout << "If you want active twotime option, input 1. : " << endl;
            cin >> twotime;

            cv->Init(NameOfChangeV, start_time, turn_to_intro);
            cv->use_2time_len = twotime;
            table.table.push_back(cv);
        }
        break;
        case 2:
        {
            ChangingValue* choosed = nullptr;
            wchar_t NameOfChangeV[128] = {};
            int seektime = 0;
            cout << "Enter Name Of ChangeV : " << endl;
            wcin >> NameOfChangeV;

            for(int i=0;i<table.table.size();++i){
                ChangingValue* cv = table.table.at(i);
                if(wcscmp(cv->name.c_str(), NameOfChangeV) == 0){
                    choosed = cv;
                    break;
                }
            }

            if(choosed == nullptr){
                cout << "There is no name like that." << endl;
                break;
            }

            cout << "Enter Start Time Of ChangeV : " << endl;
            cin >> seektime;

            IRTCV irtcv = choosed->GetIRTC(seektime);
            cout << "level : " << irtcv.start_level << "\tphaze : ";
            coutIRTC(irtcv.cv[0]);
            cout << "[";
            coutIRTC(irtcv.cv[1]);
            cout << "]" << endl;
            wchar_t* temp = choosed->GetValue(seektime)->p.str.value;
            if(temp != nullptr){
                wcout << L"value : " << temp << endl;
            }
            else{
                cout << "value : (Blank)" << endl;
            }
        }
        break;
        case 3:
        {
            ChangingValue* choosed = nullptr;
            wchar_t NameOfChangeV[128] = {};
            wchar_t ValueWStr[256] = {};
            int seektime = 0;
            cout << "Enter Name Of ChangeV : " << endl;
            wcin >> NameOfChangeV;

            for(int i=0;i<table.table.size();++i){
                ChangingValue* cv = table.table.at(i);
                if(wcscmp(cv->name.c_str(), NameOfChangeV) == 0){
                    choosed = cv;
                    break;
                }
            }

            if(choosed == nullptr){
                cout << "There is no name like that." << endl;
                break;
            }

            cout << "Enter Start Time Of ChangeV : " << endl;
            cin >> seektime;

            cout << "Enter Value : " << endl;
            wcin >> ValueWStr;

            ValuePin* vp = (ValuePin*)fm->_New(sizeof(ValuePin), true);
            vp->pintype = 's';
            vp->p.str.time = seektime;
            vp->p.str.value = (wchar_t*)fm->_New(sizeof(wchar_t)*(wcslen(ValueWStr)+2), true);

            choosed->input(vp);
        }
        break;
        default:
        table.SaveData(filename);
        return 0;
        }
    }

    return 0;
}