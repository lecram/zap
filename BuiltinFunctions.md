## $ ##
$(_o_)

Return a copy of _o_.

## tname ##
tname(_o_)

Return type name of _o_.

## refc ##
refc(_o_)

Return the reference count of _o_.

## print ##
print(_s_)

Print the string _s_ to standard output.

Return the number of characters printed.

## printx ##
printx([_s1_ _o1_ _s2_ _o2_ ... _sn_ _on_])

Print the concatenation of the list items, with every even item passed through [repr](BuiltinFunctions#repr.md)().

Return the number of characters printed.

Equivalent to:

```
\def printx(list)
    total 0
    repring FALSE
    list $(list)
    \while list
        item pop(list)
        \if repring
            parcial print(repr(item))
        \else
            parcial print(item)
        total +(total parcial)
        repring not(repring)
    \ret total
```

## repr ##
repr(_o_)

Return the textual representation of _o_.

## len ##
len(_x_)

Return the length of _x_.

If _x_ is a string, return the number of characters in _x_.

If _x_ is a list, return the number of items in _x_.

If _x_ is a nable, return the number of names in _x_.

If _x_ is a dict, return the number of keys in _x_.

## arr ##
arr(_c_)

Return the character _c_ converted to a string.

## concat ##
concat(_s1_ _s2_)

Return the result of the concatenation of _s1_ and _s2_.

## join ##
join([_s1_ _s2_ ... _sn_] _sep_)

Return the result of the concatenation of _s1_, _sep_, _s2_, ... , _sep_, _sn_.

Equivalent to:

```
\def join(ss sep)
    \if not(ss)
        \ret ""
    ss $(ss)
    res pop(ss)
    \while ss
        s pop(ss)
        res concat(res sep)
        res concat(res s)
    \ret res
```

## push ##
push(_list_ _item_)

Insert _item_ at the beginning of _list_.

Return a reference to _list_.

## peek ##
peek(_list_)

Return the first item of _list_.

## pop ##
pop(_list_)

Remove the first item from _list_ and return it.

The following code doesn't change the value of _list_:

`list push(list pop(list))`

## append ##
append(_list_ _item_)

Insert _item_ as the last item of _list_.

Return a reference to _list_.

## set ##
set(_list_ _index_ _item_)

Set the _index_-th item of _list_ to _item_.

Return a reference to _list_.

If _index_ is negative, the actual index used is `+(len(list) index)`.

```
> x [0 0 0 0]
[0 0 0 0]
> set(x 1 1)
[0 1 0 0]
> set(x 1 0)
[0 0 0 0]
> set(x -(len(x) 2) 1)
[0 0 1 0]
> set(x -2 0)
[0 0 0 0]
>
```

## get ##
get(_list_ _index_)

Return the _index_-th item of _list_.

If _index_ is negative, the actual index used is `+(len(list) index)`.


## ins ##
ins(_list_ _index_ _item_)

Insert _item_ before _index_-th position in _list_.

Return a reference to _list_.

If _index_ is negative, the actual index used is `+(len(list) index)`.

If _index_ == len(_list_), append _item_ to _list_.

## ext ##
ext(_lista_ _listb_)

Extend _lista_ with all items in _listb_.

Return a reference to _lista_.

## rem ##
rem(_list_ _index_)

Remove the item in _index_-th position of _list_.

Return a reference to _list_.

If _index_ is negative, the actual index used is `+(len(list) index)`.

## has ##
has(_list_ _item_)

If _list_ contains an item equal to _item_, return TRUE.

Otherwise, return FALSE.

## setkey ##
setkey(_dict_ _key_ _value_)

Set the value of _key_ to _value_, in _dict_.

Return a reference to _dict_.

## getkey ##
getkey(_dict_ _key_ _defval_)

If _dict_ has a key equal to _key_, return its value.

Otherwise, return a copy of _defval_.

## + ##
+(_a_ _b_)

Return the sum _a_ + _b_.

## - ##
-(_a_ _b_)

Return the difference _a_ - _b_.

## `*` ##
`*`(_a_ _b_)

Return the product _a_ `*` _b_.
## / ##
/(_a_ _b_)

Return the quotient of the division _a_ / _b_.

## % ##
%(_a_ _b_)

Return the remainder of the division _a_ / _b_.

## << ##
<<(_a_ _b_)

Return _a_ shifted to the left by _b_ bits.

## >> ##
>>(_a_ _b_)

Return _a_ shifted to the right by _b_ bits.

## ? ##
?(_o_)

Return the truth value of _o_.

The truth value of an object is a value that qualifies its content under a boolean perspective.

A NONE object always has a FALSE truth value.

A boolean object has a truth value equivalent to its own value.

A numeric object has a FALSE truth value if and only if its content is equivalent to 0.

A sequence object has a FALSE truth value if and only if its length is 0.

The truth value of an object is TRUE under all other condictions.

## not ##
not(_o_)

Return the negation of the truth value of _o_.

See [?](BuiltinFunctions#?.md).

## or ##
or(_a_ _b_)

If the truth values of _a_ and _b_ are both FALSE, return FALSE.

Otherwise, return TRUE.

## and ##
and(_a_ _b_)

If the truth values of _a_ and _b_ are both TRUE, return TRUE.

Otherwise, return FALSE.

## == ##
==(_a_ _b_)

If _a_ is equal _b_, return TRUE.

Otherwise, return FALSE.

## != ##
!=(_a_ _b_)

If _a_ is not equal _b_, return TRUE.

Otherwise, return FALSE.

## < ##
<(_a_ _b_)

If _a_ is less than _b_, return TRUE.

Otherwise, return FALSE.

## > ##
>(_a_ _b_)

If _a_ is greater than _b_, return TRUE.

Otherwise, return FALSE.

## <= ##
<=(_a_ _b_)

If _a_ is less than or equal _b_, return TRUE.

Otherwise, return FALSE.

## >= ##
>=(_a_ _b_)

If _a_ is greater than or equal _b_, return TRUE.

Otherwise, return FALSE.

## node ##
node()

Return an empty nable.

## any ##
any(_list_)

If the truth value of any item of _list_ is TRUE, return TRUE.

Otherwise, return FALSE.

## all ##
all(_list_)

If the truth value of all items of _list_ is TRUE, return TRUE.

Otherwise, return FALSE.

## range ##
range(_start_ _stop_ _step_)

Return a _list_ of integers where the following expressions evaluates to TRUE:

```
==(get(list n) +(start *(step n)))
# if step is positive:
<(get(list -1) stop)
>=(+(get(list -1) step) stop)
# if step is negative:
>(get(list -1) stop)
<=(+(get(list -1) step) stop)
```

Equivalent to:

```
\def range(start stop step)
    \if <(step 0)
        stepsign -1
    \else
        stepsign 1
    list []
    counter start
    \while <(*(counter stepsign) *(stop stepsign))
        append(list counter)
        counter +(counter step)
    \ret list
```

## arity ##
arity(_func_)

Return the arity (number of parameters) of _func_.