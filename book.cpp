#include <sstream>
#include "book.h"
#include "util.h"
#include <iomanip>
using namespace std;

Book::Book(const string name, double price, int qty,
           const string isbn, const string author)
    : Product("book", name, price, qty), isbn_(isbn), author_(author)
{ }

Book::~Book(){}

set<string> Book::keywords() const
{
    set<string> result = parseStringToWords(name_);   // the product name
    set<string> authorWords = parseStringToWords(author_);
    result = setUnion(result, authorWords);
    result.insert(isbn_);   // ISBN goes in as-is, not parsed
    return result;
}

string Book::displayString() const
{
    stringstream ss;
    ss << name_ << "\n"
       << "Author: " << author_ << " ISBN: " << isbn_ << "\n"
       << fixed << setprecision(2) << price_ << " " << qty_ << " left.";
    return ss.str();
}

void Book::dump(ostream& os) const
{
  Product::dump(os);
  os << isbn_ << "\n" << author_ << endl;
}