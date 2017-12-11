#include <bits/stdc++.h>
using namespace std;

string readFile(const char* filename){
	ifstream fileX(filename);
	string text((istreambuf_iterator<char>(fileX)),(istreambuf_iterator<char>()));
	return text;
}

vector<int> getLinesBefore(string text, int pos){
	vector<int> positions;
	string sub("\n");
	int index = -sub.length();
	while(index < pos){
		index = text.find(sub,index+sub.length());
		if(index == string::npos) break;
		positions.push_back(index);
	}
	return positions;
}

pair<int,int> getRowColumn(string text, int pos){
	vector<int> lines;
	string salto("\n");
	lines = getLinesBefore(text,pos);
	int column = pos - lines[lines.size()-2];
	int row = lines.size();
	return pair<int,int>(row,column);
}

vector<pair<int,int> > getSubstringRowColumn(string text, string sub){
	vector<pair<int,int> > positions;
	int index = -sub.length();
	while(true){
		index = text.find(sub,index+sub.length());
		if(index == string::npos) break;
		positions.push_back(getRowColumn(text,index));
	}
	return positions;
}

vector<pair<string,pair<int,int> > > searchInFiles(vector<string> filenames, string search){
	vector<pair<string,pair<int,int> > > data;
	for(int i=0; i<filenames.size(); i++){
		string text(readFile(filenames[i].c_str()));
		vector<pair<int,int> > sub = getSubstringRowColumn(text,search);
		for(int j=0; j<sub.size(); j++) {
			data.push_back(pair<string,pair<int,int> >(filenames[i],sub[j]));
		}
	}
	return data;
}

vector<pair<string,pair<int,int> > > searchInIndexFile(const char* indexFile, string search){
	vector<string> fi;
	ifstream fileX(indexFile);
	string line;
	while(getline(fileX,line)){
		fi.push_back(line);
	}return searchInFiles(fi,search);
}

vector<pair<string,pair<int,int> > > searchInIndexFileByParts(const char* indexFile,
                                                      string search, int n, int all){
	vector<string> fi, files;
	ifstream fileX(indexFile);
	string line;
	while(getline(fileX,line)){
		fi.push_back(line);	
	}
	int start = (fi.size()/all) * (n-1);
	int end = (fi.size()/all) * n;
	for(int i=start; i<end; i++){
		files.push_back(fi[i]);
	}
	return searchInFiles(files,search);
}

int main(int argc, const char* argv[]){
	const char* indexFile = argv[1];
	string search(argv[2]);
	
	vector<pair<string,pair<int,int> > > found;
	found = searchInIndexFileByParts(indexFile,search,atoi(argv[3]),atoi(argv[4]));
	
	for(int i=0; i<found.size(); i++) {
		cout << "Found at Filename:" << found[i].first;
		cout << "\tRow:" << found[i].second.first;
		cout << ", Column:" << found[i].second.second << endl;
	}
	
	return 0;
}
