#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>

#define MAX 100
#define MIN 50

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
    std::ifstream checkFile("games_data.txt");
    std::string line;

    // Verifica se o registro já existe no arquivo
    while (getline(checkFile, line)) {
        std::stringstream ss(line);
        std::string idStr;
        std::getline(ss, idStr, ',');
        int id = std::stoi(idStr);
        if (id == game.id) {
            checkFile.close();  // Fecha o arquivo
            return;  // Sai da função se o registro já existir
        }
    }

    // Se o registro não existe, grava no arquivo
    checkFile.close();  // Fecha o arquivo

    std::ofstream dataFile("games_data.txt", std::ios::app);
    dataFile << game.id << ',' << game.name << ',' << game.genre << ',' << game.sinopse << '\n';
    dataFile.close();
}

void writeTreeNode(BTreeNode* node, std::ofstream& indexFile) {
    if (!node) return;

    // Escreve os índices do nó no arquivo
    for (int i = 1; i <= node->count; i++) {
        indexFile << node->item[i].id;
        if (i < node->count) {
            indexFile << ',';
        }
    }
    indexFile << '\n';

    // Chama recursivamente para os filhos
    for (int i = 0; i <= node->count; i++) {
        writeTreeNode(node->linker[i], indexFile);
    }
}


void writeTreeToFile(BTreeNode* root) {
    std::ofstream indexFile("btree_index.txt");
    if (indexFile.is_open()) {
        writeTreeNode(root, indexFile);
        indexFile.close();
    } else {
        std::cerr << "Unable to open index file for writing." << std::endl;
    }
}

//Funções básicas para o funcionamento da árvore

//Inserção

BTreeNode *createNode(Game game, BTreeNode *child) {
    BTreeNode *newNode = new BTreeNode;
    newNode->item[1] = game;
    newNode->count = 1;
    newNode->linker[0] = child;
    for (int i = 1; i <= MAX; i++) {
        newNode->linker[i] = nullptr;
    }
    writeTreeToFile(newNode);
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
    writeTreeToFile(node);
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
    writeTreeToFile(*newNode);
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
        for (pos = node->count; pos > 0 && game.id < node->item[pos].id; pos--);
        if (game.id == node->item[pos].id) {
            std::cout << "Duplicates not allowed\n";
            return 0;
        }
    }

    if (setValueInNode(game, pval, node->linker[pos], child)) {
        if (node->count < MAX) {
            addValToNode(*pval, pos, node, *child);
            return 0;
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
        std::ofstream indexFile("btree_index.txt", std::ios::app);
        writeTreeNode(root, indexFile);
        indexFile.close();
    }
    writeGameData(game);
}

//Exclusão

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

//Area da modificação do nodo

void updateGameData(const Game& game) {
    std::ifstream fileIn("games_data.txt");
    std::ofstream fileOut("temp.txt");

    std::string line;
    while (std::getline(fileIn, line)) {
        std::stringstream ss(line);
        std::string idStr;
        std::getline(ss, idStr, ',');
        int id = std::stoi(idStr);
        if (id == game.id) {
            fileOut << game.id << ',' << game.name << ',' << game.genre << ',' << game.sinopse << '\n';
        } else {
            fileOut << line << '\n';
        }
    }

    fileIn.close();
    fileOut.close();

    std::remove("games_data.txt");
    std::rename("temp.txt", "games_data.txt");
}
void updateGame(BTreeNode *root, int id, const std::string& newName = "", const std::string& newGenre = "", const std::string& newSinopse = "") {
    Game* game = search(root, id);  // Reutiliza a função de busca
    if (game != nullptr) {
        // Atualiza os dados do jogo
        if (!newName.empty()) {
            game->name = newName;
            updateGameData(*game);
        }
        if (!newGenre.empty()) {
            game->genre = newGenre;
            updateGameData(*game);
        }
        if (!newSinopse.empty()) {
            game->sinopse = newSinopse;
            updateGameData(*game);
        }

        // Mostra os dados atualizados do jogo
        std::cout << "Game updated: ID = " << game->id
                  << ", Name = " << game->name
                  << ", Genre = " << game->genre
                  << ", Sinopse = " << game->sinopse << std::endl;
    } else {
        std::cout << "Game with ID " << id << " not found." << std::endl;
    }
}

//Busca

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

void deleteTree(BTreeNode* node) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i <= node->count; ++i) {
        deleteTree(node->linker[i]);
    }

    delete node;
}

void rebuildTree(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<Game> games;

    // Lê os registros do arquivo e armazena-os em um vetor de jogos
    while (getline(file, line)) {
        std::stringstream ss(line);
        Game game;
        std::string id, name, genre, sinopse;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, genre, ',');
        getline(ss, sinopse);

        // Verifica se todos os campos foram lidos corretamente
        if (id.empty() || name.empty() || genre.empty() || sinopse.empty()) {
            std::cerr << "Erro ao ler registro: " << line << std::endl;
            continue; // Ignora o registro incompleto
        }

        game.id = std::stoi(id);
        game.name = name;
        game.genre = genre;
        game.sinopse = sinopse;

        games.push_back(game);
    }
    file.close();

    // Constrói a árvore a partir dos registros lidos
    for (const auto& game : games) {
        insertion(game);
    }
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
    std::string nomeUPD, generoUPD, sinopseUPD;
    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Inserir Jogo\n";
        std::cout << "2. Excluir Jogo\n";
        std::cout << "3. Buscar Jogo\n";
        std::cout << "4. Mostrar Todos os Jogos\n";
        std::cout << "5. Filtrar jogo por genero\n";
        std::cout<<"6. Atualizar algum jogo pelo ID\n";
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
            case 6:
                int ch, chave;
                std::cout << "Qual jogo deseja modificar? Insira a chave: ";
                std::cin >> chave;
                std::cout << "\nEscolha o que deseja modificar do jogo:\n";
                std::cout << "1-Apenas o nome\n";
                std::cout << "2-Apenas o gênero\n";
                std::cout << "3-Apenas a sinopse\n";
                std::cout << "4-Todos os dados\n";
                std::cout << "Insira: ";
                std::cin >> ch;

                // Limpar o buffer de entrada
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (ch == 1) {
                    std::string nomeUPD;
                    std::cout << "Digite o nome novo: ";
                    std::getline(std::cin, nomeUPD);
                    updateGame(root, chave, nomeUPD);
                } else if (ch == 2) {
                    std::string generoUPD;
                    std::cout << "Digite o gênero novo: ";
                    std::getline(std::cin, generoUPD);
                    updateGame(root, chave, "", generoUPD);
                } else if (ch == 3) {
                    std::string sinopseUPD;
                    std::cout << "Digite a sinopse nova: ";
                    std::getline(std::cin, sinopseUPD);
                    updateGame(root, chave, "", "", sinopseUPD);
                } else if (ch == 4) {
                    std::string nomeUPD, generoUPD, sinopseUPD;
                    std::cout << "Digite o nome novo: ";
                    std::getline(std::cin, nomeUPD);
                    std::cout << "Digite o gênero novo: ";
                    std::getline(std::cin, generoUPD);
                    std::cout << "Digite a sinopse nova: ";
                    std::getline(std::cin, sinopseUPD);
                    updateGame(root, chave, nomeUPD, generoUPD, sinopseUPD);
                } else {
                    std::cout << "Opção inválida!\n";
                }
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
