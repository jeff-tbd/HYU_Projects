//2013012148 LeeJaeil 508

#include <cstdio>
#include <vector>
#include <queue>
using namespace std;

typedef struct _GraphEdge {
	int to;
	int weight;
} GraphEdge;

typedef struct _Vertex {
	int index;
	int selected;
	int weight;
} Vertex;

int main()
{
	int N, numOfEdges, fromEdge, toEdge;
	vector<Vertex> vertices;
	vector< vector<GraphEdge> > edges;
	Vertex inputVertex = { 0, -1 };
	queue<Vertex> q;

	scanf("%d", &N);
	vertices.resize(N + 1);
	edges.resize(N + 1);

	for (int i = 1; i <= N; ++i) {
		vertices[i].index = i;
		vertices[i].selected = 0;
		vertices[i].weight = -1;
	}

	while (scanf("%d%d", &fromEdge, &numOfEdges) != EOF) {

		edges[fromEdge].resize(numOfEdges);
		for (int i = 0; i < numOfEdges; ++i) {
			scanf("%d%d", &edges[fromEdge][i].to, &edges[fromEdge][i].weight);
		}
	}

	vertices[1].selected = 1;
	vertices[1].weight = 0;
	q.push(vertices[1]);

	while (!q.empty()) {
		Vertex curVertex = q.front();
		const int curIndex = curVertex.index;
		q.pop();

		// if the value is changed
		//if (curVertex.weight != vertices[curIndex].weight) continue;
		vertices[curIndex].selected = 1;

		const int edgesSize = edges[curIndex].size();
		for (int i = 0; i < edgesSize; ++i) {
			int to = edges[curIndex][i].to;
			int toWeight = edges[curIndex][i].weight;
			if (curVertex.weight + toWeight < vertices[to].weight || vertices[to].weight == -1) {
				const Vertex prevVertex = vertices[to];
				vertices[to].weight = curVertex.weight + toWeight;
			}
		}

		int minValue = 2147483647;
		int minIndex = -1;

		for (int i = 1; i <= N; ++i) {

			if (!vertices[i].selected && minValue > vertices[i].weight && vertices[i].weight != -1) {
				minValue = vertices[i].weight;
				minIndex = i;
			}
		}

		if (minIndex != -1) q.push(vertices[minIndex]);
	}

	int maxValue = -1;
	for (int i = 1; i <= N; ++i) {
		//printf("%d ", vertices[i].weight);
		maxValue = max(maxValue, vertices[i].weight);
	}

	printf("%d\n", maxValue);
	return 0;
}