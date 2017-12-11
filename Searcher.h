#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef SEARCHER_H_
#define SEARCHER_H_

using namespace std;

//Start
string readFile(const char* filename);
vector<int> getLinesBefore(string text, int pos);
pair<int,int> getRowColumn(string text, int pos);
vector<pair<int,int> > getSubstringRowColumn(string text, string sub);
vector<pair<string,pair<int,int> > > searchInFiles(vector<string> filenames, string search);
vector<pair<string,pair<int,int> > > searchInIndexFile(const char* indexFile, string search);
vector<pair<string,pair<int,int> > > searchInIndexFileByParts(const char* indexFile, string search, int n, int all);
int createIndex();

//End

#endif