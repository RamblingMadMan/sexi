# Fast C++ **S**-**ex**pression L**i**brary

> **WARNING**: This library is experimental and under heavy-development.

Sexi is a fast, easy to use s-expression library for C++17.

## Example

Given a file `test.se` with the following contents:

```lisp
(add 1 2)
(sub 3 4)
(mul 5 6)
(div 7 8)
```

We can parse and read the data like so:

```c++
#include <iostream>

#include "sexi.hpp"

void printExpr(const sexi::ValuePtr &val){
	if(expr->isList()){
		std::cout << "List of " << val->size() << " elements: " << val->toStr() << '\n';
		// expr->at(idx) or (*expr)[idx] for element access
	}
	else if(expr->isId()){
		std::cout << "Id: " << expr->toStr() << '\n';
	}
	else if(expr->isStr()){
		std::cout << "Str: " << expr->toStr() << '\n';
		// expr->toStr() to get the unescaped string
	}
	else if(expr->isNum()){
		std::cout << "Num: " << expr->toStr() << '\n';
	}
	else{
		// this should never be possible
		std::cout << "Unknown: " << expr->toStr() << '\n';
	}
}

int main(){
	auto exprs = sexi::parseFile("test.se");
	
	for(auto &&expr : exprs){
		printExpr(expr);
	}
}
```

Which should give us the output:

```
List of 3 elements:
Id: add
Num: 1
Num: 2
List of 3 elements:
Id: sub
Num: 3
Num: 4
List of 3 elements:
Id: mul
Num: 5
Num: 6
List of 3 elements:
Id: div
Num: 7
Num: 8
```

## Roadmap / TODO

- [ ] Add proper numeric types/conversions for values.
- [ ] Handle operators/punctuation correctly.
- [ ] Add binary s-expression parser.