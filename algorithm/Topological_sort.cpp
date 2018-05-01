//2013012148 LeeJaeil 508

#include <cstdio>
#include <stack>
#include <queue>
using namespace std;

#define MAX_VERTICE 1000

int N;
int visited[MAX_VERTICE + 1];
int graph[MAX_VERTICE + 1][MAX_VERTICE + 1];
stack<int> result;

int dfs(int curVertex) {

	visited[curVertex] = 1;
	for (int i = 1; i <= N; ++i) {
		if (graph[curVertex][i]) {

			graph[curVertex][i] = 0;
			if (visited[i] == 1) return -1;
			if (visited[i] == 0 && dfs(i) == -1) return -1;
		}
	}
	visited[curVertex] = -1;
	result.push(curVertex);
	return 0;
}

int main() {
	int x, y, i, res;

	scanf("%d", &N);

	while (scanf("%d%d", &x, &y) != EOF) {
		graph[x][y] = 1;
	}

	for (i = 1; i <= N; ++i) {
		if (!visited[i]) res = dfs(i);
		if (res == -1) break;
	}

	if (i == N + 1) {
		printf("1\n");

		while (!result.empty()) {
			printf("%d ", result.top());
			result.pop();
		}
	}
	else {
		printf("0\n");

		while (!result.empty()) {
			printf("%d ", result.top());
			result.pop();
		}
	}
	return 0;
}