# Simple Fork Wrapper

### Examples 

One function but no return. 
```cpp
Fork f; // create a fork object
f(FunctionWrapper<int, int, int>{[](int x, int y) {
    return x + y;
}, 3, 5}); // Will call the function but no return. 
```

Two function, gets a return. Calls the first function in the main process and the second one in the sub process. Also returns the function called in the main process. 
```cpp
Fork f; 
int val = f(FunctionWrapper<int, int, int>{[](int x, int y) {
    return x + y;
}, 3, 5}, FunctionWrapper<void, std::string>{[](std::string x) {
    std::cout << x << std::endl;
}, "Hello World!"}); 

std::cout << val; // Output: 8
```
And let's seee the function wrapper; 
```cpp
FunctionWrapper<void, std::string> foo {
    [](std::string x) {
        std::cout << x << std::endl;
    }, "Hello World"
};

foo() // Output: Hello World!
```
You can return things
```cpp
FunctionWrapper<int, int, int> foo {
    [](int x, int y) {
        return x + y; 
    }, 3, 5
};

foo(); // returns 8
foo.Invoke(); // returns 8
int res = foo; // assigns 8 if the return type is convertible to type you try to assign 
```