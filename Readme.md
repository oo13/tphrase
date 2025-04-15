# TPhrase, Translatable phrase generator for C++11

This library is one of the translatable phrase generators. See [the manual](manual.md) for details.

[Lua version is here.](https://github.com/oo13/translatable_phrase_generator)

## Manual
[The manual](manual.md)

## What's the translatable phrase generator

1. The generator can generate a phrase (like a word, a name, a sentence, and a paragraph) with a syntax rule expressed by a text that can be replaced (=translated) in the run-time.
1. The syntax rule can express the phrase in many languages other than English.

For instance, [Context-free grammar of Wesnoth](https://wiki.wesnoth.org/Context-free_grammar) is a translatable phrase generator. [The phrase node in Endless Sky](https://github.com/endless-sky/endless-sky/wiki/CreatingPhrases) might be, except that it cannot replace the text of the syntax rule.

## Motive for Creating Another One
[Context-free grammar of Wesnoth](https://wiki.wesnoth.org/Context-free_grammar) can theoretically describe a phrase syntax rule in many languages, but a syntax rule that can be simply expressed a combination of words in English might not be expressed by a combination in other languages because they have inflection and so on; a syntax rule with three production rules have ten options in English may be translated into a syntax rule with one production rule have a thousand options in a language. It's hard to maintain a "combination explosion" like this. (In other hand, the expandable to all options in a single production rule means translatable. If a method doesn't allow the translator to specify all options, it may be impossible to translate potentially.)

Also a word in English cannot always translate into the same word of a language in the different sentence, and it should be translated a various word depended on the context. It causes a trouble similar to inflection.

As far as I had experienced about translating the phrase node in my translatable version of Endless Sky, the substitution is effective to handle the inflection and the like depended on the context, and the sophisticated substitution may make easy to handle the changing word order.

[Context-free grammar of Wesnoth](https://wiki.wesnoth.org/Context-free_grammar)  has no substituting functions. That's why I would create another phrase generator that wasn't compatible with it.

## Features (in comparison with Context-free grammar of Wesnoth)
- gsub (global substitution) function.
- Generator with a parameter to restrict the context.
- Equal chance to generate all possible texts by default, and the creator can change it.
- The comment blocks to maintain the translation easily.
- Don't care some white spaces for readability.

## Example of Phrase Syntax
### Avoid combination explosion
Japanese translation of Arach ship's name in Endless Sky (excerpt):
```
ARACH_START= マg | グラb | ブロg | ブロp | ブラb | モg | モb {* | ... }
ARACH_MIDDLE = aラg | aバg | グラg | グロp | aロp | プルーt {* | ... }
ARACH_NEXT = ・{ARACH_START}
ARACH_EMPTY = ""
main = {ARACH_START}{ARACH_MIDDLE}{=
         {ARACH_MIDDLE} | {ARACH_EMPTY} | {ARACH_NEXT}
       }{=
         {ARACH_MIDDLE} | {ARACH_EMPTY}
       } ~
       /ga/ガ/g ~
       /ba/バ/g ~
       /pa/パ/g ~
       /ta/タ/g ~
       /g/グ/g ~
       /b/ブ/g ~
       /p/プ/g ~
       /t/トゥ/g
```
Gsubs handle the characters that must be replaced by the combination with preceding and succeeding words.

### Inflection
```
ARTICLES = a | the | that | its
NOUNS = @apple | banana | @orange | @avocado | watermelon
main = {ARTICLES} {NOUNS} ~
       /a @/an / ~
       /@//
```

### Translate a single word into some different words
An example that an English word cannot translates into the same word in Japanese.
English version:
```
ECONOMICAL_SITUATION = poor | rich
main = You are {ECONOMICAL_SITUATION}. |
       You purchased a {ECONOMICAL_SITUATION} ship.
```

Japanese version:
```
ECONOMICAL_SITUATION = 貧乏 | 裕福
main = あなたは{ECONOMICAL_SITUATION}です。 |
       あなたは{ECONOMICAL_SITUATION}な船を購入しました。 ~
       /貧乏な船/粗末な船/ ~
       /裕福な船/豪華な船/
```
If you use a simple substitution instead of a translatable phrase generator, it cannot translate. For example:
```
    if (std::rand() > 0.5) {
        word = _("poor");
    } else {
        word = _("rich");
    }
    if (std::rand() > 0.5) {
        message = std::format(std::runtime_format(_("You are {}.")), word);
    } else {
        message = std::format(std::runtime_format(_("You purchased a {} ship.")), word);
    }
```
The translator can create only two independent messages at most but Japanese translator need four independent messages.
(std::format is a C++20 function, std::runtime_format is a C++26 function)

### Alternative to printf
This phrase generator can play a role of printf by "external context":
```
    if (money < 10000) {
        s = _("poor");
    } else {
        s = _("rich");
    }
    tphrase::Generator ph1(_("main = You are {ECONOMICAL_SITUATION}."));
    std::string r1{ph1.generate({{ "ECONOMICAL_SITUATION", s }})};
    // ...snip...
    if (cost < 10000) {
        s = _("poor");
    } else {
        s = _("rich");
    }
    tphrase::Generator ph2(_("main = You purchased a {ECONOMICAL_SITUATION} ship."));
    std::string r2{ph2.generate({{ "ECONOMICAL_SITUATION", s }})};
```
If you use a format function, it might not be translatable. For example, this is not a translatable in Japanese by gettext:
```
    if (money < 10000) {
        s = _("poor");
    } else {
        s = _("rich");
    }
    std::string r1{std::format(std::runtime_format(_("You are {}.")), s)};
    // ...snip...
    if (cost < 10000) {
        s = _("poor");
    } else {
        s = _("rich");
    }
    std::string r2{std::format(std::runtime_format(_("You purchased a {} ship.")), s)};
```
because gettext merges same words into a single entry, so "poor" can have a single translated word, but it should be translate into two different words. (so GNU gettext has pgettext() function.)

In fact, this is the best way for the translation:
```
    if (money < 10000) {
        r1 = _("You are poor.");
    } else {
        r1 = _("You are rich.");
    }
    // ...snip...
    if (cost < 10000) {
        r2 = _("You purchased a poor ship.");
    } else {
        r2 = _("You purchased a rich ship.");
    }
```
but it's not practical if the combination increases.

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
TPhrase is licensed under GPL-3.0-or-later or Apache-2.0. (I know Apache-2.0 is compatible with GPL 3.0, but with or later?)

Copyright © 2024 OOTA, Masato

This file is part of TPhrase.

TPhrase is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TPhrase is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with TPhrase.  If not, see <http://www.gnu.org/licenses/>.

OR

Licensed under the Apache License, Version 2.0 (the "License"); you may not use TPhrase except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
