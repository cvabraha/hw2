#include "mydatastore.h"
using namespace std;

MyDataStore :: MyDataStore(){}

MyDataStore::~MyDataStore()
{
    
    for (set<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
        delete *it;
    }
    for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        delete it->second;
    }
}

void MyDataStore::addProduct(Product* p)
{
    products_.insert(p);
    set<string> kws = p->keywords();
    for (set<string>::iterator it = kws.begin(); it != kws.end(); ++it) {
        index_[*it].insert(p);  
    }
}

void MyDataStore::addUser(User* u)
{
    string key = convToLower(u->getName());
    users_[key] = u;
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type)
{
    vector<Product*> hits;
    if (terms.empty()) {
        return hits;  
    }

    set<Product*> result;
    for (size_t i = 0; i < terms.size(); i++) {
        set<Product*> current;
        map<string, set<Product*> >::iterator found = index_.find(convToLower(terms[i]));
        if (found != index_.end()) {
            current = found->second;
        }

        if (i == 0) {
            result = current;
        }
        else if (type == 0) {
            result = setIntersection(result, current);   // AND
        }
        else {
            result = setUnion(result, current);          // OR
        }
    }

    for (set<Product*>::iterator it = result.begin(); it != result.end(); ++it) {
        hits.push_back(*it);
    }
    return hits;
}

void MyDataStore::dump(ostream& ofile)
{
    ofile << "<products>" << endl;
    for (set<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
        (*it)->dump(ofile);
    }
    ofile << "</products>" << endl;

    ofile << "<users>" << endl;
    for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        it->second->dump(ofile);
    }
    ofile << "</users>" << endl;
}

void MyDataStore::addToCart(string username, Product* p)
{
    string key = convToLower(username);
    if (users_.find(key) == users_.end() || p == NULL) {
        cout << "Invalid request" << endl;
        return;
    }
    carts_[key].push_back(p);   // duplicates allowed, FIFO order
}

void MyDataStore::viewCart(string username)
{
    string key = convToLower(username);
    if (users_.find(key) == users_.end()) {
        cout << "Invalid username" << endl;
        return;
    }
    deque<Product*>& cart = carts_[key];
    int num = 1;
    for (deque<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
        cout << "Item " << num << endl;
        cout << (*it)->displayString() << endl;
        cout << endl;
        num++;
    }
}

void MyDataStore::buyCart(string username)
{
    string key = convToLower(username);
    map<string, User*>::iterator uit = users_.find(key);
    if (uit == users_.end()) {
        cout << "Invalid username" << endl;
        return;
    }
    User* user = uit->second;
    deque<Product*>& cart = carts_[key];
    deque<Product*> remaining;   // items that stay in the cart

    for (deque<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
        Product* p = *it;
        if (p->getQty() > 0 && user->getBalance() >= p->getPrice()) {
            p->subtractQty(1);
            user->deductAmount(p->getPrice());
        }
        else {
            remaining.push_back(p);
        }
    }
    cart = remaining;
}