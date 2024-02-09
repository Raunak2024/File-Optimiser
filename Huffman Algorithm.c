// Opening input file in read-only mode
int fd1 = open(“sample.txt”, O_RDONLY);
if (fd1 == -1) {
	perror("Open Failed For Input File:\n");
	exit(1);
}

// Creating output file in write mode
int fd2 = open(“sample - compressed.txt”, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
if (fd2 == -1) {
	perror("Open Failed For Output File:\n");
	exit(1);
}

// Structure for tree nodes
struct Node {
	char character;
	int freq;
	struct Node *l, *r;
};

// Structure for min heap
struct Min_Heap {
	int size;
	struct Node** array;
};

// Function to create min heap
struct Min_Heap* createAndBuildMin_Heap(char arr[], int freq[], int unique_size)
{
	int i;

	// Initializing heap
	struct Min_Heap* Min_Heap = (struct Min_Heap*)malloc(sizeof(struct Min_Heap));
	Min_Heap->size = unique_size;
	Min_Heap->array = (struct Node**)malloc(Min_Heap->size * sizeof(struct Node*));

	// Initializing the array of pointers in minheap.
	// Pointers pointing to new nodes of character
	// and their frequency
	for (i = 0; i < unique_size; ++i) {

		// newNode is a function
		// to initialize new node
		Min_Heap->array[i] = newNode(arr[i], freq[i]);
	}

	int n = Min_Heap->size - 1;
	for (i = (n - 1) / 2; i >= 0; --i) {

		// Standard function for Heap creation
		Heapify(Min_Heap, i);
	}

	return Min_Heap;
}

// Function to build Huffman Tree
struct Node* buildHuffmanTree(char arr[], int freq[], int unique_size)
{
	struct Node *l, *r, *top;
	while (!isSizeOne(Min_Heap)) {
		l = extractMinFromMin_Heap(Min_Heap);
		r = extractMinFromMin_Heap(Min_Heap);
		top = newNode('$', l->freq + r->freq);
		top->l = l;
		top->r = r;
		insertIntoMin_Heap(Min_Heap, top);
	}
	return extractMinFromMin_Heap(Min_Heap);
}
// Structure to store codes in compressed file
typedef struct code {
	char k;
	int l;
	int code_arr[16];
	struct code* p;
} code;

// Function to print codes into file
void printCodesIntoFile(int fd2, struct Node* root,
						int t[], int top = 0)
{
	int i;
	if (root->l) {
		t[top] = 0;
		printCodesIntoFile(fd2, root->l, t, top + 1);
	}

	if (root->r) {
		t[top] = 1;
		printCodesIntoFile(fd2, root->r, t, top + 1);
	}

	if (isLeaf(root)) {
		data = (code*)malloc(sizeof(code));
		tree = (Tree*)malloc(sizeof(Tree));
		data->p = NULL;
		data->k = root->character;
		tree->g = root->character;
		write(fd2, &tree->g, sizeof(char));
		for (i = 0; i < top; i++) {
			data->code_arr[i] = t[i];
		}
		tree->len = top;
		write(fd2, &tree->len, sizeof(int));
		tree->dec = convertBinaryToDecimal(data->code_arr, top);
		write(fd2, &tree->dec, sizeof(int));
		data->l = top;
		data->p = NULL;
		if (k == 0) {
			front = rear = data;
			k++;
		}
		else {
			rear->p = data;
			rear = rear->p;
		}
	}
}
// Function to compress file
void compressFile(int fd1, int fd2, unsigned char a)
{
	char n;
	int h = 0, i;

	// Codes are written into file in bit by bit format
	while (read(fd1, &n, sizeof(char)) != 0) {
		rear = front;
		while (rear->k != n && rear->p != NULL) {
			rear = rear->p;
		}
		if (rear->k == n) {
			for (i = 0; i < rear->l; i++) {
				if (h < 7) {
					if (rear->code_arr[i] == 1) {
						a++;
						a = a << 1;
						h++;
					}
					else if (rear->code_arr[i] == 0) {
						a = a << 1;
						h++;
					}
				}
				else if (h == 7) {
					if (rear->code_arr[i] == 1) {
						a++;
						h = 0;
					}
					else {
						h = 0;
					}
					write(fd2, &a, sizeof(char));
					a = 0;
				}
			}
		}
	}
	for (i = 0; i < 7 - h; i++) {
		a = a << 1;
	}
	write(fd2, &a, sizeof(char));
}
typedef struct Tree {
	char g;
	int len;
	int dec;
	struct Tree* f;
	struct Tree* r;
} Tree;

// Function to extract Huffman codes
// from a compressed file
void ExtractCodesFromFile(int fd1)
{
	read(fd1, &t->g, sizeof(char));
	read(fd1, &t->len, sizeof(int));
	read(fd1, &t->dec, sizeof(int));
}

// Function to rebuild the Huffman tree
void ReBuildHuffmanTree(int fd1, int size)
{
	int i = 0, j, k;
	tree = (Tree*)malloc(sizeof(Tree));
	tree_temp = tree;
	tree->f = NULL;
	tree->r = NULL;
	t = (Tree*)malloc(sizeof(Tree));
	t->f = NULL;
	t->r = NULL;
	for (k = 0; k < size; k++) {
		tree_temp = tree;
		ExtractCodesFromFile(fd1);
		int bin[MAX], bin_con[MAX];
		for (i = 0; i < MAX; i++) {
			bin[i] = bin_con[i] = 0;
		}
		convertDecimalToBinary(bin, t->dec, t->len);
		for (i = 0; i < t->len; i++) {
			bin_con[i] = bin[i];
		}

		for (j = 0; j < t->len; j++) {
			if (bin_con[j] == 0) {
				if (tree_temp->f == NULL) {
					tree_temp->f = (Tree*)malloc(sizeof(Tree));
				}
				tree_temp = tree_temp->f;
			}
			else if (bin_con[j] == 1) {
				if (tree_temp->r == NULL) {
					tree_temp->r = (Tree*)malloc(sizeof(Tree));
				}
				tree_temp = tree_temp->r;
			}
		}
		tree_temp->g = t->g;
		tree_temp->len = t->len;
		tree_temp->dec = t->dec;
		tree_temp->f = NULL;
		tree_temp->r = NULL;
		tree_temp = tree;
	}
}
void decompressFile(int fd1, int fd2, int f)
{
	int inp[8], i, k = 0;
	unsigned char p;
	read(fd1, &p, sizeof(char));
	convertDecimalToBinary(inp, p, 8);
	tree_temp = tree;
	for (i = 0; i < 8 && k < f; i++) {
		if (!isroot(tree_temp)) {
			if (i != 7) {
				if (inp[i] == 0) {
					tree_temp = tree_temp->f;
				}
				if (inp[i] == 1) {
					tree_temp = tree_temp->r;
				}
			}
			else {
				if (inp[i] == 0) {
					tree_temp = tree_temp->f;
				}
				if (inp[i] == 1) {
					tree_temp = tree_temp->r;
				}
				if (read(fd1, &p, sizeof(char)) != 0) {
					convertDecimalToBinary(inp, p, 8);
					i = -1;
				}
				else {
					break;
				}
			}
		}
		else {
			k++;
			write(fd2, &tree_temp->g, sizeof(char));
			tree_temp = tree;
			i--;
		}
	}
}
