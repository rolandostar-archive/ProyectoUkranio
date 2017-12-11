#include "Searcher.h"
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main(int argc, const char* argv[]){
	createIndex();
	string search(argv[1]);
	
	vector<pair<string,pair<int,int> > > found;
	found = searchInIndexFileByParts("INDEXFILE",search,atoi(argv[2]),atoi(argv[3]));
	for(int i=0; i<found.size(); i++) {
		cout << "Found at Filename:" << found[i].first;
		cout << "\tRow:" << found[i].second.first;
		cout << ", Column:" << found[i].second.second << endl;
	}
	
	return 0;
}
