#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <tuple>
#include <thread>
#include <future>

using namespace std;

const int NUMBER_OF_TREES = 25000;
const int TRUE_NUMBER_OF_TREES = NUMBER_OF_TREES + 4;
const int LAST_INDEX_NUMBER = TRUE_NUMBER_OF_TREES - 1;
const int DIMENSION = 25000;

/*
10000 = 3.502
25000 = 21.502
*/

struct Tree {
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

int tree_area(int left, int right, int up, int down)
{
	return (right - left)*(down - up);
}

// the original solution extracted from original main()
//
// @param trees to be considered (unsorted)
// @param sort_fn the sort function to use: either bubble_sort above or stdlib qsort wrapped (below)
// @param startTreeIndex 0-based index of first tree to consider (needed to divide and conquer in multithreaded)
// @param pastEndTreeIndex 0-based index of last tree to consider (needed to divide and conquer in multithreaded)
//
// @return (max_area, elapsedTime)
std::tuple<int, float> solve(Tree* trees, void (*sort_fn)(Tree*), int startTreeIndex, int pastEndTreeIndex) {
	//2. start the timer and sort the array on x, then y if x the same
	const clock_t begin_time = clock();
	sort_fn(trees);

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

	for (int i = startTreeIndex; i < pastEndTreeIndex; ++i)
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
	return std::make_tuple(max_area, elapsedTime);
}

// Dump results to stdout
void dump_result(std::tuple<int, float> result) {
	int max_area = std::get<0>(result);
	float elapsedTime = std::get<1>(result);
	cout << "\tMaximum area: " << max_area << endl;
	cout << "\tElapsed time for " << NUMBER_OF_TREES << " trees in a " << DIMENSION << " * " << DIMENSION << " was: " << elapsedTime << endl;
}

/*******************************************************************/
/* My mods to speed up                                             */
/*                                                                 */
/* - use stdlib qsort instead of bubble sort                       */
/*******************************************************************/

// compare function for qsort
int tree_cmp(const void *lhs, const void *rhs) {
	if (((Tree*)lhs)->x == ((Tree*)rhs)->x) {
		return ((Tree*)lhs)->y - ((Tree*)rhs)->y;
	}
	return ((Tree*)lhs)->x - ((Tree*)rhs)->x;
}

// wrap stdlib qsort in void* fn to pass to algorithm
void stdlib_qsort_wrapped(Tree* trees) {
	qsort((void*)trees, TRUE_NUMBER_OF_TREES, sizeof(Tree), tree_cmp);
}

// Helper to run solve and return max area as future.
void async_solve(std::promise<int> && p, Tree* trees, int startTreeIndex, int pastEndTreeIndex) {
	auto result = solve(trees, stdlib_qsort_wrapped, startTreeIndex, pastEndTreeIndex);
	p.set_value(std::get<0>(result));
}

// Run 'solve' in four threads and provide the elapsed time.  
std::tuple<int, float> solve_multithreaded(Tree* trees) {
	const clock_t begin_time = clock();
	std::promise<int> promise1;
	std::promise<int> promise2;
	std::promise<int> promise3;
	std::promise<int> promise4;
	auto future1 = promise1.get_future();
	auto future2 = promise2.get_future();
	auto future3 = promise3.get_future();
	auto future4 = promise4.get_future();
	int gradient = TRUE_NUMBER_OF_TREES / 4;
	std::thread thread1(&async_solve, std::move(promise1),trees,0, gradient);
	std::thread thread2(&async_solve, std::move(promise2), trees, gradient, gradient*2);
	std::thread thread3(&async_solve, std::move(promise3), trees, gradient*2, gradient*3);
	std::thread thread4(&async_solve, std::move(promise4), trees, gradient*3, TRUE_NUMBER_OF_TREES);
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	float elapsedTime = float(clock() - begin_time) / CLOCKS_PER_SEC;
	int max_area = max(max(max(future1.get(),future2.get()),future3.get()),future4.get());
	return std::make_tuple(max_area, elapsedTime);
}

/*******************************************************************/

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

	cout << "original solution: 1 thread + bubble_sort" << endl;
	auto result = solve(trees, bubble_sort, 0, TRUE_NUMBER_OF_TREES);
	dump_result(result);

	cout << endl;
	cout << "modified solution: 1 thread + stdlib qsort" << endl;
	result = solve(trees, stdlib_qsort_wrapped, 0, TRUE_NUMBER_OF_TREES);
	dump_result(result);

	cout << endl;
	cout << "modified solution: 4 threads + stdlib qsort" << endl;
	result = solve_multithreaded(trees);
	dump_result(result);

}//end main