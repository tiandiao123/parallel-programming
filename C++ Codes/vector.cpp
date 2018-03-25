#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	vector<int> v;

	for(int i=0;i<10;i++){
		v.push_back(i);
	}

	for(int i=0;i<v.size();i++){
		cout<<v[i]<<" ";
	}

	v[8] = 80;
	for(int i=0;i<v.size();i++){
		cout<<v[i]<<" ";
	}
	return 0;
}