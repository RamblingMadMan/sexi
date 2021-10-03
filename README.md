# C++ **S**-**ex**pression L**i**brary

> **WARNING**: This library is experimental and under heavy-development.

Sexi is an easy to use s-expression library for C11 and C++17.

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
#include <fstream>
#include <iostream>
#include <iterator>

#include "sexi.h"

int main(){
	std::ifstream file("test.se");
	std::string str(std::istreambuf_iterator<char>{file}, {});

	auto result = sexi::parse(str);
	
	for(auto &&expr : result){
		std::cout << expr.toStr() << '\n';
    }
}
```

Or you can declare s-expressions inline with the `_se` user-defined literal and `<<` operator:

```c++
#include "sexi/literals.hpp"

using namespace sexi::literals;

int main(){
	auto expr =
		("add"_se << 1 << 2) <<
		("sub"_se << 3 << 4) <<
		("mul"_se << 5 << 6) <<
		("div"_se << 7 << 8);
	
	std::ifstream file("test.se");
	std::string str(std::istreambuf_iterator<char>{file}, {});
	
	auto result = sexi::parse(str);
	
	assert(result.exprs() == expr.toList());
}
```

## Roadmap / TODO

- [ ] Add proper numeric types/conversions for values.
- [ ] Handle operators/punctuation correctly.
- [ ] Add binary s-expression encode/decode.
