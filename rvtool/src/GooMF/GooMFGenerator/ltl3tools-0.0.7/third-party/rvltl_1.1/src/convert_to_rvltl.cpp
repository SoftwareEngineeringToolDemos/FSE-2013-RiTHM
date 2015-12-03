//============================================================================
// Name        : convert_to_rvltl.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

using namespace std;

char* dphi_name = NULL;
char* crossprod_name = NULL;
char* out_name = NULL;

bool find_pair(vector< pair<int, int> > v, pair<int, int> p)
{
	for (unsigned int i=0; i<v.size(); ++i)
	{
		if (v[i].first == p.first && v[i].second == p.second)
			return true;
	}

	return false;
}

bool find_int(vector<int> v, int p)
{
	for (unsigned int i=0; i<v.size(); ++i)
	{
		if (v[i] == p)
			return true;
	}

	return false;
}

bool parse_params(int argc, char** argv)
{
	//parameters parsing
	char* temp;
	int i = 0;
	for(i=1; i<argc; ++i)
	{
		if (strstr(argv[i], "dphi=") != NULL)
		{
			temp = strpbrk(argv[i], "=");
			dphi_name = temp+1;
			continue;
		}

		if (strstr(argv[i], "crossprod=") != NULL)
		{
			temp = strpbrk(argv[i], "=");
			crossprod_name = temp+1;
			continue;
		}

		if (strstr(argv[i], "out=") != NULL)
		{
			temp = strpbrk(argv[i], "=");
			out_name = temp+1;
			continue;
		}
	}

	if (dphi_name == NULL || crossprod_name == NULL || out_name == NULL)
		return false;

	return true;
}

int main(int argc, char** argv) {

	ifstream buchi_phi_fsm;
	ifstream crossprod_fsm;
	ofstream final_fsm;

	if (!parse_params(argc, argv))
		return -1;

	vector<int> buchi_phi_acceptance_states;
	vector< pair<int, int> > yellow_states;

	buchi_phi_fsm.open(dphi_name);
	if(!buchi_phi_fsm) {
		cerr << "Error opening file\n";
		return -1;
	}

	crossprod_fsm.open(crossprod_name);
	if(!crossprod_fsm) {
		cerr << "Error opening file\n";
		return -1;
	}

	final_fsm.open(out_name);
	if(!final_fsm) {
		cerr << "Error opening file\n";
		return -1;
	}

	string input_line;

	//parsing all conclusive states in DPhi.dot
	getline(buchi_phi_fsm, input_line);
	while (!buchi_phi_fsm.eof())
	{
		if(input_line.find("doublecircle") != string::npos)
		{
			string temp_string = input_line.substr(0, input_line.find_first_of(' '));
			int temp_int = atoi(temp_string.c_str());
			buchi_phi_acceptance_states.push_back(temp_int);
		}

		getline(buchi_phi_fsm, input_line);
	}

	getline(crossprod_fsm, input_line);	//first line is spare
	final_fsm << input_line << '\n';
	getline(crossprod_fsm, input_line);
	while (!crossprod_fsm.eof())
	{
		if(input_line.find("color=yellow") != string::npos)
		{
			string temp_string = input_line.substr(2, input_line.find_first_of(','));
			int temp_int1 = atoi(temp_string.c_str());
			temp_string = input_line.substr(input_line.find_first_of(',')+2, input_line.find_first_of(')'));
			int temp_int2 = atoi(temp_string.c_str());
			pair<int, int> p;
			p.first = temp_int1;
			p.second = temp_int2;

			bool first_in_dphi = find_int(buchi_phi_acceptance_states, p.first);

			if ((p.first >= 0) && (p.second >= 0) && first_in_dphi)
			{
				string new_line = input_line.substr(0, input_line.find("yellow"));
				new_line.append("darkseagreen1]\n");
				final_fsm << new_line;
			}
			else if ((p.first >= 0) && (p.second >= 0) && !first_in_dphi)
			{
				string new_line = input_line.substr(0, input_line.find("yellow"));
				new_line.append("pink]\n");
				final_fsm << new_line;
			}
			else
			{
				final_fsm << input_line << '\n';
			}
		}
		else
		{
			final_fsm << input_line << '\n';
		}

		getline(crossprod_fsm, input_line);
	}

	buchi_phi_fsm.close();
	crossprod_fsm.close();
	final_fsm.close();
}
