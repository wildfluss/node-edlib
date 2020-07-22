# node-edlib

```bash
$ node-gyp configure
$ node-gyp build
$ node
Welcome to Node.js v12.18.0.
Type ".help" for more information.
> const addon = require('./build/Release/addon')
undefined
> console.log(addon.edit_distance('hello', 'world!'))
5
undefined
>
```
