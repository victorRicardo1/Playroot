#include <iostream>
#include <string>

#define MAX 4
#define MIN 2

struct Game {
    int id;
    std::string name;
    std::string genre;
    std::string sinopse;
};

struct BTreeNode {
    Game item[MAX + 1];
    int count;
    BTreeNode *linker[MAX + 1];
};

BTreeNode *root = nullptr;

// Criação do nodo
BTreeNode *createNode(Game game, BTreeNode *child) {
    BTreeNode *newNode = new BTreeNode;
    newNode->item[1] = game;
    newNode->count = 1;
    newNode->linker[0] = child;  // Dirá que o primeiro link apontará para o filho passado
    for (int i = 1; i <= MAX; i++) {
        newNode->linker[i] = nullptr;
    }
    return newNode;
}

// Add um valor ao nodo
void addValToNode(Game game, int pos, BTreeNode *node, BTreeNode *child) {
    int j = node->count;
    while (j > pos) {
        node->item[j + 1] = node->item[j];
        node->linker[j + 1] = node->linker[j];
        j--;
    }
    node->item[j + 1] = game;
    node->linker[j + 1] = child;
    node->count++;
}

// Dividir nodo de acordo com a ordem
void splitNode(Game game, Game *pval, int pos, BTreeNode *node,
               BTreeNode *child, BTreeNode **newNode) {
    int median, j;

    if (pos > MIN)
        median = MIN + 1;
    else
        median = MIN;

    *newNode = new BTreeNode;
    j = median + 1;
    while (j <= MAX) {
        (*newNode)->item[j - median] = node->item[j];
        (*newNode)->linker[j - median] = node->linker[j];
        j++;
    }
    node->count = median;
    (*newNode)->count = MAX - median;

    if (pos <= MIN) {
        addValToNode(game, pos, node, child);
    } else {
        addValToNode(game, pos - median, *newNode, child);
    }
    *pval = node->item[node->count];
    (*newNode)->linker[0] = node->linker[node->count];
    node->count--;
}

// Função suporte para inserção na árvore
int setValueInNode(Game game, Game *pval,
                   BTreeNode *node, BTreeNode **child) {
    int pos;
    if (!node) {
        *pval = game;
        *child = nullptr;
        return 1;
    }

    if (game.id < node->item[1].id) {
        pos = 0;
    } else {
        for (pos = node->count; game.id < node->item[pos].id && pos > 1; pos--);
        if (game.id == node->item[pos].id) {
            std::cout << "Duplicates not allowed\n";
            return 0;
        }
    }
    if (setValueInNode(game, pval, node->linker[pos], child)) {
        if (node->count < MAX) {
            addValToNode(*pval, pos, node, *child);
        } else {
            splitNode(*pval, pval, pos, node, *child, child);
            return 1;
        }
    }
    return 0;
}

// Inserção
void insertion(Game game) {
    Game pval;
    BTreeNode *child;

    if (setValueInNode(game, &pval, root, &child)) {
        root = createNode(pval, child);
    }
}

//Area de exclusão

bool deleteFromNode(BTreeNode *node, int id) {
    int pos;
    if (!node)
        return false;

    bool found = false;
    for (pos = 1; pos <= node->count; pos++) {
        if (id < node->item[pos].id) {
            found = deleteFromNode(node->linker[pos - 1], id);
            break;
        } else if (id == node->item[pos].id) {
            found = true;
            if (node->linker[pos]) {
                // Complex logic for non-leaf nodes
            } else {
                // Simple case for leaf nodes
                for (int i = pos; i < node->count; i++) {
                    node->item[i] = node->item[i + 1];
                }
                node->count--;
            }
            break;
        }
    }
    if (!found && pos > node->count) {
        found = deleteFromNode(node->linker[pos - 1], id);
    }
    // Further code to handle underflow conditions and node merging
    return found;
}

void deleteNode(BTreeNode *&root, int id) {
    if (!deleteFromNode(root, id)) {
        std::cout << "ID not found in the tree.\n";
    } else {
        if (root && root->count == 0) {
            BTreeNode *tmp = root;
            root = root->linker[0];
            delete tmp;
        }
    }
}

// Função para mostrar todos os jogos registrados
void traversal(BTreeNode *myNode) {
    if (myNode) {
        for (int i = 0; i < myNode->count; i++) {
            traversal(myNode->linker[i]);
            std::cout << "ID: " << myNode->item[i + 1].id << ", Name: " << myNode->item[i + 1].name
                      << ", Genre: " << myNode->item[i + 1].genre << ", Synopsis: " << myNode->item[i + 1].sinopse
                      << std::endl;
        }
        traversal(myNode->linker[myNode->count]);
    }
}

//Busca de um nodo

void searchAndDisplay(BTreeNode *root, int id) {
    int i = 0;
    while (i < root->count && id > root->item[i + 1].id) {
        i++;
    }

    if (i < root->count && id == root->item[i + 1].id) {
        Game &foundGame = root->item[i + 1];
        std::cout << "Game found: ID = " << foundGame.id
                  << ", Name = " << foundGame.name
                  << ", Genre = " << foundGame.genre
                  << ", Sinopse = " << foundGame.sinopse << std::endl;
        return;  // Finaliza a função após imprimir os detalhes
    }

    if (root->linker[i] != nullptr) {
        searchAndDisplay(root->linker[i], id);  // Continua a busca no nó filho apropriado
    } else {
        std::cout << "Game with ID " << id << " not found." << std::endl;
    }
}

void buscarGenero(){

}

int main() {
//    Game game1 = {1, "Game One", "Adventure", "Explore an ancient world"};
//    insertion(game1);
//    Game game2 = {2, "Game Two", "Action", "Action-packed sequences"};
//    insertion(game2);
//    Game game3 = {3, "Game Three", "Puzzle", "Solve intricate puzzles"};
//    insertion(game3);
//    traversal(root);
//    printf("\n");
//    deleteNode(root, 2);
//    traversal(root);
//    printf("\n");
//    searchAndDisplay(root, 1);
std::cout<<"Welcome to PlayRoot"<<"n";

    return 0;
}

//test to add in Git