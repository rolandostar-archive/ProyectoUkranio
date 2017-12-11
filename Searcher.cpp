#include "Searcher.h"
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

//public

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
		fi.push_back("ARCHIVOS/" + line);
	}return searchInFiles(fi,search);
}

vector<pair<string,pair<int,int> > > searchInIndexFileByParts(const char* indexFile, string search, int n, int all){
	vector<string> fi, files;
	ifstream fileX(indexFile);
	string line;
	while(getline(fileX,line)){
		if(strcmp(line.c_str(),""))
			fi.push_back("ARCHIVOS/" + line);	
	}
	int start = (fi.size()/all) * (n-1);
	int end = (fi.size()/all) * n;
	for(int i=start; i<end; i++){
		files.push_back(fi[i]);
	}
	return searchInFiles(files,search);
}

int createIndex(){
	int link[2];
	pid_t pid;
	char foo[4096];
	if (pipe(link)==-1 || (pid = fork()) == -1)
		cout << "Error al generar Index" << endl;
	else if(pid == 0){
		dup2 (link[1], STDOUT_FILENO);
		close(link[0]); close(link[1]);
		execl("/bin/ls", "ls", "ARCHIVOS/","-1", (char *)0);
	}else{
		close(link[1]);
		FILE* file = fopen("INDEXFILE","w");
		int bytes = read(link[0], foo, sizeof(foo));
		fprintf(file,"%.*s\n", bytes, foo);
		fclose(file);
	}
}
