#include <iostream>
#include <array>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;

const int NUMBER_OF_TREES = 500;
const int TRUE_NUMBER_OF_TREES = NUMBER_OF_TREES + 4;
const int LAST_INDEX_NUMBER = TRUE_NUMBER_OF_TREES - 1;
const int DIMENSION = 500;

/*
10000 = 3.502
25000 = 21.502
*/


class Tree {
	friend int main();
public:
	int x;
	int y;

	Tree()
	{
		x = 0;
		y = 0;
	}

	Tree(int a, int b)
	{
		x = a;
		y = b;
	}

	inline bool operator < (const Tree& rhs) const {
		if (x == rhs.x)
			return y < rhs.y;
		else
			return x < rhs.x;
	};
	inline bool operator > (const Tree& rhs) const {
		if (x == rhs.x)
			return y > rhs.y;
		else
			return x > rhs.x;
	};
};


int TreeArea(int left, int right, int up, int down)
{
	return (right - left)*(down - up);
}


void bubble_sort(Tree trees[])
{
	Tree temp;
	for (int i = 0; i < LAST_INDEX_NUMBER; ++i)
	{
		for (int j = 0; j <= LAST_INDEX_NUMBER; ++j)
		{
			if (trees[j] > trees[i])
			{
				//swap them
				temp = trees[i];
				trees[i] = trees[j];
				trees[j] = temp;
			}
		}
	}
}

int main()
{

	//0. Create variables:
	//		an array for the entries
	//		upper edge, lower edge, far left edge, far right edge
	//		three trees fpr the edges

	Tree trees[TRUE_NUMBER_OF_TREES];
	trees[NUMBER_OF_TREES] = Tree(0, 0);
	trees[NUMBER_OF_TREES + 1] = Tree(DIMENSION, 0);
	trees[NUMBER_OF_TREES + 2] = Tree(0, DIMENSION);
	trees[LAST_INDEX_NUMBER] = Tree(DIMENSION, DIMENSION);

	//populate the array, range for doesn't work here, pointer issue
	for (int i = 1; i<NUMBER_OF_TREES; ++i)
	{
		trees[i] = Tree(rand() % NUMBER_OF_TREES + 1, rand() % NUMBER_OF_TREES + 1);
	}

	//test the content of the array
	/*for (auto t : trees)
	{
	cout << "{x:" << t.x << ", y:" << t.y << "}" << endl;
	}*/


	//2. start the timer and sort the array on x, then y if x the same
	const clock_t begin_time = clock();
	bubble_sort(trees);


	//test the content of the array after sort
	//cout << "after sort:"<<endl;
	/*for (auto t : trees)
	{
	cout << "{x:" << t.x << ", y:" << t.y << "}" << endl;
	}*/


	int up = 0;
	int down = DIMENSION;
	int right = DIMENSION;
	int left = 0;
	int max_area = 0;

	for (int i = 0; i < TRUE_NUMBER_OF_TREES; ++i)
	{
		up = 0;
		down = DIMENSION;
		right = DIMENSION;
		left = trees[i].x;
		int area = 0;
		bool edge = false;
		for (int j = i + 1; j < TRUE_NUMBER_OF_TREES && edge == false; ++j)
		{

			right = trees[j].x;
			area = ((right - left)* (down - up));
			if (area > max_area)
				max_area = area;

			if (trees[j].y > up && trees[j].y < trees[i].y)
			{
				up = trees[j].y;
			}
			else if (trees[j].y < down && trees[j].y >= trees[i].y)
			{
				down = trees[j].y;
			}
			area = ((right - left)* (down - up));
			if (area > max_area)
				max_area = area;
			else if (area == 0)
				edge = true;
		}//end inner for; y

		 //do the same from left edge to up edge
		up = 0;
		down = DIMENSION;
		right = trees[i].x;
		left = 0;
		bool left_edge = 0;
		for (int ed = 1; ed < i && left_edge == false; ++ed)
		{

			if (trees[ed].y > up && trees[ed].y < trees[i].y)
			{
				up = trees[ed].y;
			}
			else if (trees[ed].y < down && trees[ed].y >= trees[i].y)
			{
				down = trees[ed].y;
			}
			else if (ed == i)
				left_edge = true;
		}//end inner for; y
		area = ((right - left)* (down - up));
		if (area > max_area)
			max_area = area;
		else if (area == 0)
			left_edge = true;

	}//end outer for; x

	float elapsedTime = float(clock() - begin_time) / CLOCKS_PER_SEC;
	cout << "Maximum area: " << max_area << endl;
	cout << "Elapsed time for " << NUMBER_OF_TREES << " trees in a " << DIMENSION << " * " << DIMENSION << " was: " << elapsedTime << endl;

}//end main