# Fast C++ **S**-**ex**pression L**i**brary

> **WARNING**: This library is experimental and under heavy-development.

Sexi is a fast, easy to use s-expression library for C11 and C++17.

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

#include "sexi.h"

void printExpr(const sexi::Expr &expr){
    if(expr.isList()){
        std::cout << "List of " << expr.length() << " elements: " << expr.toStr() << '\n';
        // expr[idx] for element access
    }
    else if(expr.isId()){
        std::cout << "Id: " << expr.toStr() << '\n';
    }
    else if(expr.isStr()){
        std::cout << "Str: " << expr.toStr() << '\n';
    }
    else if(expr.isNum()){
        std::cout << "Num: " << expr.toStr() << '\n';
    }
    else if(expr.isEmpty()){
        std::cout << "Empty: ()\n";
    }
    else{
        // this should never be possible
        std::cout << "Unknown: " << expr.toStr() << '\n';
    }
}

int main(){
    constexpr char src[] = "(my (test \"source\") 1)";
    
    // 'src' must stay alive for life of 'exprs'
    auto exprs = sexi::parse(src);
	
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
- [ ] Add binary s-expression encode/decode.
