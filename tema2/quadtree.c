/* HORDUNA Emilian - 313CD */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Pixel {                         // definirea structurii unui pixel de culoare
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

typedef struct node {                          // definirea structurii unui nod de arbore
    pixel color;
    int x, y;
    int dim;
    int is_leaf;
    struct node *fiu1;
    struct node *fiu2;
    struct node *fiu3;
    struct node *fiu4;
} TreeNode, *Tree;

typedef struct queueNode {                     // definirea structurii unui nod din coada
    TreeNode *node;
    struct queueNode *next;
} queueNode;


typedef struct queue {                         // structura stats coada
    queueNode *front;
    queueNode *rear;
} queue;

Tree createNode(pixel color, int x, int y, int dim) {           // alocarea dinamica a memoriei pt un nod si 
	Tree root = malloc(sizeof(TreeNode));                       // atribuirea valorilor
    root->color = color;
    root->x = x;
    root->y = y;
    root->dim = dim;
	root->fiu1 = NULL;
	root->fiu2 = NULL;
    root->fiu3 = NULL;
    root->fiu4 = NULL;
	return root;
}

pixel medie(pixel** image, int x, int y, int dim, unsigned long long* mean) {       // calcularea mediei
    unsigned long long red_sum = 0, green_sum = 0, blue_sum = 0;
    for(int i = x; i < x + dim; i++) {
        for(int j = y; j < y + dim; j++) {
            red_sum += (unsigned long long)image[i][j].red;
            green_sum += (unsigned long long)image[i][j].green;
            blue_sum += (unsigned long long)image[i][j].blue;
        }
    }
    unsigned long long red = red_sum / (dim * dim);
    unsigned long long green = green_sum / (dim * dim);
    unsigned long long blue = blue_sum / (dim * dim);
    unsigned long long sqr_red = 0, sqr_green = 0, sqr_blue = 0;
    for(int i = x; i < x + dim; i++) {
        for(int j = y; j < y + dim; j++) {
            sqr_red = (red - image[i][j].red) * (red - image[i][j].red);
            sqr_green = (green - image[i][j].green) * (green - image[i][j].green);
            sqr_blue = (blue - image[i][j].blue) * (blue - image[i][j].blue);
            *mean += (sqr_red + sqr_green + sqr_blue);
        }
    }
    
    *mean /= 3;
    *mean /= dim;
    *mean /= dim;
    //printf("%lld\n", *mean);
    return (pixel) {red, green, blue};
}

TreeNode* quadTree(pixel **image, int x, int y, int dim, int stop, int nivel) {         // crearea arborelui de compresie
    TreeNode* node = createNode(image[x][y], x, y, dim);
    if(dim == 1) return node;
    unsigned long long mean = 0;                                                        // injumatatirea spatiului din matrice se opreste
    node->color = medie(image, x, y, dim, &mean);                                       // atunci cand dimensiunea = 1
    if(floor(mean) > stop) {
        node->fiu1 = quadTree(image, x, y, dim / 2, stop, nivel + 1);
        node->fiu2 = quadTree(image, x, y + (dim / 2), dim / 2, stop, nivel + 1);
        node->fiu3 = quadTree(image, x + (dim / 2), y + (dim / 2), dim / 2, stop, nivel + 1);
        node->fiu4 = quadTree(image, x + (dim / 2), y, dim / 2, stop, nivel + 1);      // recursiv pt fiecare nod copil
    }
    return node;
}

int inaltime(Tree root) {                                                               // functie care calculeaza inaltimea arborelui
    if(root == NULL) return 0;
    int height1 = inaltime(root->fiu1);
    int height2 = inaltime(root->fiu2);
    int height3 = inaltime(root->fiu3);
    int height4 = inaltime(root->fiu4);
    int max = 0;
    if(height1 >= height2 && height1 >= height3 && height1 >= height4) max = height1;
    else if(height2 >= height1 && height2 >= height3 && height2 >= height4) max = height2;
    else if(height3 >= height1 && height3 >= height2 && height3 >= height4) max = height3;
    else max = height4;
    return max + 1;
}

void frunzee (Tree node, int *frunze) {                                              // calculam numarul de frunze din arbore
    if (node->fiu1 == NULL && node->fiu2 == NULL && node->fiu3 == NULL && node->fiu4 == NULL){
        *frunze += 1;
        return;
    }
    frunzee(node->fiu1, frunze);
    frunzee(node->fiu2, frunze);
    frunzee(node->fiu3, frunze);
    frunzee(node->fiu4, frunze);
}

void largest_leaf(Tree node, int *max, int currSize) {                              // calculam dimensiunea celei mai mari frunze
    if(node == NULL) return;
    if(node->fiu1 == NULL && node->fiu2 == NULL && node->fiu3 == NULL && node->fiu4 == NULL) {
        if (currSize > *max)
            *max = currSize;
        return;
    }
    largest_leaf(node->fiu1, max, currSize / 2);
    largest_leaf(node->fiu2, max, currSize / 2);
    largest_leaf(node->fiu3, max, currSize / 2);
    largest_leaf(node->fiu4, max, currSize / 2);
}

void este_frunza(Tree node) {                                                   // functie prin care distingem dintre frunze si noduri interne
    if(node == NULL) return;
    if(node->fiu1 == NULL && node->fiu2 == NULL && node->fiu3 == NULL && node->fiu4 == NULL) {
        node->is_leaf = 1;
    }
    else {
        node->is_leaf = 0;
        if(node->fiu1 != NULL) este_frunza(node->fiu1);
        if(node->fiu2 != NULL) este_frunza(node->fiu2);
        if(node->fiu3 != NULL) este_frunza(node->fiu3);
        if(node->fiu4 != NULL) este_frunza(node->fiu4);
    }
}

int isQueueEmpty(queue *q) {                                                    // functie care verifica daca coada e goala
    return q->front == NULL;
}

void enqueue(queue *q, TreeNode *node){                                         // functie prin care se aduga elemente in coada
	queueNode *nou = (queueNode *) malloc (sizeof(queueNode));
    nou->node = node;
    nou->next = NULL;
    if(q->rear == NULL) {
        q->front = nou;
        q->rear = nou;
        return;
    }
    q->rear->next = nou;
    q->rear = nou;
}

Tree dequeue(queue *q) {                                                        // functie prin care se scot elemente din coada
    if(q->front == NULL) return NULL;
    TreeNode *node = q->front->node;
    queueNode *aux = q->front;
    q->front = q->front->next;
    if(q->front == NULL) q->rear = NULL;
    free(aux);
    return node;
}

void bfs(TreeNode *root, FILE *out_bin) {                                       // functie prin care arborele de compresie este
    queue *q = (queue *) malloc (sizeof(queue));                                // parcurs nivel cu nivel, iar valorile sunt puse
    q->front = NULL;                                                            // intr-un fisier
    q->rear = NULL;
    char zero = 0;
    char one = 1;
    enqueue(q, root);
    while(!isQueueEmpty(q)) {
        TreeNode *node = dequeue(q);
        if(node->is_leaf == 0) {
            fwrite(&zero, sizeof(char), 1, out_bin);
        }
        if(node->is_leaf == 1) {
            fwrite(&one, sizeof(char), 1, out_bin);
            fwrite(&node->color.red, sizeof(unsigned char), 1, out_bin);
            fwrite(&node->color.green, sizeof(unsigned char), 1, out_bin);
            fwrite(&node->color.blue, sizeof(unsigned char), 1, out_bin);
        }
        if(node->fiu1 != NULL) enqueue(q, node->fiu1);
        if(node->fiu2 != NULL) enqueue(q, node->fiu2);
        if(node->fiu3 != NULL) enqueue(q, node->fiu3);
        if(node->fiu4 != NULL) enqueue(q, node->fiu4);
    }
}

void reverse(TreeNode *root, FILE *in) {                                                // functie care se foloseste de elemntele de culoare
    queue *coada = (queue *) malloc (sizeof(queue));                                    // si informatia is_leaf dintr-un fisier si creaza
    coada->front = NULL;                                                                // un arbore cuaternar
    coada->rear = NULL;
    enqueue(coada, root);
    while(!isQueueEmpty(coada)) {
        TreeNode *node = dequeue(coada);
        fread(&node->is_leaf, sizeof(char), 1, in);
        if(node->is_leaf == 0) {
            node->fiu1 = createNode(node->color, 0, 0, 0);
            node->fiu2 = createNode(node->color, 0, 0, 0);
            node->fiu3 = createNode(node->color, 0, 0, 0);
            node->fiu4 = createNode(node->color, 0, 0, 0);
            enqueue(coada, node->fiu1);
            enqueue(coada, node->fiu2);
            enqueue(coada, node->fiu3);
            enqueue(coada, node->fiu4);
        }
        else {
            fread(&(node->color.red), sizeof(unsigned char), 1, in);
            fread(&(node->color.green), sizeof(unsigned char), 1, in);
            fread(&(node->color.blue), sizeof(unsigned char), 1, in);
        }
    }
}

void printTree(TreeNode *root, int lvl){
    if (!root)
        return;
    printf("lvl: %d, r: %d, g: %d, b: %d\n", lvl, root->color.red, root->color.green, root->color.blue);
    printTree(root->fiu1, lvl + 1);
    printTree(root->fiu2, lvl + 1);
    printTree(root->fiu3, lvl + 1);
    printTree(root->fiu4, lvl + 1);
}

void rebuild_matrix(TreeNode *root, pixel ***re_matrix, int x, int y, int dim) {                    // functie care recreaza o matrice (imagine) 
    if(root->is_leaf == 1) {                                                                        // pornind de la un arbore cuatternar
        printf("r: %d, g: %d, b: %d\n", root->color.red, root->color.green, root->color.blue);
        for (int i = x; i < x + dim; i++)
            for (int j = y; j < y + dim; j++){
                (*re_matrix)[i][j].red = root->color.red;
                (*re_matrix)[i][j].green = root->color.green;
                (*re_matrix)[i][j].blue = root->color.blue;
            }
    }
    else {
        rebuild_matrix(root->fiu1, re_matrix, x, y, dim / 2);
        rebuild_matrix(root->fiu2, re_matrix, x, y + dim / 2, dim / 2);
        rebuild_matrix(root->fiu3, re_matrix, x + dim / 2, y + dim / 2, dim / 2);
        rebuild_matrix(root->fiu4, re_matrix, x + dim / 2, y, dim / 2);
    }
}

int main(int argc, char *argv[]) {
    FILE *in, *out;
    if (argc < 2) {
        printf("Error!\n");
        return 1;
    }
    unsigned char tip[5];
    int width = 0, height = 0, x = 0, y = 0, stop = -1;
    if(strcmp(argv[1], "-d") == 0) {                                    // se rezolva cerinta 3
        in = fopen(argv[2], "rb");
        out = fopen(argv[3], "wb"); 
        fread(&width, sizeof(int), 1, in);
        TreeNode *root = (TreeNode *) calloc (1, sizeof(TreeNode));     // alocam dinamic memorie pt arbore
        reverse(root, in);                                              // functie de realizare a arborelui de decompresie
        printTree(root, 0);
        pixel **re_matrix = (pixel **) calloc (width, sizeof(pixel *));
        for (int i = 0; i < width; i++)
            re_matrix[i] = calloc(width, sizeof(pixel));
        rebuild_matrix(root, &re_matrix, 0, 0, width);                  // dupa realizarea arborelui, recreem matricea (imaginea)
        fprintf(out, "P6\n%d %d\n255\n", width, width);                 // printam datele intr-un fisier in format PPM
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < width; j++) {
                fwrite(&re_matrix[i][j].red, sizeof(unsigned char), 1, out);
                fwrite(&re_matrix[i][j].green, sizeof(unsigned char), 1, out);
                fwrite(&re_matrix[i][j].blue, sizeof(unsigned char), 1, out);

            }
        }
    }
    else {                                                              // se rezolva cerintele 1 si 2
    in = fopen(argv[3], "rb");
    fread(tip, sizeof(unsigned char), 3, in);                           // citirea datelor din fisierul in format PPM
    unsigned char c = 10;
    fread(&c, sizeof(unsigned char), 1, in);
    while(c != ' ') {
        width = width * 10 + c - '0';
        fread(&c, sizeof(unsigned char), 1, in);
    }
    while (c != '\n')
    {
        fread(&c, sizeof(unsigned char), 1, in);
    }
    fread(tip, sizeof(unsigned char), 4, in);                           // [tip] e redundant, noi cunoastem deja valoarea
    height = width;                                                     // maxima a unui pixel de culoare
    pixel **image = (struct Pixel **) malloc (height * sizeof(struct Pixel *));
    for(int i = 0; i < height; i++) {
        image[i] = (pixel *) malloc (width * sizeof(pixel));            // alocam dinamic spatiul pt o matrice de structuri rgb
    }
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            fread(&image[i][j].red, sizeof(char), 1, in);               // citim informatia din matricea de structuri
            fread(&image[i][j].green, sizeof(char), 1, in);
            fread(&image[i][j].blue, sizeof(char), 1, in);
        }
    }
    int nivel = 0;
    stop = atoi(argv[2]);                                               // factorul de compresie
    Tree root = quadTree(image, x, y, width, stop, nivel);              // functie care creaza arborele de compresie
    int depth = inaltime(root);                                         // functie care calculeaza inaltimea arborelui
    int frunze = 0;
    frunzee(root, &frunze);                                             // functie care calculeaza numarul de frunze ale arborelui
    int max_dim_fr = 0;
    largest_leaf(root, &max_dim_fr, width);                             // functie care gaseste cea mai mare dimensiune a unei frunze
    if (strcmp(argv[1], "-c1") == 0) {                                  // se rezolva prima cerinta
        out = fopen(argv[4], "wt");
        fprintf(out, "%d\n%d\n%d\n", depth, frunze, max_dim_fr);        // afisam valorile cerute
    } else {                                                            // se rezolva a doua cerinta
        out = fopen(argv[4], "wb");
        este_frunza(root);                                              // functie care face distinctia dintre noduri interne si frunze
        fwrite(&height, sizeof(unsigned int), 1, out);
        bfs(root, out);                                                 // parcurgem arborele pe niveluri ca in cadrul algoritmului bfs
    }                                                                   // si scriem valorile rgb intr-un fisier
    fclose(in);
    fclose(out);
    }
    return 0;
}