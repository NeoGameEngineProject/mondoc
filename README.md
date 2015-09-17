# Mondoc
Mondoc is a simple tool to generate documentation from Lua sources in a way similar to doxygen.
It has a modular design that allows creating custom output generators to allow a wide range of output options.
The current default is the HTML generator which also is the only one that is complete at this point of time.

* Reads all comments in a Lua file and processes them for clean output
* Produces specialized output for functions, variables and free standing comments
* Supports example code blocks in comments
* HTML generator generates complete webpage for each module
* Only requires JS if syntax highlighting for inline code examples

# Usage
```
mondoc -o <output> <input files>
  -i: The file used to produce the index.html
  -o: Selects the output directory
```
Example: 
``` 
mondoc -i main.lua -o documentation utils.lua stuff.lua
```

# Example Lua script
```lua
--- This is a main script
-- @module MainScript

--- This is a variable
local var = 123

--- This is a function
-- Example usage:
-- @code
-- -- This is a comment!
-- print(f(123))
-- @endcode
--
-- @param p Some parameter
-- @return Return value!
function f(p)
  return p
end

--- Some random title
-- Some random comment that has the title "Some random title"
-- RANDOMNESS!
```
