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

// Função para mostrar todos os jogos registrados ordenados
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

//função para modificar algum nodo

Game* search(BTreeNode *root, int id) {
    if (root == nullptr) return nullptr;

    int i = 0;
    while (i < root->count && id > root->item[i + 1].id) {
        i++;
    }

    if (i < root->count && id == root->item[i + 1].id) {
        return &root->item[i + 1];
    }

    if (root->linker[i] != nullptr) {
        return search(root->linker[i], id);
    }

    return nullptr;
}

void updateGame(BTreeNode *root, int id, const std::string& newName = "", const std::string& newGenre = "", const std::string& newSinopse = "") {
    Game* game = search(root, id);  // Reutiliza a função de busca
    if (game != nullptr) {
        // Atualiza os dados do jogo
        if (!newName.empty()) game->name = newName;
        if (!newGenre.empty()) game->genre = newGenre;
        if (!newSinopse.empty()) game->sinopse = newSinopse;

        // Mostra os dados atualizados do jogo
        std::cout << "Game updated: ID = " << game->id
                  << ", Name = " << game->name
                  << ", Genre = " << game->genre
                  << ", Sinopse = " << game->sinopse << std::endl;
    } else {
        std::cout << "Game with ID " << id << " not found." << std::endl;
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

//Impressão dos jogos de um respectivo gênero.

void printGamesByGenre(BTreeNode *node, const std::string &genre) {
    if (node == nullptr) return;  // Condição de parada da recursão

    for (int i = 0; i < node->count; i++) {
        printGamesByGenre(node->linker[i], genre);  // Visita o filho à esquerda do item

        // Verifica se o gênero do jogo atual corresponde ao buscado
        if (node->item[i + 1].genre == genre) {
            std::cout << "Game found: ID = " << node->item[i + 1].id
                      << ", Name = " << node->item[i + 1].name
                      << ", Genre = " << node->item[i + 1].genre
                      << ", Sinopse = " << node->item[i + 1].sinopse << std::endl;
        }
    }
    printGamesByGenre(node->linker[node->count], genre);  // Visita o último filho
}

int main() {
    printGamesByGenre(root, "Adventure");
    printf("\n\n");
    updateGame(root, 1, "Superman");
    traversal(root);
    printf("\n");

    int counter;
std::cout<<"\t\t\tWelcome to PlayRoot"<<"\n";




return 0;
}