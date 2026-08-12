# Tom's Obvious Minimal Language - TOML
Minimal configuration file format representing key-value pairs, arrays, tables, inline tables, arrays of tables, integers, floats, booleans, and dates and times.


>Note: dates and times are not supported, yet.

>Note: quoted names are not supported, yet.

>Note: multi-line basic and literal strings are not supported,  yet. 

>Note: No special considerations are currently accounted for with basic or literal strings (i.e., escaping)

## Input Syntax
The supported v1.0.0 TOML syntax patterns are as follows:

```
file := (key_value|table|array-table)*
array-table := '[[' key ']]' block_members
table := '[' key ']' block_members
block_members := key_value
key_value := key '=' value
key := name ('.' name)+
name := [a-zA-Z_0-9-]+
value := (number|string|boolean|array|object)
array := "[" (number|string|object|boolean|array)* "]"
object := "{" key_value* "}"
comment := "#" .* \n
```


See the [TOML website](https://toml.io/en/) for more information, including examples.

Briefly, here is an example TOML file:

```toml
# This is a TOML document

title = "TOML Example"

[owner]
name = "Tom Preston-Werner"
dob = "1979-05-27T07:32:00-08:00"

[database]
enabled = true
ports = [ 8000, 8001, 8002 ]
data = [ ["delta", "phi"], [3.14] ]
temp_targets = { cpu = 79.5, case = 72.0 }

[servers]

[servers.alpha]
ip = "10.0.0.1"
role = "frontend"

[servers.beta]
ip = "10.0.0.2"
role = "backend"
```
This is equivalent to the following JSON file:

```json
{
  "title": "TOML Example",
  "owner": {
    "name": "Tom Preston-Werner",
    "dob": "1979-05-27T07:32:00-08:00"
  },
  "database": {
    "enabled": true,
    "ports": [
      8000,
      8001,
      8002
    ],
    "data": [
      [
        "delta",
        "phi"
      ],
      [
        3.14
      ]
    ],
    "temp_targets": {
      "cpu": 79.5,
      "case": 72
    }
  },
  "servers": {
    "alpha": {
      "ip": "10.0.0.1",
      "role": "frontend"
    },
    "beta": {
      "ip": "10.0.0.2",
      "role": "backend"
    }
  }
}
```

> Note: JSON does not support comments while TOML does support. Comments are lost during translation.