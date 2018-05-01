//2013012148_¿Ã¿Á¿œ_508

#include<stdio.h>
#include<stdlib.h>

typedef struct Node {
	int value;
	struct Node *left_child;
	struct Node *right_child;
}Node;

typedef struct HeapType {
	Node heap[30001];
	int heap_size;
}HeapType;

Node et[30001];
int huff_size = 0;

void InsertHeap(HeapType *H, Node e) {
	int i;
	i = ++(H->heap_size);

	while ((i != 1) && (e.value < H->heap[i / 2].value)) {
		H->heap[i] = H->heap[i / 2];
		i = i / 2;
	}

	H->heap[i] = e;
}

void CreateHeap(HeapType *H, int size) {
	int i;

	H->heap_size = 0;

	for (i = 0; i < size; i++) {
		if (et[i].value) {
			InsertHeap(H, et[i]);
		}
	}
}

//pop operation from min-heap
Node *Delete(HeapType *H) {
	int parent = 1, child = 2;
	Node *e = (Node*)malloc(sizeof(Node));
	Node tmp;

	//pop min-value
	*e = H->heap[parent];
	//move last element's value to first element
	tmp = H->heap[(H->heap_size)--];

	//min heapify
	while (child <= H->heap_size) {
		//set childe which is compared to parent 
		if ((child < H->heap_size) && (H->heap[child].value > H->heap[child + 1].value)) {
			child++;
		}

		//when last element satisfy min-heap struct
		if (tmp.value < H->heap[child].value) {
			break;
		}

		//compare with child recursively
		H->heap[parent] = H->heap[child];
		parent = child;
		child *= 2;
	}

	H->heap[parent] = tmp;

	return e;
}


Node *HuffmanTree(HeapType *H) {

	Node *e1, *e2;
	Node tmp;

	while (H->heap_size > 1) {
		e1 = Delete(H);
		e2 = Delete(H);
		tmp.value = e1->value + e2->value;
		tmp.left_child = e1;
		tmp.right_child = e2;
		InsertHeap(H, tmp);
	}

	return &(H->heap[1]);
}

//calculate cost by tracing
void CheckHuff(Node *e, int size) {

	if (e->left_child) {
		CheckHuff(e->left_child, size + 1);
	}
	if (e->right_child) {
		CheckHuff(e->right_child, size + 1);
	}

	if (!(e->left_child) && !(e->right_child)) {
		huff_size += e->value * size;
	}
}

int main() {
	int N, num, S, i, k = 0;
	char name[5];
	HeapType H;
	Node *e;

	scanf("%d", &N);

	for (i = 0; i < N; i++) {
		scanf("%s %d", name, &num);
		et[i].value = num;
		et[i].left_child = NULL;
		et[i].right_child = NULL;
	}

	scanf("%d", &S);

	CreateHeap(&H, N);

	e = HuffmanTree(&H);

	CheckHuff(e, 0);

	N--;

	while (N > 0) {
		k++;
		N /= 2;
	}

	if (k == 0) {
		k++;
	}

	printf("%d\n", k * S);
	printf("huffsize :%d\n", huff_size);

	return 0;
}