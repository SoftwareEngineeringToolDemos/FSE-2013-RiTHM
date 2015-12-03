#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main (int argc, char* argv[]) {
	string line;
	string fileName = argv[1];
	fileName.append("/alias.txt");
	ifstream myfile (fileName.c_str());
	
	string outfileName =  argv[2];
	outfileName.append("/Alias/alias.txt");
	
	ofstream outfile;
	outfile.open(outfileName.c_str(), ios::app);

	if (myfile.is_open() && outfile.is_open())
	{
		while ( myfile.good() )
		{
			getline (myfile,line);
			if (line.find("AliasSet") != string::npos || line.find("Tracker") != string::npos) {
				outfile << line << endl;

			}
		}
		myfile.close();
		outfile.close();
	} 
    else {
        cout << "Unable to open file" << endl; 
        return 1;
    }
	
	return 0;
}
