#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

using namespace std;

/*Індетифікатор адреси*/
#define ADDRESS_IND 			'@'

/*Розмір хеш таблиці для даних*/
#define HASHTABLE_DATA_SIZE 		5

/*Розмір хеш таблиці для адрес*/
#define HASHTABLE_CONNECTIONS_SIZE 	5

/*Кількість серверів*/
#define SERVERS_COUNT               	12

/*Вхідний файл з даними про лікарські рослини*/
#define INPUT_DATA_FILENAME			"input_data.txt"

/*Матриця зв'язків між серверами*/
bool serversConnections[SERVERS_COUNT][SERVERS_COUNT] = {
	{0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
	{1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}
};

template <typename keyT, typename dataT> class Server;

/*Структура елементу хеш таблиці*/
template <typename keyT, typename dataT> struct Node {
	keyT key;			//ключ
	dataT data;			//дані
	Node* next = NULL;	//адреса на наступний елемент
		
	Node(keyT _key, dataT _data) : key(_key), data(_data) {}
};

/*Клас хеш таблиці*/
template <typename keyT, typename dataT> class HashTable {
	protected:
		Node<keyT, dataT>** table;	//хеш таблиці
		int records;				//кількість записів
		int size;					//розмір таблиці
	
		/*Хеш функція*/
		unsigned int hashFunc(keyT key) {
			int sum = 0;
			for (int i = 0; i < key.length(); i++)
				sum += key[i];
			return sum % size;
		}
	
	public:
		HashTable(int _size) : size(_size) {
			records = 0;
			table = new Node<keyT, dataT>* [size];
			
			for (int i = 0; i < size; i++)
				table[i] = NULL;
		}
		
		/*Функція додавання нового елемента*/
		void push(keyT key, dataT data) {
			records++;
			unsigned int hashNumber = hashFunc(key);
			
			Node<keyT, dataT>* newNode = new Node<keyT, dataT>(key, data);
			Node<keyT, dataT>* hashNode = table[hashNumber];
			
			if (hashNode == NULL) {
				table[hashNumber] = newNode;
			} else {
				while (hashNode->next)
					hashNode = hashNode->next;
				hashNode->next = newNode;
			}	
		}
		
		/*Функція знаходження елемента*/
		Node<keyT, dataT>* find(keyT key) {
			unsigned int hashNumber = hashFunc(key);
			
			Node<keyT, dataT>* hashNode = table[hashNumber];
			
			while (hashNode) {
				if (hashNode->key == key)
					return hashNode;
				hashNode = hashNode->next;
			}
			return NULL;
		}
		
		/*Функція видалення елемента*/
		void pop(keyT key) {
			unsigned int hashNumber = hashFunc(key);
			
			Node<keyT, dataT>* hashNode = table[hashNumber];
			Node<keyT, dataT>* preNode;
			preNode->next;
			
			while (hashNode) {
				if (hashNode->key == key) {
					records--;
					if (hashNode->next == NULL) {
						if (table[hashNumber]->key == key) {
							table[hashNumber] = NULL;
						} else {
							(*preNode).next = NULL;
						}
					} else {
						*hashNode = *hashNode->next;
					}
					return;
				}
				preNode = hashNode;
				hashNode = hashNode->next;
			}
		}
		
		/*Функція друкування усіх ключів*/
		void printKeys() {
			keyT* keys = getKeys();
			
			for (int i = 0; i < records; i++)
				cout << "- " << keys[i] << endl;
			
			if (records == 0)
				cout << "Noting." << endl;
		}
		
		/*Функція знаходження усіх ключів*/
		keyT* getKeys() {
			keyT* keys = NULL;
			int k = 0;

			if (records)
				keys = new keyT[records];
			
			for (int i = 0; i < size; i++) {
				Node<keyT, dataT>* hashNode = table[i]; 
				
				while (hashNode != NULL) {
					keys[k++] = hashNode->key;		
			 		hashNode = hashNode->next; 	
				}
			}
			return keys;
		}
};

template <typename keyT, typename dataT> class Interface : public HashTable<keyT, Server<keyT, dataT>*> {
	protected:
		unsigned int nConn;
		
	public:
		Interface(int _mConn) : HashTable<keyT, Server<keyT, dataT>*>(_mConn) {}
		
		void addConnection(Server<keyT, dataT>* pAdress) {
			HashTable<keyT, Server<keyT, dataT>*>::push(pAdress->getAdress(), pAdress);
		}
		
		void removeConnection(keyT address) {
			HashTable<keyT, Server<keyT, dataT>*>::pop(address);
		}
		
		void sendAddInfo(keyT key, string address) {
			keyT* keys = HashTable<string, Server<keyT, dataT>*>::getKeys();
		
			for (int i = 0; i < HashTable<keyT, Server<keyT, dataT>*>::records; i++) {
				Server<keyT, dataT>* pAdress = HashTable<string, Server<keyT, dataT>*>::find(keys[i])->data;
				pAdress->addData(key, address);	
			}
		}
		
		Node<keyT, dataT>* sendGetInfo(keyT key, string address) {
			Node<keyT, Server<keyT, dataT>*>* node = HashTable<keyT, Server<keyT, dataT>*>::find(address);
			
			if (node)
				return node->data->getData(key);
			return NULL;
		}
		
		void sendRemoveInfo(keyT key) {
			Node<keyT, Server<keyT, dataT>*>* hashNode;
			for (int i = 0; i < HashTable<keyT, Server<keyT, dataT>*>::size; i++) {
				hashNode = HashTable<keyT, Server<keyT, dataT>*>::table[i];
				
				while (hashNode) {
        			hashNode->data->removeData(key);
                    hashNode = hashNode->next;  
                }
			}
		}
};

template <typename keyT, typename dataT> class Server : public HashTable<keyT, dataT>, public Interface<keyT, dataT> {
	private:
		string adress;
	
	public:
		Server(int _size, int _mConn, string _adress) : HashTable<keyT, dataT>(_size), Interface<keyT, dataT>(_mConn), adress(_adress) {};
		
		string getAdress() {return adress;}
		
		void addData(keyT key, dataT data) {
			if (HashTable<keyT, dataT>::find(key) == NULL) {
				HashTable<keyT, dataT>::push(key, data);
				Interface<keyT, dataT>::sendAddInfo(key, adress);
			}
		}
		
		Node<keyT, dataT>* getData(keyT key) {
			Node<keyT, dataT>* node = HashTable<keyT, dataT>::find(key);
			
			if (node) {
				if (node->data[0] == ADDRESS_IND)
					return Interface<keyT, dataT>::sendGetInfo(key, node->data);
				return node;	
			} 
			return NULL;
		}
		
		void removeData(keyT key) {
			Node<keyT, dataT>* node = HashTable<keyT, dataT>::find(key);
			
			if (node) {
				HashTable<keyT, dataT>::pop(key);
				Interface<keyT, dataT>::sendRemoveInfo(key);
			}
		}
};

template <typename keyT, typename dataT> class DHashTable {
	private:
		unsigned int serversCount;
		unsigned int hashtableDataSize;
		unsigned int hashtableConnectionsSize;
		Server<keyT, dataT>** servers;
		
	public:
		DHashTable(int a, int b, int c, bool serversConnections[SERVERS_COUNT][SERVERS_COUNT]) : serversCount(a), hashtableDataSize(b), hashtableConnectionsSize(c) {
			srand(time(NULL));
			
			servers = new Server<keyT, dataT>* [serversCount];
			
			for (int i = 0; i < serversCount; i++) 
				servers[i] = new Server<keyT, dataT>(hashtableDataSize, hashtableConnectionsSize, ADDRESS_IND + to_string(i + 1));
				
				for (int i = 0; i < serversCount; i++)
					for (int j = 0; j < serversCount; j++)
						if (i != j && serversConnections[i][j])
							servers[i]->addConnection(servers[j]);
		}
		
		void push(keyT key, dataT data) {
			int adress = rand() % serversCount;
			
			servers[adress]->addData(key, data);
		}
		
		dataT find(keyT key) {
			int adress = rand() % serversCount;
			
			Node<keyT, dataT>* node = servers[adress]->getData(key);
			
			if (node) return node->data;
			return "None";
		}
		
		void pop(keyT key) {
			int adress = rand() % serversCount;
			
			servers[adress]->removeData(key);
		}
		
		void printKeys() {
			int adress = rand() % serversCount;
			
			servers[adress]->HashTable<keyT, dataT>::printKeys();
		}
};

void pushInputData(DHashTable<string, string>& dHashTable) {
	ifstream ifs(INPUT_DATA_FILENAME);
	int n;
	string key, data;
	
	ifs >> n;
	getline(ifs, key);
	
	for (int i = 0; i < n; i++) {
		getline(ifs, key);
		getline(ifs, data);
		dHashTable.push(key, data);
		ifs.clear();
	}	
}

int main() {
	DHashTable<string, string> dHashTable(SERVERS_COUNT, HASHTABLE_DATA_SIZE, HASHTABLE_CONNECTIONS_SIZE, serversConnections);
	
	pushInputData(dHashTable);
	
	string command, key, data;
    
    while (true) {
        system("CLS");
        cout << "=================================" << endl;
        cout << "==directory=of=medicinal=plants==" << endl;
        cout << "============based=on=============" << endl;
        cout << "=====distributed=hash=table======" << endl;
        cout << "=================================" << endl << endl;
        
        cout << "All plants in directory:" << endl;
        dHashTable.printKeys();
        cout << endl;
        
        cout << "To add new record:" << endl;
        cout << "/add [name] [data]" << endl << endl;
        cout << "To remove record:" << endl;
        cout << "/remove [name]" << endl << endl;
        cout << "To get info:" << endl;
        cout << "/get [name]" << endl << endl;
        cout << "@dhashtable: ";
        cin >> command;
        
        if (command == "/add") {
            cout << "Name:" << endl;
            cin.ignore();
            getline(cin, key);
            cout << endl << "Data:" << endl;
            getline(cin, data);
            
            dHashTable.push(key, data);
            
        } else if (command == "/remove") {
            cout << "Name:" << endl;
            cin.ignore();
            getline(cin, key);
            dHashTable.pop(key);
            
        } else if (command == "/get") {
            cout << "Name:" << endl;
            cin.ignore();
            getline(cin, key);
        
            cout << endl << "Result data:" << endl;
            cout << dHashTable.find(key) << endl;
            
        } else {
            cout << endl << '"' << command << '"' << " is not recognized!" << endl;
        } 
        cout << endl << "Press any key ..." << endl;
        _getch();   
    }
	
	return 0;
}
