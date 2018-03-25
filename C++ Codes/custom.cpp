#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

string customSortString(string S, string T) {
     vector<char> charArray;
     int hash[26];
     for(int i=0;i<T.length();i++){
         hash[T[i]-'a']++;
     }
    
     for(int i=0;i<S.length();i++){
         char c = S[i];
         
         while(hash[c-'a']>0){
             //charArray.push_back(c);
             cout<<c<<endl;
             hash[c-'a']--;
         }
     }
    
     for(char c='a';c<='z';c++){
         while(hash[c-'a']>0){
             //charArray.push_back(c);
             cout<<c<<endl;
             hash[c-'a']--;
         }
     }
     
     string str(charArray.begin(),charArray.end());
     return "";
}


int main(int argc, char const *argv[])
{
	cout<<customSortString("cba","abcd")<<endl;
	return 0;
}


