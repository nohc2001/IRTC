#define H_IRTC
#include <iostream>
#include <fstream>
#include <math.h>
#include <locale>
#include <codecvt>
#include <math.h>

#ifndef H_UTILL_FREEMEMORY
#include "Utill_FreeMemory.h"
#endif
using namespace freemem;

std::string wstr_to_utf8(wchar_t * wstr)
{
	std::wstring_convert < std::codecvt_utf8 < wchar_t >> converter;
	std::string utf8Str = converter.to_bytes(wstr);
	return utf8Str;
}

std::wstring utf8_to_wstr(char * utf8str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wideStr = converter.from_bytes(utf8str);
    return wideStr;
}

enum class IRTC
{
	NONE = 0,
	I = 1,
	R = 2,
	T = 3,
	C = 4,
	TI = 5
};

struct pin_str
{
	int time;
	wchar_t *value;
};

struct pin_num
{
	int time;
	float value;
};

union pin
{
	pin_str str;
	pin_num num;
};

struct ValuePin
{
	pin p;
	char pintype;				// s - str, n - num
};

struct IRTCV
{
	IRTC cv[2] = { };
	int start_level = 0;
};

class ChangingValue
{
  public:
	vecarr < ValuePin * >staticInput;
	
	lwstr name;
	int startTime;
	bool use_TI_table = false;
	bool onlyTI = false;
	bool use_2time_len = false;

	  vecarr < bool > include_TI;
	  ChangingValue()
	{
	}
	virtual ~ ChangingValue()
	{
	}

	void Init(wchar_t *vname, int starttime, bool onlyti)
	{
		name.Init(32, false);
		name = vname;
		startTime = starttime;
		use_TI_table = false;
		onlyTI = onlyti;
		staticInput.NULLState();
		staticInput.Init(16, false);
	}

	void Set2Times(bool enable)
	{
		use_2time_len = enable;
	}

	bool getTI(int index)
	{
		if (use_TI_table)
		{
			if (include_TI.size() <= index)
			{
				return false;
			}
			else
			{
				return include_TI[index];
			}
		}
		else
		{
			return onlyTI;
		}
	}

	void input(ValuePin * vp)
	{
		staticInput.push_back(vp);
	}

	ValuePin *getvalue_with_time(int time)
	{
		for (int i = 0; i < staticInput.size(); ++i)
		{
			if (staticInput[i]->p.num.time == time)
			{
				return staticInput[i];
			}
		}
		return nullptr;
	}

	int get_rloc_with_level(int time, int level)
	{
		int po = pow(4, level);
		if (use_2time_len)
			po *= 2;
		return time - po * (time / po);
	}

	int get_r4loc_with_level(int time, int level)
	{
		int po = pow(4, level);
		if (use_2time_len)
			po *= 2;
		return (time / po) % 4;
	}

	IRTCV GetIRTC(int time)
	{
		IRTCV cv;
		cv.start_level = -1;
		if (time < startTime)
		{
			return cv;
		}

		int toplevel = 0;
		int at = startTime;
		int po = 4;
		int pastpo = 1;
		if (use_2time_len)
		{
			po *= 2;
			pastpo *= 2;
		}

		while (true)
		{
			if (get_r4loc_with_level(at, toplevel) == 0)
			{
				int nat = at - (at % po);
				if (nat <= time && time <= nat + po - 1)
				{
					int rloc = get_r4loc_with_level(time, toplevel);
					switch (rloc)
					{
					case 0:
						{
							if(toplevel == 0){
								cv.cv[0] = IRTC::I;
								cv.cv[1] = IRTC::I;
								cv.start_level = toplevel;
								return cv;
							}
							int rrloc = get_r4loc_with_level(time, toplevel - 1);
							switch (rrloc)
							{
							case 0:
								{
									float mul = 1;
									if(use_2time_len){
										mul = 2;
									}
									IRTCV pastcv = GetIRTC(time - mul*pow(4, cv.start_level));
									if (pastcv.cv[0] == IRTC::R)
									{
										cv.cv[0] = IRTC::TI;
									}
									else
									{
										cv.cv[0] = IRTC::I;
									}
								}
								break;
							case 1:
								cv.cv[0] = IRTC::R;
								break;
							case 2:
								cv.cv[0] = IRTC::T;
								break;
							case 3:
								cv.cv[0] = IRTC::C;
								break;
							}
						}
						cv.cv[1] = IRTC::I;
						cv.start_level = toplevel - 1;
						return cv;
					case 1:
						cv.cv[0] = IRTC::R;
						cv.cv[1] = IRTC::I;
						cv.start_level = toplevel;
						return cv;
					case 2:
						cv.cv[0] = IRTC::T;
						cv.cv[1] = IRTC::I;
						cv.start_level = toplevel;
						return cv;
					case 3:
						cv.cv[0] = IRTC::C;
						cv.cv[1] = IRTC::I;
						cv.start_level = toplevel;
						return cv;
					}
				}
				else
				{
					at = nat;
					pastpo = po;
					po *= 4;
					toplevel += 1;
				}
			}
			else
			{
				int nat = at - (at % po) + po;
				if (at <= time && time <= nat - 1)
				{
					// return irtc related of location;
					int rloc = (time - at) / pastpo;
					bool f4 = (time / pastpo) % 4 == 0;
					bool f2 = (time / pastpo) % 2 == 0 && f4 == false;

					cv.start_level = toplevel;
					if (rloc == 0)
					{
						cv.start_level -= 1;
						if (cv.start_level < 0)
						{
							cv.cv[0] = IRTC::I;
							cv.cv[1] = IRTC::NONE;
							cv.start_level = 0;
						}
						cv.cv[1] = IRTC::I;
						// add cv1
						int rrloc = (time - at) / (pastpo / 4);
						switch (rrloc)
						{
						case 0:
							{
								if (getTI(cv.start_level))
								{
									float mul = 1;
									if(use_2time_len){
										mul = 2;
									}
									IRTCV pastcv = GetIRTC(time - mul*pow(4, cv.start_level));
									if (pastcv.cv[1] == IRTC::I || pastcv.cv[0]== IRTC::TI)
									{
										cv.cv[0] = IRTC::R;
									}
									else if(pastcv.cv[0] == IRTC::NONE){
										cv.cv[0] = IRTC::I;
									}
									else
									{
										cv.cv[0] = IRTC::TI;
									}
								}
								else
								{
									cv.cv[0] = IRTC::R;
								}
								return cv;
							}
						case 1:
							cv.cv[0] = IRTC::R;
							return cv;
						case 2:
							cv.cv[0] = IRTC::T;
							return cv;
						case 3:
							cv.cv[0] = IRTC::C;
							return cv;
						}
					}
					else if (rloc > 1 && f2)
					{
						cv.cv[0] = IRTC::T;
						cv.cv[1] = IRTC::NONE;
						return cv;
					}
					else
					{
						cv.cv[0] = IRTC::R;
						cv.cv[1] = IRTC::NONE;
						return cv;
					}
				}
				else
				{
					at = nat;
					pastpo = po;
					po *= 4;
					toplevel += 1;
				}
			}
		}
	}

