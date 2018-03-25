#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

using namespace std;

bool vector_compare(const vector<int>& vect1,const vector<int>& vect2){

      return vect1[1]<vect2[1];

}

int main(int argc, char const *argv[])
{
	vector<vector<int>> v{{1,100,3},{2,3,4},{3,4,5}};
	

	sort(v.begin(),v.end(),vector_compare);

	for(int i=0;i<v.size();i++){

        for(int j=0;j<v[i].size();j++){
        	cout<<v[i][j]<<" ";
        }
        cout<<endl;
	}

	return 0;


}