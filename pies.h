#include "stdafx.h"

using namespace std;

class pies
{
public:
	pies( string inputFile , int sampleSize , string outputFile );
	~pies(void);

	string inputFile;
	int size;
	string outputFile;

	
	map<int64_t,int> VSdegree;//点ID，点ID的度
	map<int,int> degreeMap;//度，子图中拥有该度的点的个数，便于计算被替换概率
	multimap<int64_t,int64_t> ES;


	void process();
	void updateDegreeMap( int , int );
	void dropNode();
	void delEdge( int64_t );
	void saveSampleGraph();
};