	ValuePin *GetValue(int time)
	{
		if(time < 0){
			return nullptr;
		}
		IRTCV chanV = GetIRTC(time);
		if (chanV.cv[0] != IRTC::R)
		{
			return getvalue_with_time(time);
		}
		else
		{
			ValuePin *pin = getvalue_with_time(time);
			if (pin == nullptr)
			{
				float aa = 1;
				if(use_2time_len){
					aa = 2;
				}
				return GetValue(time - aa*pow(4, chanV.start_level));
			}
			else
			{
				return pin;
			}
		}
	}
};

class IRTC_Table
{
  public:
	vecarr < ChangingValue * >table;

	IRTC_Table()
	{
	}
	virtual ~ IRTC_Table()
	{
	}

	void init()
	{
		table.NULLState();
		table.Init(8, false);
	}
	
	void SaveData(const char* filename){
		if(table.size()==0) return;
		FILE* fp = fopen(filename, "w");
		fprintf(fp, "%d\n", (int)table.size());
		for(int k=0;k<table.size();++k){
			ChangingValue* cv = table.at(k);
			fprintf(fp, "%s %d %d %d %d\n", wstr_to_utf8(cv->name.Arr).c_str(), cv->startTime, cv->onlyTI, cv->use_2time_len, (int)cv->staticInput.size());
			for(int i=0;i<cv->staticInput.size(); ++i){
				ValuePin* pin = cv->staticInput.at(i);
				fprintf(fp, "%d %s\n", pin->p.str.time, wstr_to_utf8(pin->p.str.value).c_str());
			}
		}
		fclose(fp);
	}
	
	void LoadData(const char* filename)
	{
		FILE* fp = fopen(filename, "r");
		int siz = 0;
		fscanf(fp, "%d", &siz);
		table.Init(siz + 8, false);
		wchar_t wstr[128] = {};
		char str[128] = {};
		for(int k=0;k<siz;++k){
			ChangingValue* cv = (ChangingValue*)fm->_New(sizeof(ChangingValue), true);
			int sisize = 0;
			fscanf(fp, "%s", str);
			fscanf(fp, "%d", &cv->startTime);
			fscanf(fp, "%d", &cv->onlyTI);
			fscanf(fp, "%d", &cv->use_2time_len);
			fscanf(fp, "%d", &sisize);
			wcscpy(wstr, utf8_to_wstr(str).c_str());
			cv->Init(wstr, cv->startTime, cv->onlyTI);
			for(int i=0;i<sisize; ++i){
				ValuePin* pin = (ValuePin*)fm->_New(sizeof(ValuePin), true);
				fscanf(fp, "%d", &pin->p.str.time);
				fscanf(fp, "%s", str);
				wcscpy(wstr, utf8_to_wstr(str).c_str());
				pin->p.str.value = (wchar_t*)fm->_New(sizeof(wchar_t)*wcslen(wstr)+8, true);
				wcscpy(pin->p.str.value, wstr);
				cv->staticInput.push_back(pin);
			}
			table.push_back(cv);
		}
		fclose(fp);
	}
};