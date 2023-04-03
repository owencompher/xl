# xl (e*X*tended by *L*ambda calculus)
is an esolang and [lambda calculus](https://en.wikipedia.org/wiki/Lambda_calculus)
interpreter that acts as a preprocessor for almost any other programming 
language. The point is to implement functions at the lowest possible level by
using the concept of lambda abstractions.

### Purpose
xl combines lambda calculus and [any](README.md#extendable-languages) 
other programming language in a minimalistic way, resulting in two uses:
- a way to learn and use lambda calculus, and have a tangible thing that does
something
- an extension to small languages that have no or limited methods for 
abstractions (I had [brainfuck](https://esolangs.org/wiki/Brainfuck) in mind)

### Developer Contact
There may still be bugs, so please try out the converter and try to break it. If
you do discover unexpected behavior (the program may still crash or freeze; this
is not nescesarily unexpected, I just haven't implemented syntax/error checking
yet) please contact me at O2C#9358 on Discord or any other contact method you 
know or can find.

## Using The Program
Clone or download the repository, and run `make` to build the converter file. 
Alternatively, you can download a prebuilt release.

The executable 'xlc' is a simple program that converts xl code from stdin to
stdout. You can use `cat <infile> | ./xlc` to view the converted code, or add
`> <outfile>` to the end to output into a file.

By default, the program checks for `{`, `}`, and `\` to define lambda 
abstractions. If you want to use different "control characters", you can specify
them in a string as the first argument to the program. For example, 
`./xlc " (.)"` uses `()` in place of `{}` and `.` in place of `\`. The first 
character in the string (a space in the example) is reserved for future use and 
(for now) will be discarded. You might have to escape some characters, like `\` 
or `"`.

The repo includes an example xl program [example.bfx](example.bfx), which was 
used to test each release. Run `cat example.bfx | ./xlc` and the output should 
appear in the console. It should be valid brainfuck code, and running it in a 
brainfuck interpreter should output  
```
7
12345
```

## xl Definition
There are 3 "control characters" in xl, by default `{`,`}`, and `\`, which are
used to define lambda abstractions .

For example, `{x\x}` is equivalent to λx.x (the identity function) in formal 
lambda calculus.

The global lambda expression is read left to right, repeatedly popping the next
valid term of the stack and evaluating it. There are three cases for the term:

- a single character is discarded and sent to the output.

- a block (in the form `{...}`) drops its outer brackets and is pushed back onto
the stack, so that the subsequent terms off the stack are the individual terms
in the block. Since the stack is read left-to-right, this preserves the
left-associative property of formal lambda calculus.

- an abstraction (in the form `{...\...}`), for each character before the slash,
replaces that character's occurance in the body after the slash with a term 
popped off the stack, and then pushes the body, without the outer brackets, back
onto the stack. **If there is a sub-term in the body which also binds that 
character, it is not replaced in that sub-term.** (I'm not quite sure why, but 
this results in alpha-reduction being unnescessary, and drastically simplifies 
the conversion). Multiple characters before the slash `{abc\...}` are treated as
nested lambda terms `{a\{b\{c\...}}}`, to implement currying. 

### Example
Starting with the code `{fn\f{fn}}{ab}c`:

 `{fn\f{fn}}` is popped off the stack, which is now `{ab}c`.

Occurances of `f` are replaced with the next term, `{ab}`, in `{n\f{fn}}` and the
result, `{n\{ab}{{ab}n}}`, is pushed back onto the stack, which is now 
`{n\{ab}{{ab}n}}c`.

`{n\{ab}{{ab}n}}` is popped off the stack, leaving `c`.
 
Occurances of `n` are replaced with `c` in `{n\{ab}{{ab}n}}` resulting in
`{\{ab}{{ab}c}}`, which is pushed back onto the now-empty stack without the 
`{\}`. The rest is just single characters in blocks:

`{ab}{{ab}c}` -> `ab{{ab}c}` -> `b{{ab}c}` -> `{{ab}c}` -> `{ab}c` -> `abc` ->
`bc` -> `c` -> done, with `ababc` put in the output.


### Alternate Definition
Just because xl uses `{x\x}` instead of λx.x doesn't mean that xl code isn't
a valid lambda term. xl is still lambda calculus, except that all characters
that don't eventually get replaced in lambda abstractions are considered to
have been already bound to the identity function. If you add the fact that
some characters or combinations of characters also have a side effect *outside*
of lambda calculus (as in, their definition in their own language), then
interpreting xl code the same way you would any other lambda calculus should
give the same result.

## Writing xl Code and Limitations
### Global Functions
As seen in [example.bfx](example.bfx), global functions can be declared at the
beginning of the code and defined at the end, like so:
```
{ab\
... program, which can use a and b
}
{definition of a (can use b)}
{definition of b}
```
Since `a` will be replaced before `b`, it can use `b` in the definition as long
as it is never used inside an abstraction that binds `b`. In general, when
declaring/defining functions in this way, functions that are declared earlier
can use functions that are delared later.

An alternate way is to manually define groups of functions:
```
{ab\ {\cd
... program, can use a, b, c, and d
}
{definition of c (can use a, b, and d)}
{definition of d (can use a and b)}
}
{definition of a (can use b, c, and d)}
{definition of b (can use c and d)}
```
In this case, `c` and `d` can use `a` and `b` because `a` and `b` will be
replaced in their body when the outer term (which contains their definitions)
is evaluated. `a` and `b` can use `c` and `d` because `c` and `d` will be 
replaced in their bodies after they are substitited into the main program. This
is different from currying:  
`{a\{b\ main }} {a def} {b def}` ≠ `{a\{b\ main } {a def}} {b def}`.

Note that if functions in the `c` and `d` group contain functions from the `a` 
and `b` group *and* vice versa, it will effectively evaluate to a recursive call
to `c` or `d` in their definitions. Unfortunately, **recursion is not possible 
in xl**. The recursive call will just be evaluated as an undefined character and
put in the output.

### Numbers
One of the best uses I found when testing xl with brainfuck (which, to be 
honest, is the only language I've tested it with) was repeating blocks of code 
certain amounts of time. You can use Church numerals for this; `{fn\f{fn}}` and
`{fn\f{f{fn}}}` for 2 and 3, but you if don't ever pass other functions, you can
use simpler versions like `{f\ff}` and `{f\fff}`. Note that math is different in
each case: with Church numerals, `23x` is the same as the Church numeral 9 
(3<sup>2</sup>) applied to `x`, but the simpler version will get stuck in 
infinite recursion while evaluating. `2{3x}` does work as multiplication in both
cases.

### File Inclusion / Libraries
It isn't too hard to use multiple files in one program, you just need to
concatenate them before they are sent to xlc. `cat <file1> <file2>` does this
automatically; as an example, you might write file `lib.bfx`:
```
{fn\{f{fn}}}
{fn\{f{f{fn}}}}
{fn\f{f{f{f{fn}}}}}
{fn\f{f{f{f{f{f{fn}}}}}}}
```
which defines the Church numerals 2, 3, 5, and 7.  
Then in something like `main.bfx`:
```
{2357\
... main program, using 2, 3, 5, and 7
}
```
Running `cat main.bfx lib.bfx | ./xlc` will evaluate as intended, with 2, 3, 5
and 7 defined in the main program. If you wanted to enforce specific names (so
that you can use library functions inside of other library functions, for
example) you could add a closing `}` to the definitions and define a header file:
```
{2357\
```
then run `cat header.bfx main.bfx lib.bfx | ./xlc`.

## Extendable Languages
The program doesn't remove characters that weren't used in lambda abstractions,
including whitespace. I haven't tested it at all for any other 
languages, but if there are 3 characters available to be used exclusively by xl,
the language should be extendable. There are certain cases where whitespace is 
discarded (like in the bound characters of lambda abstractions, or when the next
term to be used in a replacement is a whitespace character), so you will 
probably have the best luck with languages that don't care about that. Notice in
the `example.bfx` file output how much of the text is preserved.
