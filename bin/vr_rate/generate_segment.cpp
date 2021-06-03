#include <iostream>
#include <stdlib.h>
#include <vector> 
#include <cstring>
using namespace std;
int sd = 2000;
int main(){
	string creat_folder = "mkdir " + to_string (sd) + "ms"; 
	system (creat_folder.c_str());
	system (cd.c_str());
	vector<int> rate={17926126, 10583300, 10085911, 4410272, 3152399, 2381377, 1771217, 1195658, 877178, 653467, 470375, 251123, 257987, 113195, 109712};
	for(int i=0;i<rate.size();i++){
		string cd = "cd ./" + to_string(sd) + "ms";
	
		int rate_in_byte = (sd / 1000) * rate.at(i)/(8*6);
		string rate_name = to_string ((int )(rate.at(i)/1000));
		string command = "dd if=/dev/zero of="+rate_name+  " bs="+to_string(rate_in_byte) +" count=1";
		system(command.c_str());

	}
	return 0;
}