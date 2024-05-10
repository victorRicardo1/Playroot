#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>

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

// Funções para manipulação de arquivos
void writeGameData(const Game& game) {
    std::ofstream dataFile("games_data.txt", std::ios::app);
    dataFile << game.id << ',' << game.name << ',' << game.genre << ',' << game.sinopse << '\n';
    dataFile.close();
}

void writeTreeNode(BTreeNode* node) {
    std::ofstream indexFile("btree_index.txt", std::ios::app);
    indexFile << node->count;
    for (int i = 0; i < node->count; i++) {
        indexFile << ',' << node->item[i + 1].id;
    }
    indexFile << '\n';
    indexFile.close();
}

//Funções básicas para o funcionamento da árvore

BTreeNode *createNode(Game game, BTreeNode *child) {
    BTreeNode *newNode = new BTreeNode;
    newNode->item[1] = game;
    newNode->count = 1;
    newNode->linker[0] = child;
    for (int i = 1; i <= MAX; i++) {
        newNode->linker[i] = nullptr;
    }
    writeTreeNode(newNode);
    return newNode;
}

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
    writeTreeNode(node);
}

void splitNode(Game game, Game *pval, int pos, BTreeNode *node, BTreeNode *child, BTreeNode **newNode) {
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
    writeTreeNode(*newNode);
}

int setValueInNode(Game game, Game *pval, BTreeNode *node, BTreeNode **child) {
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

void insertion(Game game) {
    Game pval;
    BTreeNode *child;

    if (setValueInNode(game, &pval, root, &child)) {
        root = createNode(pval, child);
    }
    writeGameData(game);
}

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

void traversal(BTreeNode *node) {
    if (!node) return;
    int i = 0;
    for (i = 0; i < node->count; i++) {
        traversal(node->linker[i]);  // Visita o filho à esquerda do item[i + 1]
        std::cout << "ID: " << node->item[i + 1].id << ", Name: " << node->item[i + 1].name
                  << ", Genre: " << node->item[i + 1].genre << ", Synopsis: " << node->item[i + 1].sinopse << std::endl;
    }
    traversal(node->linker[i]);  // Visita o último filho após o último item
}

Game* search(BTreeNode *root, int id) {
    if (root == nullptr) return nullptr;

    int i = 0;
    while (i < root->count and id > root->item[i + 1].id) {
        i++;
    }

    if (i < root->count and id == root->item[i + 1].id) {
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

void searchAndDisplay(BTreeNode *root, int id) {
    int i = 0;
    while (i < root->count and id > root->item[i + 1].id) {
        i++;
    }

    if (i < root->count and id == root->item[i + 1].id) {
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

//Area das funções para carregar arquivos

void loadGames(std::unordered_map<int, Game>& games) {
    std::ifstream dataFile("games_data.txt");
    std::string line;
    while (getline(dataFile, line)) {
        std::stringstream ss(line);
        Game game;
        std::string id;

        getline(ss, id, ',');
        getline(ss, game.name, ',');
        getline(ss, game.genre, ',');
        getline(ss, game.sinopse);

        game.id = std::stoi(id);
        games[game.id] = game;
    }
    dataFile.close();
}

void rebuildTree(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<BTreeNode*> nodes;

    while (getline(file, line)) {
        std::stringstream ss(line);
        Game game;
        std::string id, name, genre, sinopse;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, genre, ',');
        getline(ss, sinopse);

        game.id = std::stoi(id);
        game.name = name;
        game.genre = genre;
        game.sinopse = sinopse;

        BTreeNode* node = new BTreeNode();
        node->item[1] = game;
        node->count = 1;
        nodes.push_back(node);
    }

    // Conectando nós de forma linear para simplificar
    for (int i = 0; i < nodes.size() - 1; i++) {
        nodes[i]->linker[0] = nodes[i + 1];
    }
    if (!nodes.empty()) {
        nodes.back()->linker[0] = nullptr;
    }

    root = nodes.empty() ? nullptr : nodes[0];
    file.close();
}

//Funções auxiliares para o switch case;
void inserirSwitch(int id, std::string nome, std::string genre, std::string sinopse){
    Game newGame = {id, nome, genre, sinopse};
    insertion(newGame);
}



//Principal

int main() {
    std::unordered_map<int, Game> games;
    loadGames(games);  // Carregar dados dos jogos
    rebuildTree("games_data.txt");


    std::cout << "\t\t\tWelcome to PlayRoot" << "\n";
    std::cout<<"\n\t\t    O seu gerenciador de jogos\n";
    int choice, id;
    std::string nome, genre, sinopse;
    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Inserir Jogo\n";
        std::cout << "2. Excluir Jogo\n";
        std::cout << "3. Buscar Jogo\n";
        std::cout << "4. Mostrar Todos os Jogos\n";
        std::cout << "5. Filtrar jogo por genero\n";
        std::cout << "0. Sair\n";
        std::cout << "Escolha uma opcao: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Limpa o buffer de entrada

        switch (choice) {
            case 1:
                std::cout <<"Acesso para administradores\n";
                std::cout <<"Insira o ID do jogo: ";
                std::cin >> id;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Limpa o buffer após ler um inteiro

                std::cout << "Insira o nome do jogo: ";
                std::getline(std::cin, nome);  // Usa getline para ler strings com espaços

                std::cout << "Insira o genero do jogo: ";
                std::getline(std::cin, genre);

                std::cout << "Insira a sinopse do jogo: ";
                std::getline(std::cin, sinopse);

                inserirSwitch(id, nome, genre, sinopse);
                break;
            case 2:
                // exclusão
                break;
            case 3:
                // busca
                break;
            case 4:
                std::cout << "\nMostrando todos os jogos...\n";
                traversal(root);
                break;
            case 5:
                // Filtrar genero
                break;
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                std::cout << "Opção inválida!\n";
        }
    } while (choice != 0);

    return 0;
}
