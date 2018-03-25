#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

using namespace std;

int main(int argc, char const *argv[])
{
	/* code */
	vector<int> vec{1,2,3,4,5,6};

	vector<int> new_vec;
	//copy(vec.begin(),vec.end(),back_inserter(new_vec));

	
    new_vec.assign(vec.begin(),vec.end());
	for(int i=0;i<vec.size();i++){
		cout<<vec[i]<<" ";
	}
    
    cout<<endl;
	for(int i=0;i<new_vec.size();i++){
		cout<<new_vec[i]<<" ";
	}
    cout<<endl;

	new_vec[0]=100;

	for(int i=0;i<vec.size();i++){
		cout<<"orignal vec's value at index "<<i<<" : "<<vec[i]<<endl;
		cout<<"new vector's value at index "<<i<<" : "<<new_vec[i]<<endl;

	}


	return 0;
}