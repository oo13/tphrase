# Introduction
TPhrase can generate various phrases with a syntax expressed by a text, which can be translated to generate a phrase in other languages. (The way to translate is such as gettext, but it's outside the scope of TPhrase.)

# Example
`_()` is a function to translate.

## A simple example

C++ code:
```C
#include <iostream>
#include "tphrase/Generator.h"

int main()
{
    tphrase::Generator greetings{_(R"(
        main = {HELLO}, {WORLD}!

        HELLO = Hi | Greetings | Hello | Good morning
        WORLD = world | guys | folks
    )")};

    std::cout << greetings.generate() << std::endl;
    std::cout << greetings.get_number_of_syntax()  << std::endl;
    std::cout << greetings.get_combination_number() << std::endl;
    std::cout << greetings.get_weight() << std::endl;

    return 0;
}
```

The result:
```
Hi, guys!
1
12
12
```

## Text Substitution, and Generation with an External Context

C++ code:
```C
#include <iostream>
#include "tphrase/Generator.h"

int main()
{
    tphrase::Generator greetings{_(R"(
        HELLO = Hi | Greetings | Hello | Good morning
        WORLD = world | "{GENDER}-siblings" 2 | folks ~
          /female-siblings/sisters/ ~
          /male-siblings/brothers/

        main = {HELLO}, {WORLD}!
    )")};

    std::cout << greetings.generate({ { "GENDER", "male" } }) << std::endl;
    std::cout << greetings.get_number_of_syntax()  << std::endl;
    std::cout << greetings.get_combination_number() << std::endl;
    std::cout << greetings.get_weight() << std::endl;

    return 0;
}
```

The result:
```
Hi, brothers!
1
12
16
```

"{GENDER}-siblings" is followed by 2 so the weight of "{GENDER}-siblings" is 2. The quotation is necessary if it's followed by a weight.

If you will make it translatable, the external contexts should be the range in the predefined variations and use in order to restrict the context, instead of to introduce extensibility, that is, you should tell the translator the possible combinations before translating.

## Multiple Phrase Syntaxes

C++ code:
```C
#include <iostream>
#include "tphrase/Generator.h"

int main()
{
    tphrase::Generator greetings{_(R"(
        main = I hope this modules is useful!
    )")};

    greetings.add(_(R"(
        main = This module is a libre software. You can help out by contributing {BUGS}.

        BUGS= bug reports | typo fixes | "revisions of the documents" 2
    )"));

    std::cout << greetings.generate() << std::endl;
    std::cout << greetings.get_number_of_syntax()  << std::endl;
    std::cout << greetings.get_combination_number() << std::endl;
    std::cout << greetings.get_weight() << std::endl;

    return 0;
}
```

The result:
```
I hope this modules is useful!
2
4
5
```

A phrase generator can have completely independent phrase syntaxes.

The chance to generate "I hope..." is 20%, "This module..." is 80% because latter has 4 weights. It equalizes the chance to select each phrase syntax if `greetings.equalize_chance()` is called.

## Separate Compilation

C++ code:
```C
#include <iostream>
#include "tphrase/Generator.h"

int main()
{
    tphrase::Syntax greet_syntax{_(R"(
        HELLO = Hi | Greetings | Hello | Good morning
    )")};

    greet_syntax.add(_(R"(
        WORLD = world | guys | folks
    )"));

    greet_syntax.add(_(R"(
        main = {HELLO}, {WORLD}!
    )"));

    tphrase::Generator greetings{greet_syntax};

    std::cout << greetings.generate() << std::endl;
    std::cout << greetings.get_number_of_syntax()  << std::endl;
    std::cout << greetings.get_combination_number() << std::endl;
    std::cout << greetings.get_weight() << std::endl;

    return 0;
}
```

The result:
```
Hi, guys!
1
12
12
```

The multiple compilation may make easy to understand the parse error message and edit the translation file. If the syntax has more than a few decades lines, you should consider dividing into the multiple syntaxes (In above example, it's too short to divide).

It can use to create a common library with some assignments, but the number of the combinations should be low enough for the translators to accept them.

# Syntax of the Phrase Syntax
## Overview
The phrase syntax is expressed by the 8bit plain text that UTF-8 can pass through. It may be problematic for gsub function, so the C++ coders may replace it by a gsub function that supports UTF-8, such as a function using [SRELL](https://www.akenotsuki.com/misc/srell/en/). (The unit of the column number in the error message is byte. TPhrase user cannot change it.)

The phrase syntax consists of assignments. The order of the assignments doesn't affect the generated text. The recursive reference is not allowed.

It needs a definition of the nonterminal "main" as the start condition to generate the phrase.

## Spaces
The spaces can consist of ' ' SPACE, '\\t' TAB, and the comment blocks "{* ... }".

The operators "=", ":=", "|", and "~" can be preceded by the spaces, and succeeded by spaces and one newline. (If it allowed multiple newlines, some syntax errors cause puzzling error messages.)

## Assignment
The assignment defines a nonterminal assigned to a production rule.

`nonterminal = production_rule` or `nonterminal := production_rule`

The assignments must be separated by one newline at least. The last assignment doesn't need a following newline. There can be any number of the spaces and newlines between the assignments.

The assignment operator ":=" means the production rule equalizes the chance to select each text. "=" means the chance depends on the number of the possible texts to generate and the weight set by user.

If you don't use ":=" and the weight, the chance to generate all possible phrases is equal:
```
main = {A1} | {A2}
A1 = 0 | 1 | 2
A2 = {A21} | {A22}
A21 = 3 | 4
A22 = 5 | 6 | 7 | 8 | 9
```
The each chance to generate from "0" to "9" is 10%. The chance to select {A1} is 30%, {A2} is 70%, {A21} is 20%, {A22} is 50%.

If you use ":=":
```
main = {A1} | {A2}
A1 = 0 | 1 | 2
A2 := {A21} | {A22}
A21 = 3 | 4
A22 = 5 | 6 | 7 | 8 | 9
```
The chance to select {A21} is 35%, {A22} is 35%. {A1} and {A2} aren't affected by ":=". (cf. The weight propagates the higher layers.)

The nonterminal starts with "_" is  a local nonterminal that is visible only from the same compile unit.

## Production rule

The production rule consist of options and gsubs. For example: `text1 | text2 | text3 ~ /pat1/repl1/ ~ /pat2/repl2/g`

## Options
The options are texts separated "|". For example: `text1 | text2 | text3`

## Text
The text is the candidate for the result of the production rule.

If a text enclose quotation ('"', "'", or "`"), the text can have any character except the quotation. If it's followed by a number, the number is the weight of the chance to select the text. There can be the spaces between the quoted text and the weight number.

```
A = text1 | "text2" 2
```
The chance of "text1" is 33%, "text2" is 67%.
```
A = text1 | {B}
B = text2 | "{C}" 2
C = 1 | 2 | 3
```
The chance of "text1" is 25%, "text2" is 25%, "{C}" is 50%. The chance of "{C}" is lower than no weight.

The text doesn't need to enclose quotations ('"', "'", or "`") if it meets these requirements:
   1. The text is not empty.
   1. The text has neither newline, "|", "~", nor "}" except for a part of an expansion.
   1. The beginning of the text is other than the spaces and quotations. (The spaces preceded by the text are not a part of the text.)
   1. The end of the text is not the spaces. (The spaces succeeded by the text are not a part of the text. The expansion is a part of the text even if the expansion results the spaces or the empty string.)
   1. The text is not followed by a weight number. (The number is a part of the text.)

The text may have expansions, which is a string enclosed by "{" and "}". The text can contain "{" only as the beginning of the expansion, and the expansion can include any character except "}". The rule is prior to the above rules, for example &quot; {&quot;} &quot; is a valid text.

## Expansion
The string enclosed by "{" and "}" is the expansion, which will be expanded into a text. "{" and "}" can enclose any character except "}". If the string enclosed "{" and "}" has only alphabet, numeric, and low line characters ("[A-Za-z0-9_]"), the enclosed string is a nonterminal.

1. If the nonterminal is assigned to a production rule, the expansion will be expanded in the generated text.
1. The local unsolved nonterminal occurs an error.
1. If the external context specifies the substitution for the global unsolved nonterminal, it's applied.
1. "{(}" and "{)}" will be expanded into "{" and "}".
1. If the beginning of the expansion is "{*", the expansion will be expanded into the empty string. (Technically speaking, it's not a comment block.)
1. If the beginning of the expansion is "{=" or "{:=", the content (except the first "=" or ":=") is considered as a production rule. For example, "{= A|B|C}" will be expanded into the result of the production rule "A|B|C". The syntax of the content is expressed by EBNF: `content = space_nl_opt, production_rule, space_nl_opt ;` "{:=" is, of course, the equalized select version of "{=".
1. The other expansion will be expanded into itself removed outer "{" and "}". (I recommend that the nonterminal is noticeable to find it easily unless you will leave it unsolved.)

## Gsub (Global substitution)
Gsub is the function to substitute the resulting string selected from the options. You can specify any number (including zero) of gsubs that substitute the string. 1st gsub specifies the substitution of the selected text out of the options, and then the result of the preceding substitution is substituted by the next gsub's.

A gsub specification follows "~", the first character except spaces means the separator in the specification. The separator character can be any 7 bit character except spaces, newline, and "{", and can differ from a separator in the other specification.

For example:
```
~ /A B/C D/g ~ !/!|!11 ~ $X Y$1 2$
```

The pattern parameter succeeds the first separator in the specification and can have any characters except the separator, but the pattern must not be the empty string.

The separator succeeds the pattern, and the replacement parameter follows the separator. The replacement can have any characters except the separator, and can be the empty string.

The separator succeeds the replacement, and the global parameter "g" may follow the separator. "g" means all the matched parts are substituted, nothing means the only first matched part is substituted.

The parameters "pattern", "replacement", "global" are compatible with the regex in the ECMAScript by default. The C++ coders can customize the gsub function.

## EBNF

```EBNF
start = space_nl_opt, [ { assignment, space_nl_opt } ], $ ;
space = " " | "\t" | ( "{*", [ { ? [^}] ? } ], "}" ) ;
nl = "\n" ;
space_nl_opt = [ { space | nl } ] ;

assignment = nonterminal, space_opt, operator, space_one_nl_opt, production_rule, ( nl | $ ) ;
nonterminal = { ? [A-Za-z0-9_] ? } ;
operator = "=" | ":=" ;
space_opt = [ { space } ] ;
space_one_nl_opt = space_opt, [ nl, space_opt ] ;

production_rule = options, gsubs ;

options = text, space_opt, [ { "|", space_one_nl_opt, text, space_opt } ] ;
text = text_begin, [ text_body, [ text_postfix ] ] |
       '"', [ { ? [^"{] ? | expansion } ], '"', space_opt, [ weight ] |
       "'", [ { ? [^'{] ? | expansion } ], "'", space_opt, [ weight ] |
       "`", [ { ? [^`{] ? | expansion } ], "`", space_opt, [ weight ] ;
text_begin = ? [^ \t\n"'`|~{}] ? | expansion ; (* "}" is the next to the text when it's in {= ...}. *)
text_body = { ? [^\n|~{}] ? | expansion } ;
text_postfix = space_opt, ( $ | '\n' | '|' | '~' | '}' ) ; (* This space_opt is greedy match. It isn't a part of the text. *)
expansion = "{", [ { ? [^}] ? } ], "}" ;
weight = ( ( { ? [0-9] ? }, [ "." ] ) | ( ".", ? [0-9] ? ) ), [ { ? [0-9] ? } ] ;

gsubs = [ { "~", space_one_nl_opt, sep, { pat }, sep2, [ { pat } ], sep2, [ "g" ], space_opt } ] ; (* 'sep2' is the same character of 'sep'. *)
sep = ? 7 bit character - [ \t\n{] ? ; (* '{' may be the beginning of the comment block. *)
pat = ? all characters ? - sep2 ; (* 'sep2' is the character precedes 'pat' in the parent 'gsubs'. *)
```

# To Build the Library
Use meson.
```
% meson setup build
% cd build
% meson compile
```

I tested on meson 1.4.1.

# To Generate C++ Library Manual
Use doxygen.

Library User's Manual:
```
% doxygen tphrase.doxygen
```

Library Developer's Manual:
```
% doxygen tphrase_internal.doxygen
```

I tested on doxygen 1.9.8.

# Unit Test
Use meson.

```
% meson setup build
% cd build
% meson test
```

I tested on meson 1.4.1.

# Requirement

TPhrase supports C++11 or later, and it's tested on C++11 and C++20. The source files uses only C++11, the public include files (under include/) uses concept if available.

# License
This file is part of TPhrase.

TPhrase is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TPhrase is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with TPhrase.  If not, see <http://www.gnu.org/licenses/>.

Copyright © 2024 OOTA, Masato
