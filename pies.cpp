#include "stdafx.h"
#include "pies.h"

pies::pies( string inFile , int sampleSize , string outFile  )
{
	inputFile = inFile;
	size = sampleSize;
	outputFile = outFile;
}

pies::~pies()
{
}

void pies::process()
{
	ifstream ifs(inputFile.c_str());
	if( !ifs.is_open() )
	{
		cout<<"Error opening file";
		exit(1);
	}

	cout<<"sampling......"<<endl;
	string line;
	int64_t from=0;
	int64_t to=0;

	int t = 1;
	float pe = 0;
	float r = 0;
	unsigned long seed = 0;

	while( getline( ifs,line ))
	{	
//		if (t%1000) cout<<t<<endl;
		seed += (unsigned)time(NULL)%1000;
		srand(seed%100000000);
		
		sscanf(line.c_str(),"%ld %ld",&from,&to);

		if ( VSdegree.size() < size )
		{
			if( VSdegree.find(from) == VSdegree.end() )
			{
				VSdegree.insert( pair<int64_t,int>(from,0) );
			}
			if( VSdegree.find(to) == VSdegree.end() )
			{
				VSdegree.insert( pair<int64_t,int>(to,0) );
			}

			ES.insert( pair<int64_t,int64_t>(from,to) );
			ES.insert( pair<int64_t,int64_t>(to,from) );

			VSdegree[from]++;
			updateDegreeMap(VSdegree[from],1);
			updateDegreeMap(VSdegree[from]-1,-1);

			VSdegree[to]++;
			updateDegreeMap(VSdegree[to],1);
			updateDegreeMap(VSdegree[to]-1,-1);
				
		}
		else
		{	
			pe = ES.size() / ( (float)t * 2.0 );
			r = rand() / double(RAND_MAX+1.0);

			if ( r <= pe )
			{
				if ( VSdegree.find(from) == VSdegree.end() )
				{
					dropNode();
					VSdegree.insert(pair<int64_t,int>(from,0));
				}
				if ( VSdegree.find(to) == VSdegree.end() )
				{
					dropNode();
					VSdegree.insert(pair<int64_t,int>(to,0));
				}
			}

			if ( VSdegree.find(from) != VSdegree.end() && VSdegree.find(to) != VSdegree.end() )
			{
				ES.insert( pair<int64_t,int64_t>(from,to) );
				ES.insert( pair<int64_t,int64_t>(to,from) );

				VSdegree[from]++;
				updateDegreeMap(VSdegree[from],1);
				updateDegreeMap(VSdegree[from]-1,-1);

				VSdegree[to]++;
				updateDegreeMap(VSdegree[to],1);
				updateDegreeMap(VSdegree[to]-1,-1);
			}
		}

		t++;

	}
}

void pies::updateDegreeMap( int degree , int para )
{
	map<int,int>::iterator mt;

	if (degree <= 0)
	{
		return;
	}

	if (para > 0)
	{
		if ((mt = degreeMap.find(degree)) == degreeMap.end())
		{   
			degreeMap.insert(pair<int,int>(degree,para));
		}
		else
		{   
			mt->second += para;
		}
	}
	else if (para < 0)
	{
		mt = degreeMap.find(degree);	

		if (mt->second + para <= 0)
		{
			degreeMap.erase(mt);
		}
		else
		{
			mt->second += para;
		}
	}
}
	
	

void pies::dropNode()
{
	double max = 0;
	map<int,int>::iterator it;
	for ( it = degreeMap.begin() ; it != degreeMap.end() ; it++ )
	{
		max = max + (it->second) * float(1.0) / it->first;
	}
	
	unsigned long seed = (unsigned long)max;
	srand(seed);	

	float randomValue = rand() % ( size + 1 );

	map<int64_t,int>::iterator itVSdegree;
	int64_t rmVertex=0;
	
	float addValue = 0;
	float tempValue = 0;

	for ( itVSdegree = VSdegree.begin() ; itVSdegree != VSdegree.end() ; itVSdegree++ )
	{
		if ( itVSdegree->second == 0 ) continue;

		tempValue = size * float(1.0)/( itVSdegree->second * max);	 
		//cout<<"tempValue = "<<tempValue<<endl;
		addValue += tempValue;
		 
		if ( addValue <= randomValue )
		{
			rmVertex = itVSdegree -> first;
			updateDegreeMap( itVSdegree->second , -1 );
			VSdegree.erase( itVSdegree );
			delEdge( rmVertex );
			break;
		}
	}

	//cout<<"Node "<<rmVertex<<" is deleted "<<endl;
}
void pies::delEdge( int64_t node )
{
	multimap<int64_t,int64_t>::iterator itES;
	map<int64_t,int>::iterator itVSdegree;

	multimap<int64_t,int64_t>::iterator end;
	map<int64_t,int>::iterator endVSdegree;

	multimap<int64_t,int64_t>::iterator position = ES.lower_bound(node);
	multimap<int64_t,int64_t>::iterator upper = ES.upper_bound(node);

	multimap<int64_t,int64_t>::iterator position2;
	multimap<int64_t,int64_t>::iterator upper2;

	while( position != upper )
	{
		position2 = ES.lower_bound(position->second);
		upper2 = ES.upper_bound(position->second);
		while( position2 != upper2 )
		{
			if ( position2->second == node && position2->first != node)
			{
				itVSdegree = VSdegree.find( position2->first );
				itVSdegree->second -= 1;
				
				updateDegreeMap( itVSdegree->second , 1 );
				updateDegreeMap( (itVSdegree->second) + 1 , -1 );

				if ( itVSdegree->second <= 0 )
				{
					VSdegree.erase( itVSdegree );
				}

				if ( position2 == upper )
				{
					
					multimap<int64_t,int64_t>::iterator temp;
					temp = position2;
					temp++;
					upper = temp;
					ES.erase(position2);
				}
				else
				{
					ES.erase(position2);
				}

				break;
			}
		position2++;
		}
	position++;
	}
	ES.erase(node);
}

void pies::saveSampleGraph()
{
	cout<<"VS COUNT: "<<VSdegree.size()<<"  "<<" ES COUNT: "<<ES.size()<<endl;

	multimap<int64_t,int64_t>::const_iterator it;
	ofstream out(outputFile.c_str());
	if ( ! out.is_open() )
	{
		cout<<"Error opening file"<<endl;
		exit(1);
	}

	for ( it = ES.begin(); it != ES.end() ; it++ )
	{
		out<<it->first<<" "<<it->second<<endl;
	}

	out.close();
}
