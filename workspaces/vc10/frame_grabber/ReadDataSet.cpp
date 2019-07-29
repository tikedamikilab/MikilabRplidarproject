#include "stdafx.h"
#include "ReadDataSet.h"
#include "scanView.h"
/*
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
*/
ReadDataSet::ReadDataSet()
{
}
ReadDataSet::~ReadDataSet()
{
}
/*
vector<string> OnSplit(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

void ReadDataSet::setReadSetBackData(ifstream &ifs, CScanView &m_scanview) {
	
	string line;
	while (getline(ifs, line)) {
		std::vector<string> strvec = OnSplit(line, ',');
		scanDot_Simple tmp_scan;
		for (int i = 0; i<strvec.size(); i++) {
			OnReadSetBackData(i, stoi(strvec.at(i)), tmp_scan);
		}
		m_scanview.setReadBackData(tmp_scan);
	}
}
void ReadDataSet::OnReadSetBackData(int determinant, int data, scanDot_Simple &tmp_scan) {
	switch (determinant)
	{
	case 0: tmp_scan.X = data;        break;
	case 1: tmp_scan.Y = data;        break;
	default: break;
	}
}
*/
